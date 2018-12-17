#pragma once

#include <Resource.h>
#include <Time.hpp>
#include <Input.h>
#include <GameObject.h>
#include <PhysicsComponents/RigidBody.h>
#include <PhysicsComponents/Physics.h>
#include <Math/Matrix4.h>
#include <Display.h>
#include <GameInstance.h>
#include <PhysicsComponents/BoxCollider.h>
#include <Common/TinyFile.h>
#include <CommandQueue.h>
#include <MeshImporter.h>
#include <PhysicsComponents/TrianglesCollider.h>
#include <Graphics/Texture.h>


#include "Gl3dVertexArray_3x1.h"
#include "Gl3dShader.h"
#include "Gl3dDevice.h"
#include "Gl3dFrameBuffer.h"
#include "Gl3dTexture.h"
#include "Gl3dRenderPas.h"
#include "Gl3dVertexArrayBase.h"
#include "Gl3dPbrComputer.h"

#include <thread>

struct Ligth 
{
public:
	Ligth() :
		position(Vector3::Zero()), powerdColor(Vector3::Zero()), inFragSpace(false)
	{}
	Vector3 position;
	Vector3 powerdColor;
	bool inFragSpace;
};

class FilmicBloom 
{
public:
	Ref<Gl3dShader> prefilter;
	Ref<Gl3dShader> downsample;
	Ref<Gl3dShader> upsimple;
	Ref<Gl3dShader> final;

	float _radius = 5.5f;
	float _intensity = 0.8f;
	const int kMaxIterations = 16;
	float _softKnee = 0.5f;
	//float _threshold = 0.8f;
	float _threshold = 1.16f;
	float GammaToLiner(float x) 
	{	
		return pow(x, 2.2f);
	}
	float LinerToGamma(float v)
	{
		return pow(v, 1/2.2);
	}

	Gl3dTexture*  _BaseTex = nullptr;
	void boundValues(Gl3dRenderPas& pas)
	{
		auto lthresh = GammaToLiner(_threshold);
		pas.Uniform("_Threshold", lthresh);
		pas.Uniform("_BaseTex", _BaseTex);
		
		auto knee = lthresh * _softKnee + 1e-5f;
		auto curve = Vector3(lthresh - knee, knee * 2, 0.25f / knee);
		pas.Uniform("_Curve", curve);
		pas.Uniform("_PrefilterOffs", 0.0f);

		pas.Uniform("_SampleScale", 0.5f + logh - logh_i);
		pas.Uniform("_Intensity", _intensity);
	}

	float logh;
	int logh_i;
	int iterations;

	Gl3dTexture* outBlur;

	Ref<Gl3dVertexArrayBase> quad2;
	size_t					 vertexesCount;

	Ref<Gl3dFrameBuffer> prefiltered;

	DynamicArray<Ref<Gl3dFrameBuffer>> _bludBuffer1;
	DynamicArray<Ref<Gl3dFrameBuffer>> _bludBuffer2;

	FilmicBloom() 
	{
		_bludBuffer1.Initialize(kMaxIterations);
		_bludBuffer2.Initialize(kMaxIterations);
	}

	void InitOrResize(Ref<Gl3dFrameBuffer>& fb, int w, int h)
	{
		if (fb && fb->GetColorTexture(0)->GetWidth() == w && fb->GetColorTexture(0)->GetHeight() == h)
			return;		
		fb = new Gl3dFrameBuffer(w, h, { PixelFormat::RGBA_16F });
	}

	void Blit(Gl3dTexture* src, const Ref<Gl3dFrameBuffer>& frambuffer, const Ref<Gl3dShader>& shader)
	{
		Gl3dRenderPas pass(shader.GetPtr(), frambuffer.GetPtr());
		Gl3dDevice::Viewport(frambuffer->GetColorTexture(0)->GetWidth(), frambuffer->GetColorTexture(0)->GetHeight());
		boundValues(pass);
		pass.Uniform("_MainTex", src);
		quad2->Draw(Gl3dDrawPrimitive::Triangles, vertexesCount);

	}
	Ref<Gl3dFrameBuffer> last;
	Ref<Gl3dFrameBuffer> rezult;
	void Draw(Gl3dTexture* mainTexture, Gl3dTexture* baseTexture, int tw, int th)
	{	
		logh = Mathf::Log(th, 2) + _radius - 8;
		logh_i = (int)logh;
		iterations = Mathf::Clamp(logh_i, 1, kMaxIterations);

		InitOrResize(prefiltered, tw, th);
		/* pass 0 */
		Blit(mainTexture, prefiltered, prefilter);

		last = prefiltered;

		
		for (int level = 0; level < iterations; level++)
		{
			
			InitOrResize(_bludBuffer1[level], last->GetColorTexture(0)->GetWidth() / 2,
				last->GetColorTexture(0)->GetHeight() / 2);
			/* pass 4 */
			Blit(last->GetColorTexture(0), _bludBuffer1[level], downsample);
			last = _bludBuffer1[level];
		}

		for (int level = iterations - 2; level >= 0; level--)
		{
			auto basetex = _bludBuffer1[level];
			_BaseTex = basetex->GetColorTexture(0);

			InitOrResize(_bludBuffer2[level], _BaseTex->GetWidth(),
				_BaseTex->GetHeight());

			/* pass 6 */
			Blit(last->GetColorTexture(0), _bludBuffer2[level], upsimple);
			last = _bludBuffer2[level];
		}

		_BaseTex = baseTexture;
		InitOrResize(rezult, baseTexture->GetWidth(),
			baseTexture->GetHeight());
		Blit(last->GetColorTexture(0), rezult, final);

	}
	
};


