#include "RenderBeast.h"
#include "SSAO.h"
#include <Math/BBox3.h>
#include <Common/DynamicArray.h>
#include <Sge2Common.h>
#include <Display.h>

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

String PrepareMacroses(const String& str, const DynamicArray<String>& macroses)
{
	int pipeState = 0;
	String rez;
	for (int i = 0; i < str.Length(); i++)
	{
		if (pipeState == 0 && str[i] == '#')
			pipeState = 1;
		if (pipeState == 1 && str[i] == '\n')
			pipeState = 2;
		rez.Append(str[i]);

		if (pipeState == 2)
		{
			for (auto& x : macroses) 
				rez.Append(String::Format("#define %s\n", *x));		
			pipeState = 3;
		}
	}
	return rez;
}

void ExportShaderWithMacros(Gl3dShader* tlg, const String& filename, const DynamicArray<String>& macroses)
{

	FileStream f(filename.c_str(), OpenMode::Read);
	String content = f.AllContent();
	String vertex, fragment;
	int i = 0;
	for (; i < content.Length(); i++)
	{
		if (content[i] == '&')
			break;
		vertex += content[i];
	}
	i++;
	for (; i < content.Length(); i++)
		fragment += content[i];


	tlg->Build(PrepareMacroses(vertex, macroses).c_str(), ShaderType::VertexShader);
	tlg->Build(PrepareMacroses(fragment, macroses).c_str(), ShaderType::FragmentShader);
	tlg->Relink();
}


//void ExportShader(Gl3dShader* tlg, const String& filename)
//{
//	FileStream f(filename.c_str(), OpenMode::Read);
//	String content = f.AllContent();
//	String vertex, fragment;
//	int i = 0;
//	for (; i < content.Length(); i++)
//	{
//		if (content[i] == '&')
//			break;
//		vertex += content[i];
//	}
//	i++;
//	for (; i < content.Length(); i++)
//		fragment += content[i];
//
////#define EXPORT_DEFINE(x) \
////	vertex = String::Format("#define %s\n", x) + vertex;\
////	fragment = String::Format("#define %s\n", x) + fragment;
////#ifdef USE_INSTANCING
////	EXPORT_DEFINE("USE_INSTANCING")
////#endif
//
//	//Log("SHADER V { \n %s \n}", vertex.c_str());
//	//Log("SHADER F { \n %s \n}", fragment.c_str());
//	tlg->Build(vertex.c_str(), ShaderType::VertexShader);
//	tlg->Build(fragment.c_str(), ShaderType::FragmentShader);
//	tlg->Relink();
//}

Vector3 CUAD_VERTEX_DATA[] = {
	Vector3(-1.0f, 1.0f, 0.0f),
	Vector3(-1.0f, -1.0f, 0.0f),
	Vector3(1.0f, 1.0f, 0.0f),
	Vector3(1.0f, -1.0f, 0.0f)
};

Vector2 CUAD_TEXCOORDS_DATA[] =
{
	Vector2(0.0f, 1.0f),
	Vector2(0.0f, 0.0f),
	Vector2(1.0f, 1.0f),
	Vector2(1.0f, 0.0f)
};

unsigned short CUAD_INDECES_DATA[] = { 0, 1, 2, 2, 1 ,3 };

#define CAMERA_ANGLE (3.14f / 180.0f * 75.0)



BBox3 CalculateBBoxOfCamera(GameObject* camera, float aspect, float nearPlane, float farPlane)
{
	Vector3 eye = camera->GetPosition();
	Vector3 vZ = camera->GetForward();
	Vector3 vX = camera->GetRight();
	Vector3 vY = camera->GetUp();

	float nearPlaneHeight = tanf(CAMERA_ANGLE * 0.5f) * nearPlane;
	float nearPlaneWidth = nearPlaneHeight * aspect;
	float farPlaneHeight = tanf(CAMERA_ANGLE * 0.5f) * farPlane;
	float farPlaneWidth = farPlaneHeight * aspect;

	Vector3 nearPlaneCenter = eye + vZ * nearPlane;
	Vector3 farPlaneCenter = eye + vZ * farPlane;

	BBox3 box;
	box.ExtendPoint(Vector3(nearPlaneCenter - vX * nearPlaneWidth - vY * nearPlaneHeight));
	box.ExtendPoint(Vector3(nearPlaneCenter - vX * nearPlaneWidth + vY * nearPlaneHeight));
	box.ExtendPoint(Vector3(nearPlaneCenter + vX * nearPlaneWidth + vY * nearPlaneHeight));
	box.ExtendPoint(Vector3(nearPlaneCenter + vX * nearPlaneWidth - vY * nearPlaneHeight));
	box.ExtendPoint(Vector3(farPlaneCenter - vX * farPlaneWidth - vY * farPlaneHeight));
	box.ExtendPoint(Vector3(farPlaneCenter - vX * farPlaneWidth + vY * farPlaneHeight));
	box.ExtendPoint(Vector3(farPlaneCenter + vX * farPlaneWidth + vY * farPlaneHeight));
	box.ExtendPoint(Vector3(farPlaneCenter + vX * farPlaneWidth - vY * farPlaneHeight));
	return box;
}

