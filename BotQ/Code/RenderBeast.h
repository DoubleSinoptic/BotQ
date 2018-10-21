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
#include <TaskTranslator.h>
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


class RenderBeast 
{
	Ref<Gl3dShader> render;
	Ref<Gl3dShader> postProcess;
	Ref<Gl3dShader> blur;
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

	Ref<Gl3dTexture> noiseTexture;
	Ref<Gl3dTexture> brdfLut;
	Ref<Gl3dTexture> envaromentHdrSMap;
	Ref<Gl3dPbrComputer> pbrComuter;

	Ref<Gl3dVertexArrayBase> quad2;
	Ref<Gl3dArray<Vector2>> quad2Vertexes;
	int quad2VertexesCount;
	Ref<Gl3dFrameBuffer> godRaysPas;
	Ref<Gl3dFrameBuffer> pingpongHFBOobject;
	Ref<Gl3dFrameBuffer> pingpongVFBOobject;
	Ref<Gl3dFrameBuffer>& getHVpingbong(bool i)
	{
		if (i == 1)
			return pingpongHFBOobject;
		else
			return pingpongVFBOobject;
	}

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
	TaskTranslator	renderSyncRoot;

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