class RenderBeast 
{
	FilmicBloom bloom;

	Ref<Gl3dShader> render;
	Ref<Gl3dShader> postProcess;
	
	Ref<Gl3dShader> flat;
	Ref<Gl3dShader> debug;
	Ref<Gl3dShader> skybox;
	Ref<Gl3dShader> light;
	Ref<Gl3dShader> ssao;
	Ref<Gl3dShader> ssaoBlur;
	Ref<Gl3dShader> fxaa;
	

	Ref<Gl3dShader> albedoInstanced;
	Ref<Gl3dShader> albedo;
	Ref<Gl3dShader> shadowGeometryInstanced;
	Ref<Gl3dShader> shadowGeometry;

	Ref<Gl3dShader> godRays;
	Ref<Gl3dShader> chromaticAberration;


	Ref<Gl3dFrameBuffer> compositPas[2];
	Ref<Gl3dFrameBuffer> fxaaPas;
	Ref<Gl3dFrameBuffer> ssaoBlurPas;
	Ref<Gl3dFrameBuffer> ssaoPas;
	Ref<Gl3dFrameBuffer> lightPas;
	Ref<Gl3dFrameBuffer> albedoPas;
	Ref<Gl3dFrameBuffer> albedoPasMSAA;

	Ref<Gl3dFrameBuffer> renderPas;
	Ref<Gl3dFrameBuffer> shadowPas;
	Ref<Gl3dVertexArray_3x1> skyboxVertexArray;
	Ref<Gl3dVertexArray_3x1> debugVertexArray;
	Ref<Gl3dVertexArray_3x1> sphere;

	Ref<Gl3dVertexArrayBase> skyboxSpherical;
	Ref<Gl3dArray<Vector3>> skyboxSphericalVertexes;
	Ref<Gl3dArray<int>> skyboxSphericalIndeces;
	int skyboxSphericalN;
	Ref<Gl3dTexture> noiseTexture;
	Ref<Gl3dTexture> brdfLut;
	Ref<Gl3dTexture> envaromentHdrSMap;
	Ref<Gl3dPbrComputer> pbrComuter;

	Ref<Gl3dTexture> colorCorrectLut;

	Ref<Gl3dVertexArrayBase> quad2;
	Ref<Gl3dArray<Vector2>> quad2Vertexes;
	int quad2VertexesCount;
	Ref<Gl3dFrameBuffer> godRaysPas;

	DynamicArray<Ref<Ligth>> ligths;
	class ShaderState
	{
	public:
		ShaderState(const String& _name, Ref<Gl3dShader>& _ref, const DynamicArray<String>& macrxoses = {})
			: name(_name), ref(_ref), macroses(macrxoses)
		{}
		DynamicArray<String> macroses;
		String name;
		Ref<Gl3dShader>& ref;
	};
	DynamicArray<Ref<ShaderState>> shaderStates;
	
	const int SHADOW_SIZE_W = 4096;
	const int SHADOW_SIZE_H = 4096;
	int totalFrames = 0;
	Size lastFrameSize = { -1, -1 };
public:

	void SwapComposit();
	
	String profileString;

	Ref<Gl3dShader> gui;
	CommandQueue	renderSyncRoot;

	bool DebugDrawShadowOrhoProjection = false;
	bool DebugDrawPhysicsLens = false;
	bool ssaoEnabled = false;
	bool msaaEnabled = true;
	void RerootAllSquncys();
	int& TotalFrames();

	enum class DisplayOut
	{
		SSAO,
		COMPOSIT
	};

	DisplayOut outType = DisplayOut::COMPOSIT;

	RenderBeast();
	~RenderBeast();

	void Draw();

	int d_material_updated;
	int d_draw_calls;
	int d_total;

};