float MaxOf(const Vector3& cf) 
{
	float max = cf[0];
	if (max < cf[1])
		max = cf[1];
	if (max < cf[2])
		max = cf[2];
	return max;
}

Matrix4 CalaculateProjection(GameObject* camera, const Vector3& lightDir, const BBox3& frustumBox, Vector3& dEye, Vector3& dCenter, Vector3& dUp, float& half)
{
	Matrix4 proj;
	Matrix4 lookat;
	float halfSize = frustumBox.GetSize().Length() * 0.5;
	Vector3 lookAmplyFire = -lightDir;
	Vector3 lookCenter = frustumBox.GetCenter();
	Vector3 eyePosint = lookCenter + lookAmplyFire * halfSize;

	dEye = eyePosint;
	dCenter = lookCenter;
	dUp = (lookCenter - eyePosint).Normalize().Cross(Vector3(0, 1, 0)).Normalized().Cross((lookCenter - eyePosint).Normalize());
	lookat = Matrix4::LookAt(eyePosint, lookCenter, Vector3(0, 1, 0));
	
	half = halfSize;
	proj = Matrix4::Ortho(-halfSize, halfSize, -halfSize, halfSize, 0.1, halfSize * 2 + 0.1);
	return proj * lookat;
}


void RenderBeast::SwapComposit()
{
	Ref<Gl3dFrameBuffer> tmp = compositPas[0];
	compositPas[0] = compositPas[1];
	compositPas[1] = tmp;
}

void RenderBeast::RerootAllSquncys()
{
	for (auto i : shaderStates)
	{
		try {
			i->ref = new Gl3dShader();
			ExportShaderWithMacros(i->ref.GetPtr(), i->name, i->macroses);
		}
		catch (const std::exception & ex)
		{
			Log("Shader: %s", ex.what());
		}
	}
}

int & RenderBeast::TotalFrames() {
	return totalFrames
		;
}

struct StopwacthPair 
{
	const char* name;
	double timing;
};
bool operator==(const StopwacthPair& a, const StopwacthPair& b)
{
	return a.timing == b.timing;
}
bool operator<(const StopwacthPair& a, const StopwacthPair& b)
{
	return a.timing > b.timing;
}
bool operator>(const StopwacthPair& a, const StopwacthPair& b)
{
	return a.timing < b.timing;
}

DynamicArray<StopwacthPair> stopwacthPairs;

class Stopwacth 
{
	const char* _name;
	size_t index;
	double time;
public:
	Stopwacth(const char* name) :
		time(Time::GetTotalTime()),
		_name(name)
	{
		index = stopwacthPairs.Add({});
	}
	~Stopwacth() 
	{
		stopwacthPairs[index] = { _name , Time::GetTotalTime()  - time };
	}
};

void StopwacthSwap() {
	stopwacthPairs.Clear();
}

Ref<Gl3dArray<Vector2>> CreateQuadGrid(int& count, float droble)
{
	float distance = 2.0 / droble;
	int deltaQuadCount = (2.0 / distance);
	int quadCount = deltaQuadCount * deltaQuadCount;
	int trianglesCount = quadCount * 2;
	int triangleVertexesCount = trianglesCount * 3;
	count = triangleVertexesCount;
	Ref<Gl3dArray<Vector2>> arr = new Gl3dArray<Vector2>(Gl3dArrayTarget::Array);
	arr->EnsureCapacity(triangleVertexesCount);

	const Vector2 startTriangles[] = {
		Vector2(-1, -1),
		Vector2(-1, -1) + Vector2(distance, 0),
		Vector2(-1, -1) + Vector2(distance, distance),

		Vector2(-1, -1),
		Vector2(-1, -1) + Vector2(distance, distance),
		Vector2(-1, -1) + Vector2(0, distance)
	};

	for (int j = 0; j < deltaQuadCount; j++)
	{
		for (int i = 0; i < deltaQuadCount; i++)
		{
			arr->Add(startTriangles[0] + Vector2(distance * i, distance * j));
			arr->Add(startTriangles[1] + Vector2(distance * i, distance * j));
			arr->Add(startTriangles[2] + Vector2(distance * i, distance * j));

			arr->Add(startTriangles[3] + Vector2(distance * i, distance * j));
			arr->Add(startTriangles[4] + Vector2(distance * i, distance * j));
			arr->Add(startTriangles[5] + Vector2(distance * i, distance * j));
		}
	}

	return arr;
}


RenderBeast::RenderBeast()
{
	Gl3dDevice::Configurate();




	quad2 = new Gl3dVertexArrayBase();
	{
		quad2Vertexes = CreateQuadGrid(quad2VertexesCount, 8.0);
		quad2->Attach(0, { Gl3dDataSplitType::Float2 }, quad2Vertexes.GetPtr());

	}

	debugVertexArray = new Gl3dVertexArray_3x1();
	debugVertexArray->EvaluteToStream();
	const Vector2 texcoords[] = { Vector2(0, 0),Vector2(0, 0) };
	const unsigned short v[] = { 0, 1 };
	debugVertexArray->SetTexcoords(texcoords, 2);
	debugVertexArray->SetIndexesNative((const char*)v, sizeof(v));

	unsigned short skyboxindeces[36];
	for (int i = 0; i < 36; i++)
		skyboxindeces[i] = i;
	skyboxVertexArray = new Gl3dVertexArray_3x1();
	skyboxVertexArray->SetVertexesNative((const char*)skyboxVertices, sizeof(skyboxVertices));


	shaderStates.Add(new ShaderState("./Shaders/ChromaticAberration.glsl", chromaticAberration));
	shaderStates.Add(new ShaderState("./Shaders/SsaoBlur.glsl", ssaoBlur));
	shaderStates.Add(new ShaderState("./Shaders/Ssao.glsl", ssao));
	shaderStates.Add(new ShaderState("./Shaders/Light.glsl", light));
	shaderStates.Add(new ShaderState("./Shaders/GodRays.glsl", godRays));
	shaderStates.Add(new ShaderState("./Shaders/PostProcess.glsl", postProcess));
	shaderStates.Add(new ShaderState("./Shaders/ShadowGeometry.glsl", shadowGeometry));
	shaderStates.Add(new ShaderState("./Shaders/Blur.glsl", blur));
	shaderStates.Add(new ShaderState("./Shaders/Flat.glsl", flat));
	shaderStates.Add(new ShaderState("./Shaders/Debug.glsl", debug));
	shaderStates.Add(new ShaderState("./Shaders/Albedo.glsl", albedo));
	shaderStates.Add(new ShaderState("./Shaders/AlbedoInstanced.glsl", albedoInstanced, 
		{"USE_INSTANCING"}));
	shaderStates.Add(new ShaderState("./Shaders/ShadowGeometryInstanced.glsl", shadowGeometryInstanced, 
		{"USE_INSTANCING"}));
	shaderStates.Add(new ShaderState("./Shaders/AlbedoInstanced.glsl", albedo));
	shaderStates.Add(new ShaderState("./Shaders/ShadowGeometryInstanced.glsl", shadowGeometry));
	shaderStates.Add(new ShaderState("./Shaders/SkyBox.glsl", skybox));
	shaderStates.Add(new ShaderState("./Shaders/Fxaa.glsl", fxaa));
	shaderStates.Add(new ShaderState("./Shaders/Gui.glsl", gui));
	shaderStates.Add(new ShaderState("./Shaders/Glow.glsl", glow));

	RerootAllSquncys();

	noiseTexture = new Gl3dTexture();
	noiseTexture->SetData(4, 4, PixelFormat::RGB_32F, SSAO::SSAONoise().GetData());
	noiseTexture->SetMagMinFilters(Filter::NEAREST, Filter::NEAREST);
	{
		Gl3dRenderPas unif(ssao.GetPtr(), nullptr);

		for (int i = 0; i < 64; i++)
		{
			unif.Uniform(*String::Format("samples[%d]", i), SSAO::SSAOKernel()[i]);
		}

	}
	//newport_loft.hdr
	int w, h;
	float* bits = Bitmap::LoadHdr(w, h, "/Shaders/daytime.hdr");
	Assert(bits && w > 0 && h > 0);
	pbrComuter = new Gl3dPbrComputer();
	envaromentHdrSMap = new Gl3dTexture();
	envaromentHdrSMap->SetDataF(w, h, PixelFormat::RGBA_16F, bits);
	pbrComuter->RenderMaps(envaromentHdrSMap.GetPtr());

	Gl3dDevice::ValidateContext();
#define LIGHT_TEST
#ifdef LIGHT_TEST
	Ref<Ligth> lg = new Ligth();
	//lg->powerdColor = Vector3(0.58597f, 0.5f, 0.38f) * 50.0;
	lg->powerdColor = Vector3(255.0 / 255.0, 244.0 / 255.0, 214.0 / 255.0) * 20.0;
	lg->position = Vector3(-1, 1, -1).Normalized();
	lg->position.Normalize();
	lg->inFragSpace = true;
	ligths.Add(lg);
#endif
	/*{
		Ref<Ligth> lg = new Ligth();
		lg->powerdColor = Vector3(1.1, 0.1f, 0.1f) * 50.0;
		lg->position = Vector3(1, 23, 1);
		ligths.Add(lg);
	}
	
	{
		Ref<Ligth> lg = new Ligth();
		lg->powerdColor = Vector3(0.1f, 1.1f, 0.1f) * 50.0;
		lg->position = Vector3(13, 23, 1);
		ligths.Add(lg);
	}
	{
		Ref<Ligth> lg = new Ligth();
		lg->powerdColor = Vector3(0.1f, 0.1f, 1.1f) * 50.0;
		lg->position = Vector3(1, 23, 13);
		ligths.Add(lg);
	}*/

	new Resource(StorageFunction<void(void)>([&]() { RerootAllSquncys(); }), "/Functions/ReloadShaders");
}

RenderBeast::~RenderBeast()
{
}


#define TRY_EXEC 

void RenderBeast::Draw()
{
	double frameTime = Time::GetTotalTime();

	totalFrames++;

	auto axisof = [](const Vector3& axis, float angle)
		-> Quaternion
	{
		float s = sinf(angle / 2.0f);
		Quaternion rz;
		rz.x = axis.x * s;
		rz.y = axis.y * s;
		rz.z = axis.z * s;
		rz.w = cosf(angle / 2.0f);
		return rz;
	};
	Size s = Display::GetCurrent()->GetSize();

#ifdef	USE_BORDER_MASK
	s = Size(s.width * (0.016 + 1.0), s.height * (0.016 + 1.0));
#endif
	Size outS = Display::GetCurrent()->GetSize();
	GameObject* CameraObject = Display::GetCurrent()->GetCamera();
	Matrix4 lookat = Matrix4::LookAt(CameraObject->GetPosition(), CameraObject->GetPosition() + CameraObject->GetForward(), Vector3(0, 1, 0));
	//Matrix4 lookat = Matrix4::LookAt(Vector3(24, 24, 24), Vector3(0, 0, 0) , Vector3(0, 1, 0));

	float aspectRatio = float(s.width) / float(s.height);
	Matrix4 perspective = Matrix4::PerspectiveRH(CAMERA_ANGLE, aspectRatio, 0.1f, 2000.0f);
	if (lastFrameSize != s)
	{

		/*
		out vec4 FragPositon;
		out vec4 FragNormal;
		out vec4 FragColor;
		out vec4 FragSettings;
		out vec4 FragDepth;
		out vec4 FragPBR;
		*/
		ssaoBlurPas = new Gl3dFrameBuffer(s.width / 2, s.height / 2, { PixelFormat::RGBA_16F });
		ssaoPas = new Gl3dFrameBuffer(s.width / 2, s.height / 2, { PixelFormat::RGBA_16F });

		albedoPasMSAA = new Gl3dFrameBuffer(s.width, s.height, {
			PixelFormat::RGBA_32F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F
			},
			{ PixelFormat::DEPTH_32 }, true);
		albedoPasMSAA->GetColorTexture(0)->SetMagMinFilters(Filter::NEAREST, Filter::NEAREST);
		albedoPas = new Gl3dFrameBuffer(s.width, s.height, {
			PixelFormat::RGBA_32F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F
			},
			{ PixelFormat::DEPTH_32 });
		albedoPas->GetColorTexture(0)->SetMagMinFilters(Filter::NEAREST, Filter::NEAREST);
		renderPas = new Gl3dFrameBuffer(s.width, s.height, 
			{ 
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F,
			PixelFormat::RGBA_16F 
			});
		shadowPas = new Gl3dFrameBuffer(SHADOW_SIZE_W, SHADOW_SIZE_H, { }, { PixelFormat::DEPTH_16 });	
		pingpongHFBOobject = new Gl3dFrameBuffer(s.width / 5, s.height / 5, { PixelFormat::RGBA_16F });
		pingpongVFBOobject = new Gl3dFrameBuffer(s.width / 5, s.height / 5, { PixelFormat::RGBA_16F });
		glowPas = new Gl3dFrameBuffer(s.width / 5, s.height / 5, { PixelFormat::RGBA_16F });
		fxaaPas = new Gl3dFrameBuffer(s.width, s.height, { PixelFormat::RGBA_16F });	
		godRaysPas = new Gl3dFrameBuffer(s.width, s.height, { PixelFormat::RGBA_16F });
		for(int i = 0; i < 2; i++)
			compositPas[i] = new Gl3dFrameBuffer(s.width, s.height, { PixelFormat::RGBA_16F });
		lastFrameSize = s;
		Log("Framebuffers updated.");
	}
	

	Ref<Gl3dFrameBuffer> realAlbedo;
	if (msaaEnabled)
		realAlbedo = albedoPasMSAA;
	else
		realAlbedo = albedoPas;
	
	Gl3dDevice::Viewport(SHADOW_SIZE_W, SHADOW_SIZE_H);

	const Vector3 mainLightDir = Vector3(1.0, -1.0, 1.0).Normalize();

	float HalfSize = 20.0;

	const Vector3 LightDir = Vector3(1, -1, 1).Normalize();


	/*Vector3 rezCenter = CameraObject->GetPosition() + CameraObject->GetForward() * HalfSize;
	Vector3 rezEye = rezCenter + LightDir * -HalfSize;
	Vector3 rezUp = Vector3(0, 1, 0);*/



	Vector3 rezCenter = Vector3::Zero();
	Vector3 rezEye = Vector3::Zero();
	Vector3 rezUp = Vector3::Zero();
	//rezUp = axisof((rezCenter - rezEye).Normalize(), Clocker::GetTotalTime() * Mathf::Pi() / 4.0f) * rezUp;

	Gl3dDevice::DepthTest(true);

	BBox3 fb = CalculateBBoxOfCamera(CameraObject, aspectRatio, 0.1, 20);
	Matrix4 projToView = CalaculateProjection(CameraObject, LightDir, fb, rezEye, rezCenter, rezUp, HalfSize);
	{
		Stopwacth __("Shadow pass");
		Gl3dRenderPas current(shadowGeometryInstanced.GetPtr(), shadowPas.GetPtr());
	
		Gl3dDevice::Clear();
		Gl3dDevice::CullFront();

		current.FastUniform("depthVP", projToView);
		DynamicArray<Material*>& mats = GameInstance::GetCurrent()->materials;
		for (Material* n_current_material : mats)
		{
			ObjectMaterial* current_material = dynamic_cast<ObjectMaterial*>(n_current_material);
			if (!current_material)
				continue;
			d_material_updated++;
			for (const Ref<MeshVariant>& mesh_variant : current_material->render_variant)
			{				
#ifndef USE_INSTANCING
				int indeces_count = mesh_variant->m_mesh->indeces.Length();
				Gl3dVertexArrayBase* vertex_array = mesh_variant->m_vertex_array;
				DynamicArray<Matrix4>* transforms = mesh_variant->m_transofrms_nva;
				for (int i = 0; i < mesh_variant->m_transofrms_nva->Length(); i++)
				{
					const Matrix4& c = transforms->Get(i);
					current.FastUniform("ObjectTransform", c);
					d_total += 1;
					vertex_array->DrawIndexed(Gl3dDrawPrimitive::Triangles, indeces_count);
					d_draw_calls++;
				}
#else
				d_total += mesh_variant->m_transofrms_nva->Length();
				mesh_variant->m_vertex_array->DrawInstnaced(Gl3dDrawPrimitive::Triangles, mesh_variant->m_mesh->indeces.Length(), mesh_variant->m_transofrms_nva->Length());
				d_draw_calls++;
#endif

			}
		}

		TRY_EXEC;
	}

	d_total = 0;
	d_draw_calls = 0;
	d_material_updated = 0;
	Gl3dDevice::Viewport(s.width, s.height);
	{
		Stopwacth __("Geometry albedo-mrao pass");
		Gl3dRenderPas rezultPass(albedoInstanced.GetPtr(), realAlbedo.GetPtr());
		
		Gl3dDevice::Clear();
		Gl3dDevice::CullBack();
		

		if (msaaEnabled)
			Gl3dDevice::MultiSampleTest(true);
		rezultPass.FastUniform("vpShadow", projToView);
		rezultPass.FastUniform("projection", perspective);
		rezultPass.FastUniform("view", lookat);

		DynamicArray<Material*>& mats = GameInstance::GetCurrent()->materials;
		for (Material* n_current_material : mats)
		{
			ObjectMaterial* current_material = /*dynamic_cast*/dynamic_cast<ObjectMaterial*>(n_current_material);
			if (!current_material)
				continue;

			rezultPass.FastUniform("albedoStatic", current_material->albedo_static);
			rezultPass.FastUniform("mraoStatic", current_material->metalic_roughness_ao_static);
			if (current_material->normals)
			{
				rezultPass.FastUniform("normalMapEnable", true);
				rezultPass.FastUniform("normalMap", reinterpret_cast<Gl3dTexture*> (current_material->normals->GetTextureObject()));
			}
			else
			{
				rezultPass.FastUniform("normalMapEnable", false);
			}

			if (current_material->albedo)
			{
				rezultPass.FastUniform("albedoEnable", true);
				rezultPass.FastUniform("albedo", reinterpret_cast<Gl3dTexture*> (current_material->albedo->GetTextureObject()));
			}
			else
			{
				rezultPass.FastUniform("albedoEnable", false);
			}


			if (current_material->metalic)
			{
				rezultPass.FastUniform("_mEnable", true);
				rezultPass.FastUniform("_m", reinterpret_cast<Gl3dTexture*> (current_material->metalic->GetTextureObject()));
			}
			else
				rezultPass.FastUniform("_mEnable", false);

			if (current_material->ao)
			{
				rezultPass.FastUniform("_aoEnable", true);
				rezultPass.FastUniform("_ao", reinterpret_cast<Gl3dTexture*> (current_material->ao->GetTextureObject()));
			}
			else
				rezultPass.FastUniform("_aoEnable", false);

			if (current_material->roughness)
			{
				rezultPass.FastUniform("_rEnable", true);
				rezultPass.FastUniform("_r", reinterpret_cast<Gl3dTexture*> (current_material->roughness->GetTextureObject()));
			}
			else
				rezultPass.FastUniform("_rEnable", false);
			d_material_updated++;

			for (const Ref<MeshVariant>& mesh_variant : current_material->render_variant)
			{
#ifndef USE_INSTANCING
				int indeces_count = mesh_variant->m_mesh->indeces.Length();
				Gl3dVertexArrayBase* vertex_array = mesh_variant->m_vertex_array;
				DynamicArray<Matrix4>* transforms = mesh_variant->m_transofrms_nva;
				for (int i = 0; i < mesh_variant->m_transofrms_nva->Length(); i++)
				{
					const Matrix4& c = transforms->Get(i);
					rezultPass.Uniform("ObjectTransform", c);
					d_total += 1;
					vertex_array->DrawIndexed(Gl3dDrawPrimitive::Triangles, indeces_count);
					d_draw_calls++;
				}
#else
				d_total += mesh_variant->m_transofrms_nva->Length();
				mesh_variant->m_vertex_array->DrawInstnaced(Gl3dDrawPrimitive::Triangles, mesh_variant->m_mesh->indeces.Length(), mesh_variant->m_transofrms_nva->Length());
				d_draw_calls++;
#endif
				
			}
		}
		if (msaaEnabled)	
			Gl3dDevice::MultiSampleTest(false);
	
		TRY_EXEC;
	}
	//LogRewrite("draw state: draw calls: %d, total instances: %d, updates materials: %d", d_draw_calls, d_total, d_material_updated);
	

	DebugDrawShadowOrhoProjection = true;
	Gl3dDevice::Viewport(s.width, s.height);
	{
		Stopwacth __("Debug pass");
		Gl3dRenderPas rezultPass(debug.GetPtr(), realAlbedo.GetPtr());
		
		Gl3dDevice::CullBack();
		rezultPass.FastUniform("mvp", perspective * lookat);
		auto x = [&](const Vector3& p, const Vector3& p1, const Vector3& c, const Vector3& c1)
		{

			Vector3 v[2];
			v[0] = p;
			v[1] = p1;
			Vector3 cv[2];
			cv[0] = c;
			cv[1] = c1;


			debugVertexArray->SetVertexes(v, 2);
			debugVertexArray->SetNormals(cv, 2);
			debugVertexArray->DrawLines();
		};
		Physics::SetDebugDrawCallback(x);
		//shadowHalfExtent

		/*if (DebugDrawShadowOrhoProjection)
		{
			Vector3 Forward = (rezCenter - rezEye).Normalize();
			Vector3 Right = (rezUp.Cross(Forward)).Normalize();
			Vector3 Top = (Right.Cross(Forward)).Normalize();
			Vector3 centerNear = rezEye + (Forward * nearPlane);
			Vector3 centerFar = rezEye + (Forward * farPlane);
			Top *= HalfSize;
			Right *= HalfSize;
			Vector3 nearRU = centerNear + Top + Right;
			Vector3 nearRB = centerNear - Top + Right;
			Vector3 nearLU = centerNear + Top - Right;
			Vector3 nearLB = centerNear - Top - Right;

			Vector3 farRU = centerFar + Top + Right;
			Vector3 farRB = centerFar - Top + Right;
			Vector3 farLU = centerFar + Top - Right;
			Vector3 farLB = centerFar - Top - Right;

			Vector3 color = Vector3(0, 0, 0);

			x(nearRU, nearRB, color, color);
			x(nearRB, nearLB, color, color);
			x(nearLB, nearLU, color, color);
			x(nearLU, nearRU, color, color);

			x(farRU, farRB, color, color);
			x(farRB, farLB, color, color);
			x(farLB, farLU, color, color);
			x(farLU, farRU, color, color);

			x(farRU, nearRU, color, color);
			x(farRB, nearRB, color, color);
			x(farLB, nearLB, color, color);
			x(farLU, nearLU, color, color);

		}*/

		if (DebugDrawPhysicsLens)
			Physics::DrawDebugPhysicsWorld();
		TRY_EXEC;
	}

	Gl3dDevice::Viewport(s.width, s.height);
	{
		Stopwacth __("Skybox pass");
		Gl3dRenderPas skyboxPass(skybox.GetPtr(), realAlbedo.GetPtr());		
		skyboxPass.FastUniform("projection", perspective);
		skyboxPass.FastUniform("view", lookat);
		//skyboxPass.Uniform("skybox", cubeMap.GetPtr());
		skyboxPass.FastUniform("skybox", pbrComuter->EnvaromentSkybox);
		skyboxVertexArray->DrawAllTriangles(36);
		TRY_EXEC;
	}


	if (msaaEnabled)
	{
		Stopwacth __("MSAA copy pass");
		albedoPasMSAA->Blit(albedoPas.GetPtr(), 0, 0, s.width, s.height, { 6, 1 });
	}
	/*
	FragSettings caps

	----float_x-------------float_y-------------float_z-------------float_w---------
	|specular cooficent||	reflection  	||	skybox proxy	||	  padding      |
	--------------------------------------------------------------------------------

	*/

	Gl3dDevice::DepthTest(false);
	ssaoEnabled = true;
	if (ssaoEnabled)
	{
		Gl3dDevice::Viewport(s.width / 2, s.height / 2);
		{
			Stopwacth __("SSAO pass");
			Gl3dRenderPas pass(ssao.GetPtr(), ssaoPas.GetPtr());
			
			Gl3dDevice::Clear();
			for (int i = 0; i < SSAO::SSAOKernel().Length(); i++)
			{
				pass.FastUniform(*String::Format("samples[%d]", i), SSAO::SSAOKernel()[i]);
			}
			pass.FastUniform("displaySize", Vector3(s.width, s.height, 0));
			pass.FastUniform("projection", perspective);
			pass.FastUniform("projectionView", lookat);
			pass.FastUniform("gPosition", albedoPas->GetColorTexture(0));
			pass.FastUniform("gPositionDepth", albedoPas->GetDepthTexture(0));
			pass.FastUniform("gNormal", albedoPas->GetColorTexture(1));
			pass.FastUniform("texNoise", noiseTexture.GetPtr());
			quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
			TRY_EXEC;
		}

		Gl3dDevice::Viewport(s.width / 2, s.height / 2);
		{
			Stopwacth __("SSAO blur pass");
			Gl3dRenderPas pass(ssaoBlur.GetPtr(), ssaoBlurPas.GetPtr());
			
			Gl3dDevice::Clear();
			pass.FastUniform("ssaoInput", ssaoPas->GetColorTexture(0));
			quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
			TRY_EXEC;
		}
	}
	

	


	Gl3dDevice::Viewport(s.width, s.height);
	{
		Stopwacth __("Global illumination pass");
		Gl3dRenderPas rezultPass(light.GetPtr(), renderPas.GetPtr());
		

		rezultPass.FastUniform("lightsCount", (int)ligths.Length());
		for (int i = 0; i < ligths.Length(); i++) 
		{
			const Ref<Ligth>& l = ligths[i];
			static char strbuffer[1024];
			snprintf(strbuffer, 1024, "lights[%d].position", i);
			rezultPass.FastUniform(strbuffer, l->position);
			snprintf(strbuffer, 1024, "lights[%d].color", i);
			rezultPass.FastUniform(strbuffer, l->powerdColor);
			snprintf(strbuffer, 1024, "lights[%d].inFragSpace", i);
			rezultPass.FastUniform(strbuffer, l->inFragSpace);

		}

		rezultPass.FastUniform("lookPosition", CameraObject->GetPosition());
		rezultPass.FastUniform("FragPosition", albedoPas->GetColorTexture(0));
		rezultPass.FastUniform("FragNormal", albedoPas->GetColorTexture(1));
		rezultPass.FastUniform("FragColor", albedoPas->GetColorTexture(2));
		rezultPass.FastUniform("FragSettings", albedoPas->GetColorTexture(3));
		rezultPass.FastUniform("FragPBR", albedoPas->GetColorTexture(5));
		rezultPass.FastUniform("FragDepth", albedoPas->GetColorTexture(4));
		rezultPass.FastUniform("shadowMap", shadowPas->GetDepthTexture(0));


		rezultPass.FastUniform("irradianceMap", pbrComuter->IrradianceMap);
		rezultPass.FastUniform("prefilterMap", pbrComuter->PrefilterMap);
		rezultPass.FastUniform("brdfLUT", pbrComuter->BrdfLut);

		rezultPass.FastUniform("ssaoEnabled", ssaoEnabled);

		rezultPass.FastUniform("debugDrawShadowProjection", DebugDrawShadowOrhoProjection);

		rezultPass.FastUniform("FragSSAO", ssaoBlurPas->GetColorTexture(0));
		quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
		TRY_EXEC;
	}


	/*Gl3dDevice::Viewport(s.width, s.height);
	bool horizontal = true;
	bool first_iteration = true;
	int amount = 30;
	{
		Gl3dRenderPas pingpongpass(godRays.GetPtr(), getHVpingbong(!horizontal).GetPtr());
		pingpongpass.Uniform("UserMapSampler", renderPas->GetColorTexture(1));
		pingpongpass.Uniform2("lightPositionOnScreen", Vector2(0.5, 0.5f));
		quad->DrawTriangles();
	}

	for (int i = 0; i < amount; i++)
	{
		Gl3dRenderPas pingpongpass(blur.GetPtr(), getHVpingbong(horizontal).GetPtr());


		pingpongpass.Uniform("horizontal", horizontal);
		pingpongpass.Uniform("image", getHVpingbong(!horizontal)->GetColorTexture(0));

		quad->DrawTriangles();

		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}*/


	Gl3dDevice::Viewport(s.width / 5, s.height / 5);
	{
		Stopwacth __("Glow-S pass");
		Gl3dRenderPas pass(glow.GetPtr(), glowPas.GetPtr());
		Gl3dDevice::Clear();
	
		pass.Uniform("input", renderPas->GetColorTexture(1));
		quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
	}


	bool horizontal = true;
	bool first_iteration = true;
	int amount = 10;

	Gl3dDevice::Viewport(s.width / 5, s.height / 5);
	{	
		Stopwacth __("Glow blum pass");
		for (int i = 0; i < amount; i++)
		{
			Gl3dRenderPas pingpongpass(blur.GetPtr(), getHVpingbong(horizontal).GetPtr());


			pingpongpass.FastUniform("horizontal", horizontal);
			pingpongpass.FastUniform("image", first_iteration ? glowPas->GetColorTexture(0) : getHVpingbong(!horizontal)->GetColorTexture(0));

			quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);

			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
		TRY_EXEC;
	}

	
	Gl3dDevice::Viewport(s.width, s.height);
	{
		Stopwacth __("GodRays pass");
		Gl3dRenderPas pass(godRays.GetPtr(), godRaysPas.GetPtr());	
		Gl3dDevice::Clear();
		pass.FastUniform("projection", perspective);
		pass.FastUniform("view", lookat);
		pass.FastUniform("UserMapSampler", renderPas->GetColorTexture(2));
		quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
		TRY_EXEC;
	}
	

	Gl3dDevice::Viewport(s.width, s.height);
	{
		Stopwacth __("FXAA pass");
		Gl3dRenderPas pass(fxaa.GetPtr(), compositPas[1].GetPtr());	
		pass.FastUniform("u_colorTexture", renderPas->GetColorTexture(0));
		pass.FastUniform("u_texelStep", Vector2(1.0 / s.width, 1.0 / s.height));
		quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
		SwapComposit();
		TRY_EXEC;
	}
	
	Gl3dDevice::Viewport(s.width, s.height);
	{
		Stopwacth __("Chromatic aberration pass");
		Gl3dRenderPas pass(chromaticAberration.GetPtr(), compositPas[1].GetPtr());	
		//pass.Uniform("tInput", renderPas->GetColorTexture(0));
		pass.FastUniform("tInput", compositPas[0]->GetColorTexture(0));
		pass.FastUniform("resolution", Vector2(s.width, s.height));
		quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
		SwapComposit();
		TRY_EXEC;
	}

	Gl3dDevice::Viewport(outS.width, outS.height);

	switch (outType)
	{
	case RenderBeast::DisplayOut::SSAO:
	{
		Gl3dRenderPas rezultPass(flat.GetPtr(), nullptr);

		Gl3dDevice::Clear();
		rezultPass.FastUniform("image", ssaoBlurPas->GetColorTexture(0));
		quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
	}		
		break;
	case RenderBeast::DisplayOut::COMPOSIT:
	{
		Stopwacth __("Final pass");
		Gl3dRenderPas rezultPass(postProcess.GetPtr(), nullptr);	
		Gl3dDevice::Clear();

		rezultPass.FastUniform("projection", perspective);
		rezultPass.FastUniform("view", lookat);
		rezultPass.FastUniform("god_map", godRaysPas->GetColorTexture(0));
		//rezultPass.Uniform("rez_map", renderPas->GetColorTexture(0));
		rezultPass.FastUniform("rez_map", compositPas[0]->GetColorTexture(0));
		rezultPass.FastUniform("blur_map", getHVpingbong(!horizontal)->GetColorTexture(0));

		quad2->Draw(Gl3dDrawPrimitive::Triangles, quad2VertexesCount);
		

	}	
		break;
	default:
		break;
	}
	
	stopwacthPairs.Sort();
	profileString.Clear();
	int i = 0;
	for (auto& x : stopwacthPairs) 
	{
		i++;
		profileString += String::Format("[PASS] [%d] %s: %f\n", i, x.name, x.timing);
	}
	profileString += String::Format("[PASS] %s: %f\n", "ALL PASS", (Time::GetTotalTime() - frameTime));
	profileString += String::Format("[ACTIVE CALLS] %d\n", GL3D_COUNTER[GL3D_PROFILE_ACTIVETE_CALLS]);
	profileString += String::Format("[UNIFORM CALLS] %d\n", GL3D_COUNTER[GL3D_PROFILE_ACTIVETE_CALLS]);

	StopwacthSwap();


	Gl3dDevice::ResetProfiler();
}

