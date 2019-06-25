#include "pch.h"
/*
схема многопоточного пуска:
после окончания обновления движка отправляем рендеру сигнал о пуске отрисовки нового
кадра. при этом пока сигнала о отрисовки не приходит выполняем переброску операций в графичесий поток
 чтобы физический поток мог создавать в GPU меши и текстуры.
*/



/*
GPU->main thread
Phys->child thread
*/

#include <vector>
#include <mutex>
#include "MeshImporter.h"

//
//struct MeshSmooth
//{
//	std::vector<Vector3> v;
//	std::vector<Vector2> t;
//	std::vector<Vector3> n;
//	std::vector<int>	 tc;
//	std::vector<int>	 nc;
//
//	std::vector<int>  s;
//
//	int add_vertex(
//		const Vector3& va,
//		const Vector2& ta,
//		const Vector3& na)
//	{
//		int foundIndex = -1;
//		for (int i = 0; i < v.size(); i++)
//			if (v[i] == va)
//			{
//				foundIndex = i;
//				break;
//			}
//		if (foundIndex != -1)
//		{
//			t[foundIndex] += ta;
//			n[foundIndex] += na;
//			tc[foundIndex] += 1;
//			nc[foundIndex] += 1;
//			return;
//		}
//
//		int i = v.size();
//		v.push_back(va);
//		t.push_back(ta);
//		n.push_back(na);
//		tc.push_back(1);
//		nc.push_back(1);
//		return i;
//	}
//
//	void post_process() 
//	{
//		for (int i = 0; i < v.size(); i++) 
//		{
//			t[i] /= tc[i];
//			n[i] /= nc[i];
//		}
//	}
//
//	void add_triangle(
//		const Vector3& va, const Vector3& vb, const Vector3& vc,
//		const Vector2& ta, const Vector2& tb, const Vector2& tc,
//		const Vector3& na, const Vector3& nb, const Vector3& nc,
//		bool useOrderFixAlgo = false
//	)
//	{		
//		s.push_back(add_vertex(va, ta, na));
//		s.push_back(add_vertex(vb, tb, nb));
//		s.push_back(add_vertex(vc, tc, nc));		
//	}
//};
//

#include <Time.hpp>
#include <GameObject.h>
#include <PhysicsComponents/BoxCollider.h>
#include <PhysicsComponents/TrianglesCollider.h>
#include <PhysicsComponents/CarCollider.h>
#include <Display.h>
#include <PhysicsComponents/RigidBody.h>
#include <Math/BBox3.h>
#undef GetObject
inline Texture*  LoadTextureFileE(const String& path)
{
	Ref<Bitmap> bit = new Bitmap(path);
	Texture* r = new Texture();
	r->SetFromBitmap(bit);
	return r;
};

inline Texture* LoadTextureE(const String& path)
{
	try
	{
		Texture* x = (new Texture());
		x->SetFromBitmap(new Bitmap(path));
		return x;
	}
	catch (std::exception& ex)
	{
		return Resource::Find(path)->GetObject<Ref<Texture>>().GetPtr();
	}
};

inline void LoadModelsE(const String& name, const Vector3& pos)
{
	GameObject* ogne = MeshImporter::Import("/assets/" + name + "/Sphere.FBX")->Construct();

	ogne->SetPosition(pos);
	ogne->AddComponent<FixedRotation>();


	Tools::SetMaps(ogne,
		LoadTextureE("/assets/" + name + "/albedo.png"),
		LoadTextureE("/assets/" + name + "/normal.png"),
		LoadTextureE("/assets/" + name + "/metallic.png"),
		LoadTextureE("/assets/" + name + "/roughness.png"),
		LoadTextureE("/assets/" + name + "/ao.png"));
}

void LoadModelsE(const String& name, const String& prefix, const Vector3& pos)
{
	GameObject* ogne = MeshImporter::Import("/assets/" + name + "/Barril.FBX")->Construct();

	ogne->SetPosition(pos);

	Tools::ScaleAll(ogne, 0.1);

	Tools::SetMaps(ogne,
		LoadTextureE("/assets/" + name + "/" + prefix + "_base_color.png"),
		LoadTextureE("/assets/" + name + "/" + prefix + "_normal.png"),
		LoadTextureE("/assets/" + name + "/" + prefix + "_metallic.png"),
		LoadTextureE("/assets/" + name + "/" + prefix + "_roughness.png"),
		LoadTextureE("/assets/" + name + "/" + prefix + "_ambient.png"));
}


void LoadDefaultPrefabs()
{
	GameObject::AddPrefab("/DebugBox.prefab", []()
	{
		GameObject*  obj = MeshImporter::Import("/assets/GrassBlock/GrassBlock.fbx")->Construct();
		obj->AddComponent<RigidBody>()->SetMass(1.0);
		obj->AddComponent<BoxCollider>()->SetSize(Vector3(1.0, 1.0, 1.0));
		return obj;
	});

	GameObject::AddPrefab("/DebugCube.prefab", []()
	{
		GameObject*  obj = MeshImporter::Import("/assets/DebugCube/DebugCube.fbx")->Construct();
		Tools::SetMaps(obj,
			LoadTextureE("/assets/DebugCube/Debug.png"),
			nullptr,
			nullptr,
			nullptr,
			nullptr);
		return obj;
	});

	GameObject::AddPrefab("/Camera.prefab", []()
	{
		GameObject* oc = new GameObject();
		GameObject* obj = MeshImporter::Import("/assets/matbol/export3dcoat.obj")->Construct();
		obj->SetName("MatBoll");

		Tools::ScaleAll(obj, 0.1);
		Tools::SetMaps(obj,
			LoadTextureE("/assets/Matbol/A.tga"),
			LoadTextureE("/assets/Matbol/N.tga"),
			LoadTextureE("/assets/Matbol/M.tga"),
			LoadTextureE("/assets/Matbol/R.tga"),
			/*LoadTexture("./Data/Cerberus/AO.jpg")*/nullptr);
		obj->SetParent(oc);
		return oc;
	});


	GameObject::AddPrefab("/Cars/PickUp.prefab", []()
	{
		GameObject* pixkup = MeshImporter::Import("/assets/PickUp/PickUpBody.fbx")->Construct();

		pixkup->SetLocalPosition(Vector3(0.0, -0.991, 0.0));

		Tools::ScaleAll(pixkup, 0.44);
		GameObject* car = new GameObject();
		pixkup->SetParent(car);
		//car->AddComponent<ControllDelete>();
		car->AddComponent<RigidBody>()->SetMass(888);
		BoxCollider*  box = car->AddComponent<BoxCollider>();
		box->SetCenter(Vector3(0.01357228, 0.06907833, -0.03313088));
		box->SetSize(Vector3(1.821319, 1.138157, 4.587969));
		CarCollider* wc = car->AddComponent<CarCollider>();

		GameObject* wheelExample1 = MeshImporter::Import("/assets/PickUp/PickUpWheel.fbx")->Construct();
		Tools::ScaleAll(wheelExample1, 0.44f);
		//GameObject* wheelExample1 = new GameObject();
		float upv = 0.64;
		wc->AddWheel(wheelExample1, Vector3(0.829, -0.74 + upv, 1.525), true);
		wc->AddWheel(GameObject::Clone(wheelExample1), Vector3(-0.829, -0.74 + upv, 1.525), true);
		wc->AddWheel(GameObject::Clone(wheelExample1), Vector3(0.829, -0.74 + upv, -1.134), false);
		wc->AddWheel(GameObject::Clone(wheelExample1), Vector3(-0.829, -0.74 + upv, -1.134), false);


		return car;
	});

	GameObject::AddPrefab("/Terrain.prefab", []()
	{
		GameObject* ct = MeshImporter::Import("/assets/Terrain/Terrain.fbx")->Construct();
		ct->AddComponent<RigidBody>()->SetMass(0.0);
		ct->AddComponent<TrianglesCollider>()->CollectMeshesFromRenderer();

		//Tools::SetMaps(ct,
		//	LoadTextureE("/assets/Custom/leaf-fall1-albedo.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall3-normal-ue.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall1-metalness.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall1-roughness.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall1-ao.png"));

		/*Tools::SetMaps(ct,
			LoadTextureE("/assets/Sand/sandyground1_Base_Color.png"),
			LoadTextureE("/assets/Sand/sandyground1_Normal.png"),
			LoadTextureE("/assets/Sand/sandyground1_Metallic.png"),
			LoadTextureE("/assets/Sand/sandyground1_Roughness.png"),
			LoadTextureE("/assets/Sand/sandyground1_Ambient_Occlusion.png"));*/

		Tools::SetMaps(ct,
			LoadTextureE("/assets/RedSand/sand1-albedo.png"),
			LoadTextureE("/assets/RedSand/sand1-normal-ue.png"),
			LoadTextureE("/assets/RedSand/sand1-metalness.png"),
			LoadTextureE("/assets/RedSand/sand1-roughness.png"),
			nullptr);

		return ct;
	});

	GameObject::AddPrefab("/Doom.prefab", []()
	{
		GameObject* d2 = MeshImporter::Import("/assets/Doom/Doom.FBX")->Construct();
		d2->AddComponent<RigidBody>()->SetMass(0.0);
		d2->AddComponent<TrianglesCollider>()->CollectMeshesFromRenderer(Vector3(4.0f, 4.0f, 4.0f));
		Tools::ScaleAll(d2, 4.0f);
		return d2;
	});


	GameObject::AddPrefab("/SandHome.prefab", []()
	{
		GameObject* home = MeshImporter::Import("/assets/SandHome/SandHome.FBX")->Construct();
		home->AddComponent<RigidBody>()->SetMass(0.0);
		home->AddComponent<TrianglesCollider>()->CollectMeshesFromRenderer();
		return home;
	});

	GameObject::AddPrefab("/Ak12.prefab", []()
	{
		GameObject* home = MeshImporter::Import("/assets/ak12/ak12.fbx")->Construct();
		Tools::SetMRAO(home, 0.5, 0.5, 1.0);
		return home;
	});

	GameObject::AddPrefab("/Ogne.prefab", []()
	{
		GameObject* ogne = MeshImporter::Import("/assets/Ogne/Ogne.FBX")->Construct();
		ogne->AddComponent<FixedRotation>();
		Tools::ScaleAll(ogne, 0.05);

		Tools::SetMaps(ogne,
			LoadTextureE("/assets/Ogne/1_BC.png"),
			LoadTextureE("/assets/Ogne/1_N.jpg"),
			LoadTextureE("/assets/Ogne/1_M.jpg"),
			LoadTextureE("/assets/Ogne/1_R.jpg"),
			LoadTextureE("/assets/Ogne/1_AO.jpg"));
		return ogne;
	});


	GameObject::AddPrefab("/Palms.prefab", []()
	{
		GameObject* d2 = MeshImporter::Import("/assets/Palms/Palms.FBX")->Construct();
		Tools::ScaleAll(d2, 0.3);
		return d2;
	});

	GameObject::AddPrefab("/Matbol.prefab", []()
	{
		GameObject* obj = MeshImporter::Import("/assets/matbol/export3dcoat.obj")->Construct();
		obj->AddComponent<FixedRotation>();
		Tools::ScaleAll(obj, 0.1);
		Tools::SetMaps(obj,
			LoadTextureE("/assets/Matbol/A.tga"),
			LoadTextureE("/assets/Matbol/N.tga"),
			LoadTextureE("/assets/Matbol/M.tga"),
			LoadTextureE("/assets/Matbol/R.tga"),
			/*LoadTexture("./Data/Cerberus/AO.jpg")*/nullptr);
		return obj;
	});

	GameObject::AddPrefab("/NT_Barrils.prefab", []()
	{
		GameObject* obj = new GameObject();
		LoadModelsE("BarrilA", "drum1", Vector3(17, 40, 0));
		LoadModelsE("BarrilB", "drum2", Vector3(19, 40, 0));
		LoadModelsE("BarrilC", "drum3", Vector3(21, 40, 0));
		return obj;
	});



	GameObject::AddPrefab("/Sphere.prefab", []()
	{
		return MeshImporter::Import("/assets/Gold/Sphere.FBX")->Construct();;
	});


	GameObject::AddPrefab("/NT_Mattests.prefab", []()
	{
		GameObject* obj = new GameObject();
		LoadModelsE("Gold", Vector3(5, 40, 0));
		LoadModelsE("Grass", Vector3(7, 40, 0));
		LoadModelsE("Plasic", Vector3(9, 40, 0));
		LoadModelsE("RustedIron", Vector3(11, 40, 0));
		LoadModelsE("Wall", Vector3(13, 40, 0));
		return obj;
	});

	GameObject::AddPrefab("/NT_PbrSpheres.prefab", []()
	{
		GameObject* obj = new GameObject();
		float m, r, ao = 1.0;
		int nrRows = 14;
		int nrColumns = 14;
		float spacing = 1.2;
		for (int row = 0; row < nrRows; ++row)
		{
			m = (float)row / (float)nrRows;
			for (int col = 0; col < nrColumns; ++col)
			{
				GameObject* obj = MeshImporter::Import("/assets/Grass/Sphere.FBX")->Construct();
				obj->SetPosition(Vector3(
					(col - (nrColumns / 2)) * spacing,
					30.0f + (row - (nrRows / 2)) * spacing,
					0.0f
				));

				r = Mathf::Clamp((float)col / (float)nrColumns, 0.05f, 1.0f);
				ObjectMaterial* cx = new ObjectMaterial();

				cx->albedo = nullptr;
				cx->metalic = nullptr;
				cx->roughness = nullptr;
				cx->ao = nullptr;
				cx->metalic_roughness_ao_static = Vector3(m, r, ao);
				//cx->metalic_roughness_ao_static = Vector3(0.0, (1.0 - 0.809) * (1.0 - 0.809), 1.0);
				cx->albedo_static = Vector4(0.403, 0.403, 0.403, 1.0);
				Tools::SetMaterial(obj, cx);
			}
		}

		return obj;
	});

}

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/constants.hpp> // glm::pi

DynamicArray<class SampleSyzaRender*> m_renders;

class SampleSyzaRender : public Component
{
public:
	Matrix4			m_transform;
	Matrix4			m_scale;
	bool			m_isCube = false;
	void Destroy() override
	{
		renderThreadQueue.QueueFunctionWait([=]()
		{
			m_renders.Remove(this);
		});
	}

	void Awake() override
	{
		m_scale = Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f));
		SetEnabled(true);

		renderThreadQueue.QueueFunction([=]()
		{
			m_renders.Add(this);
		});

	}

	void FrameUpdate() override
	{
		GetGameObject()->FillMatrix4x4((float*)&m_transform);
		m_transform = m_transform * m_scale;
	}
};

const float quadVertexes[] =
{
	-1.0f, 1.0f,
	1.0f, 1.0f, 
	1.0f, -1.0f, 
	-1.0f, 1.0f,
	1.0f, -1.0f,
	-1.0f, -1.0f
};
template<typename T, typename X>
const T& PSclae(const X* ptr, size_t index)
{
	return reinterpret_cast<const T*>(ptr)[index];
}

template<typename T, typename X>
constexpr T v3t(const X& v) 
{
	return T(v.x, v.y, v.z);
}

BBox3 CalculateBBoxOfCamera(GameObject* camera, float aspect, float nearPlane, float farPlane)
{
	Vector3 eye = camera->GetPosition();
	Vector3 vZ = camera->GetForward();
	Vector3 vX = camera->GetRight();
	Vector3 vY = camera->GetUp();

	float nearPlaneHeight = tanf(3.14f / 2.0f * 0.5f) * nearPlane;
	float nearPlaneWidth = nearPlaneHeight * aspect;
	float farPlaneHeight = tanf(3.14f / 2.0f * 0.5f) * farPlane;
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

struct _CmCombine 
{
	Matrix4 proj;
	Matrix4 look;
};

_CmCombine CalaculateProjection(GameObject* camera, const Vector3& lightDir, const BBox3& frustumBox, Vector3& dEye, Vector3& dCenter, Vector3& dUp, float& half)
{
	_CmCombine r;

	float halfSize = frustumBox.GetSize().Length() * 0.5;
	Vector3 lookAmplyFire = -lightDir;
	Vector3 lookCenter = frustumBox.GetCenter();
	Vector3 eyePosint = lookCenter + lookAmplyFire * halfSize;

	dEye = eyePosint;
	dCenter = lookCenter;
	dUp = (lookCenter - eyePosint).Normalize().Cross(Vector3(0, 1, 0)).Normalized().Cross((lookCenter - eyePosint).Normalize());
	r.look = Matrix4::LookAt(eyePosint, lookCenter, Vector3(0, 1, 0));

	half = halfSize;
	r.proj = Matrix4::Ortho(-halfSize, halfSize, -halfSize, halfSize, 0.1, halfSize * 2 + 0.1);
	return r;
}


class Renderer
{
public:
	Ref<Gl3dShader>             _blit;
	Ref<Gl3dShader>				_geometry;
	Ref<Gl3dShader>				_geometrySkybox;
	Ref<Gl3dShader>				_geometryTanTex;
	Ref<Gl3dShader>				_geometryTanTexInstancing;
	Ref<Gl3dShader>				_shadowGeometryInstancing;
	Ref<Gl3dShader>				_shadowGeometry;


	Ref<Gl3dShader>				_ssao;
	Ref<Gl3dShader>				_ssaoBlur;

	Gl3dFrameBufferInstance     _shadowGeometryFrame;
	Gl3dFrameBufferInstance     _geometryFrame;

	Gl3dTexture					_shadowDepth;
	Gl3dTexture					_depth;
	Gl3dTexture					_color;
	Gl3dTexture					_normal;
	Gl3dTexture					_position;
	Gl3dTexture					_rmo;

	Gl3dTexture					_ssaoNoise;
	int							_ssaoSamplesCount = 64;

	Gl3dTexture					_pipe[2];
	Gl3dFrameBufferInstance     _pipeFrame[2];

	Gl3dLayoutInstance			_sphere;
	Ref<Gl3dArray<Vector3>>		_sphere_vertexes;
	Ref<Gl3dArray<int>>		_sphere_indeces;

	Gl3dLayoutInstance			_cube;
	Ref<Gl3dArray<Vector3>>		_cube_vertexes;
	Ref<Gl3dArray<Vector3>>		_cube_normals;
	Ref<Gl3dArray<int>>			_cube_indeces;

	Ref<Gl3dArray<Vector2>>		_quadVertexes;
	Gl3dLayoutInstance			_quad;

	struct MeshUpdater : public MeshStateUpdater
	{
		int drawCall;
		Gl3dRenderPas* pass;
		virtual bool Update(const Matrix4& transform) override
		{
			drawCall++;
			pass->Uniform("model", transform);
			return true;
		}
	} updater;

	Renderer()
	{

		UpdateShaders();
		UpdateFrameBuffers(Size(1, 1));

		{
					Gl3dFrameBufferDesc buffDesc = {};
					buffDesc.depthAttachment = &_shadowDepth;
					_shadowGeometryFrame.Create(&buffDesc);
		}
		{
			Gl3dFrameBufferDesc buffDesc = {};
			buffDesc.depthAttachment = &_depth;
			buffDesc.colorAttachments[0] = &_color;
			buffDesc.colorAttachments[1] = &_position;
			buffDesc.colorAttachments[2] = &_normal;
			buffDesc.colorAttachments[3] = &_rmo;
			_geometryFrame.Create(&buffDesc);
		}

		{
			Gl3dFrameBufferDesc buffDesc = {};
			buffDesc.colorAttachments[0] = &_pipe[0];
			_pipeFrame[0].Create(&buffDesc);
			buffDesc.colorAttachments[0] = &_pipe[1];
			_pipeFrame[1].Create(&buffDesc);
		}



		_quadVertexes = new Gl3dArray<Vector2>(Gl3dArrayTarget::Array);
		_quadVertexes->Add((Vector2*)quadVertexes, 6);

		_cube_vertexes = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_cube_indeces = new Gl3dArray<int>(Gl3dArrayTarget::Element);
		_cube_normals = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_cube_vertexes->Add((Vector3*)model_cube::vertexes, model_cube::vertexesCount);
		_cube_indeces->Add((int*)model_cube::indeces, model_cube::indecesCount);
		_cube_normals->Add((Vector3*)model_cube::normals, model_cube::vertexesCount);

		_sphere_vertexes = new Gl3dArray<Vector3>(Gl3dArrayTarget::Array);
		_sphere_indeces = new Gl3dArray<int>(Gl3dArrayTarget::Element);
		_sphere_vertexes->Add((Vector3*)model_sphere::vertexes, model_sphere::vertexesCount);
		_sphere_indeces->Add((int*)model_sphere::indeces, model_sphere::indecesCount);

		Gl3dLayoutDesc sphereDesc = {};
		sphereDesc.index = _sphere_indeces.GetPtr();
		sphereDesc.layouts[0] = { _sphere_vertexes.GetPtr(), 3, Gl3dFundamentalType::Float, 3 * sizeof(float), 0, false };
		_sphere.Create(&sphereDesc);

		Gl3dLayoutDesc desc = {};
		desc.index = _cube_indeces.GetPtr();
		desc.layouts[0] = { _cube_vertexes.GetPtr(), 3, Gl3dFundamentalType::Float, 3 * sizeof(float), 0, false };
		desc.layouts[1] = { _cube_normals.GetPtr(), 3, Gl3dFundamentalType::Float, 3 * sizeof(float), 0, false };
		_cube.Create(&desc);
		desc.index = nullptr;
		desc.layouts[0] = { _quadVertexes.GetPtr(), 2, Gl3dFundamentalType::Float, 2 * sizeof(float), 0, false };
		desc.layouts[1] = {};
		_quad.Create(&desc);

	}

	struct CaptureDesc 
	{
		Matrix4 view;
		Matrix4 projection;
		int		w;
		int		h;
		bool	geometry;
		bool	skybox;
	};

	void RenderToCapture(CaptureDesc* capture)
	{
	
	
	
	}

	Matrix4 RenderShadows()
	{
		GameObject* camera = Display::GetCurrent()->GetCamera();
		Size viewSize = Display::GetCurrent()->GetSize();
		const Vector3 mainLightDir = Vector3(1.0, -1.0, 1.0).Normalize();

		float HalfSize = 20.0;

		const Vector3 LightDir = Vector3(1, -1, 1).Normalize();

		Vector3 rezCenter = Vector3::Zero();
		Vector3 rezEye = Vector3::Zero();
		Vector3 rezUp = Vector3::Zero();

		BBox3 fb = CalculateBBoxOfCamera(camera, float(viewSize.width) / float(viewSize.height), 0.1, 20);
		_CmCombine projToView = CalaculateProjection(camera, LightDir, fb, rezEye, rezCenter, rezUp, HalfSize);
		Matrix4 r = projToView.proj * projToView.look;

		Gl3dRenderPassDesc passDesc = {};
		passDesc.cullFace = Gl3dCullFace::Front;
		passDesc.depth = Gl3dDepth::LessEqual;
		passDesc.blending = Gl3dBlending::Disable;
		passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		passDesc.clearDepth = 1.0f;
		passDesc.clear = Gl3dClear::Depth;
		passDesc.viewport = { 0, 0, 4055, 4055 };


		{
			Gl3dRenderPas pass(_shadowGeometry.GetPtr(), &_shadowGeometryFrame, &passDesc);
			updater.pass = &pass;

			pass.Uniform("projection", projToView.proj);
			pass.Uniform("view", projToView.look);

			for (auto& x : renderMaterials)
			{
				if (x->GetRenderSources().Length())
				{
					for (const auto& sour : x->GetRenderSources())
						sour->Draw(&updater, RenderThreadTag());
				}
			}

		}


		Gl3dDevice::Flush();
		{
			Gl3dRenderPas pass(_shadowGeometryInstancing.GetPtr(), &_shadowGeometryFrame, &passDesc);
			pass.Uniform("projection", projToView.proj);
			pass.Uniform("view", projToView.look);


			for (auto& x : renderMaterials)
			{
				if (x->GetRenderSources().Length())
				{
					for (const auto& sour : x->GetRenderSources())
					{
						sour->DrawInstanced(RenderThreadTag());
					}
				}
			}
		}
		
		return r;
	}




	void UpdateShaders() 
	{
		_blit = New<Gl3dShader>();
		_geometry = New<Gl3dShader>();
		_ssao = New<Gl3dShader>();
		_geometrySkybox = New<Gl3dShader>();
		_ssaoBlur = New<Gl3dShader>();
		_geometryTanTex = New<Gl3dShader>();
		_geometryTanTexInstancing = New<Gl3dShader>();
		_shadowGeometry = New<Gl3dShader>();
		_shadowGeometryInstancing = New<Gl3dShader>();
		try 
		{
			_blit->LoadFiles("./blit.vert", "./blit.frag", nullptr, 0);
			_geometry->LoadFiles("./geometry.vert", "./geometry.frag", nullptr, 0);
			_ssao->LoadFiles("./ssao.vert", "./ssao.frag", nullptr, 0);
			_ssaoBlur->LoadFiles("./ssaoBlur.vert", "./ssaoBlur.frag", nullptr, 0);


			_geometrySkybox->LoadFiles("./skybox.vert", "./skybox.frag", nullptr, 0);


			_geometryTanTex->LoadFiles("./geometry.vert", "./geometry.frag", nullptr, 0);
			_shadowGeometry->LoadFiles("./shadowGeometry.vert", "./shadowGeometry.frag", nullptr, 0);
			const char* mcDefines3[] = { "INSTANCING" };
			_geometryTanTexInstancing->LoadFiles("./geometry.vert", "./geometry.frag", mcDefines3, 1);
			_shadowGeometryInstancing->LoadFiles("./shadowGeometry.vert", "./shadowGeometry.frag", mcDefines3, 1);
			{
				Gl3dRenderPas _ssaoUpdate(_ssao.GetPtr(), nullptr, nullptr);

				DynamicArray<Vector3> ssaoNoise;
				for (unsigned int i = 0; i < _ssaoSamplesCount; ++i)
				{
					Vector3 sample(Mathf::Random() * 2.0 - 1.0, Mathf::Random() * 2.0 - 1.0, Mathf::Random());

					sample.Normalize();
					sample *= Mathf::Random();
					float scale = float(i) / _ssaoSamplesCount;

					scale = Mathf::Lerp(0.1f, 1.0f, scale * scale);
					sample *= scale;
					_ssaoUpdate.Uniform(*String::Format("kernel[%d]", i), sample);
				}

				for (unsigned int i = 0; i < 16; i++)
				{
					Vector3 noise(Mathf::Random() * 2.0 - 1.0, Mathf::Random() * 2.0 - 1.0, 0.0f);
					ssaoNoise.Add(noise);
				}
				Gl3dSubImageDesc image;
				image.width = 4;
				image.height = 4;
				image.data = (void*)ssaoNoise.GetData();
				image.type = Gl3dFundamentalType::Float;
				image.changelsCount = 3;

				_ssaoNoise.SetData(Gl3dPixelFormat::RGB_32F, &image);
			}
	
		}
		catch (std::exception& ex)
		{
			printf("error loading shaders: %s\n", ex.what());
		}
	
	}

	//Gl3dTexture* brdfLut;
	//Gl3dTexture* 

	//struct GBufferState 
	//{
	//	Gl3dTexture* gAlbedo = nullptr;
	//	Gl3dTexture* gPosition = nullptr;
	//	Gl3dTexture* gNormal = nullptr;
	//	Gl3dTexture* gMaterial = nullptr;
	//};


	//void RenderLights(Gl3dFrameBufferBase* out, GBufferState* lights)
	//{
	//	for (auto& x : renderLights) 
	//	{
	//		


	//	}
	//}


	void UpdateFrameBuffers(Size viewSize)
	{
		_shadowDepth.AllocateData(4055, 4055, Gl3dPixelFormat::DEPTH_16);
		_color.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGBA_16F);
		_normal.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_32F);
		_position.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_32F);
		_rmo.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_16F);
		_position.SetWrapMode(Gl3dWrapMode::ClampToEdge, Gl3dWrapMode::ClampToEdge, Gl3dWrapMode::ClampToEdge);
		_depth.AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::DEPTH_16);
		_pipe[0].AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_16F);
		_pipe[1].AllocateData(viewSize.width, viewSize.height, Gl3dPixelFormat::RGB_16F);
	}
	Size lastSize = Size(-1, -1);

	void OnDraw()
	{
		updater.drawCall = 0;
		int drawCall = 0;
		int updateCall = 0;
		int matCall = 0;

		if (Input::IsKeyDown(SGE_KEY_3))
		{
			_ssaoSamplesCount = 64;
			UpdateShaders();
		}
		if (Input::IsKeyDown(SGE_KEY_4))
		{
			_ssaoSamplesCount = 32;
			UpdateShaders();
		}
		if (Input::IsKeyDown(SGE_KEY_5))
		{
			_ssaoSamplesCount = 16;
			UpdateShaders();
		}

		//ssaoInput
		Size viewSize = Display::GetCurrent()->GetSize();
		if (viewSize.width < 1 || viewSize.height < 1)
			return;
		if (viewSize != lastSize)
		{
			UpdateFrameBuffers(viewSize);
			lastSize = viewSize;		
		}


		
		
		GameObject* camera = Display::GetCurrent()->GetCamera();
		Vector3 lookPosition = camera->GetPosition();
		auto view =  Matrix4::LookAt(v3t<Vector3>(lookPosition), v3t<Vector3>(lookPosition + camera->GetForward()), v3t<Vector3>(camera->GetUp()));
		auto projection = Matrix4::Perspective(3.14f / 2.0f, float(viewSize.width) / float(viewSize.height), 0.1f, 1400.0f);
		
	/*	glm::cross(glm::vec3(1.0f), glm::vec4(1.0f));*/

		Gl3dRenderPassDesc postDesc = {};
		postDesc.cullFace = Gl3dCullFace::Disable;
		postDesc.depth = Gl3dDepth::Disable;
		postDesc.blending = Gl3dBlending::Disable;
		postDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		postDesc.clearDepth = 1.0f;
		postDesc.clear = Gl3dClear::None;
		postDesc.viewport = { 0, 0, viewSize.width, viewSize.height };
	
	
		{
			Gl3dRenderPassDesc passDesc = {};
			passDesc.cullFace = Gl3dCullFace::Disable;
			passDesc.depth = Gl3dDepth::Disable;
			passDesc.blending = Gl3dBlending::Disable;
			passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			passDesc.clearDepth = 1.0f;
			passDesc.clear = Gl3dClear::DepthColor;
			passDesc.viewport = { 0, 0, viewSize.width, viewSize.height };

			Gl3dRenderPas pass(_geometrySkybox.GetPtr(), &_geometryFrame, &passDesc);

			pass.Uniform("projection", projection);
			pass.Uniform("view", view);		
			_sphere.DrawIndexed(Gl3dPrimitive::Triangles, Gl3dFundamentalType::UInteger, _sphere_indeces->Length());
				
		}
		Gl3dDevice::Flush();

		{
			Gl3dRenderPassDesc passDesc = {};
			passDesc.cullFace = Gl3dCullFace::Back;
			passDesc.depth = Gl3dDepth::LessEqual;
			passDesc.blending = Gl3dBlending::Disable;
			passDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
			passDesc.clearDepth = 1.0f;
			passDesc.clear = Gl3dClear::None;
			passDesc.viewport = { 0, 0, viewSize.width, viewSize.height };

			

			{
				Gl3dRenderPas pass(_geometryTanTex.GetPtr(), &_geometryFrame, &passDesc);
				updater.pass = &pass;

				pass.Uniform("projection", projection);
				pass.Uniform("view", view);

				for (auto& x : renderMaterials)
				{
					if (x->GetRenderSources().Length())
					{
						ObjectMaterial* o = dynamic_cast<ObjectMaterial*>(x);

						pass.Uniform("static_albedo", o->albedo_static);

						if (o->roughness)
						{
							pass.Uniform("is_roughness", true);
							pass.Uniform("map_roughness", reinterpret_cast<Gl3dTexture*>(o->roughness->GetTextureObject()));
						}
						else
							pass.Uniform("is_roughness", false);

						if (o->metalic)
						{
							pass.Uniform("is_metalic", true);
							pass.Uniform("map_metalic", reinterpret_cast<Gl3dTexture*>(o->metalic->GetTextureObject()));
						}
						else
							pass.Uniform("is_metalic", false);

						if (o->ao)
						{
							pass.Uniform("is_ao", true);
							pass.Uniform("map_ao", reinterpret_cast<Gl3dTexture*>(o->ao->GetTextureObject()));
						}
						else
							pass.Uniform("is_ao", false);

						if (o->albedo)
						{
							pass.Uniform("is_albedo", true);
							pass.Uniform("map_albedo", reinterpret_cast<Gl3dTexture*>(o->albedo->GetTextureObject()));
						}
						else
							pass.Uniform("is_albedo", false);

						if (o->normals)
						{
							pass.Uniform("is_normal", true);
							pass.Uniform("map_normal", reinterpret_cast<Gl3dTexture*>(o->normals->GetTextureObject()));
						}
						else
							pass.Uniform("is_normal", false);
						pass.Uniform("mrao", o->metalic_roughness_ao_static);


						matCall++;
						for (const auto& sour : x->GetRenderSources())
							sour->Draw(&updater, RenderThreadTag());
					}					
				}
		
			}


			Gl3dDevice::Flush();
			{
				Gl3dRenderPas pass(_geometryTanTexInstancing.GetPtr(), &_geometryFrame, &passDesc);

				pass.Uniform("projection", projection);
				pass.Uniform("view", view);

				for (auto& x : renderMaterials)
				{
					if (x->GetRenderSources().Length())
					{
						ObjectMaterial* o = dynamic_cast<ObjectMaterial*>(x);
						pass.Uniform("static_albedo", o->albedo_static);

						if (o->roughness)
						{
							pass.Uniform("is_roughness", true);
							pass.Uniform("map_roughness", reinterpret_cast<Gl3dTexture*>(o->roughness->GetTextureObject()));
						}
						else
							pass.Uniform("is_roughness", false);

						if (o->metalic)
						{
							pass.Uniform("is_metalic", true);
							pass.Uniform("map_metalic", reinterpret_cast<Gl3dTexture*>(o->metalic->GetTextureObject()));
						}
						else
							pass.Uniform("is_metalic", false);

						if (o->ao)
						{
							pass.Uniform("is_ao", true);
							pass.Uniform("map_ao", reinterpret_cast<Gl3dTexture*>(o->ao->GetTextureObject()));
						}
						else
							pass.Uniform("is_ao", false);

						if (o->albedo)
						{
							pass.Uniform("is_albedo", true);
							pass.Uniform("map_albedo", reinterpret_cast<Gl3dTexture*>(o->albedo->GetTextureObject()));
						}
						else
							pass.Uniform("is_albedo", false);

						if (o->normals)
						{
							pass.Uniform("is_normal", true);
							pass.Uniform("map_normal", reinterpret_cast<Gl3dTexture*>(o->normals->GetTextureObject()));
						}
						else
							pass.Uniform("is_normal", false);
						pass.Uniform("mrao", o->metalic_roughness_ao_static);


						matCall++;
						for (const auto& sour : x->GetRenderSources())
						{
							sour->DrawInstanced(RenderThreadTag());
							drawCall++;
						}
					}					
				}
			}
		}
		Gl3dDevice::Flush();
		

		{			
			Gl3dRenderPas pass(_ssao.GetPtr(), &_pipeFrame[0], &postDesc);
			pass.Uniform("position", &_position);
			pass.Uniform("normal", &_normal);
			pass.Uniform("texNoise", &_ssaoNoise);
			pass.Uniform("renderSize", Vector2(viewSize.width, viewSize.height));
			pass.Uniform("sampleCount", _ssaoSamplesCount);
			pass.Uniform("projection", projection);
			pass.Uniform("view", view);			
			_quad.Draw(Gl3dPrimitive::Triangles, _quadVertexes->Length());
		}

		{
			Gl3dRenderPas pass(_ssaoBlur.GetPtr(), &_pipeFrame[1], &postDesc);
			pass.Uniform("ssaoInput", &_pipe[0]);
			_quad.Draw(Gl3dPrimitive::Triangles, _quadVertexes->Length());
		}

		Matrix4 shadowVP = RenderShadows();

		{
			Gl3dRenderPas pass(_blit.GetPtr(), nullptr, &postDesc);
			pass.Uniform("ssao", &_pipe[1]);
			pass.Uniform("color", &_color);
			pass.Uniform("position", &_position);
			pass.Uniform("normal", &_normal);
			pass.Uniform("shadowVP", shadowVP);
			pass.Uniform("shadowDepth", &_shadowDepth);
			pass.Uniform("lookPosition", lookPosition);
			pass.Uniform("rmo", &_rmo);		
			_quad.Draw(Gl3dPrimitive::Triangles, _quadVertexes->Length());
		}
		
		Gl3dDevice::Flush();
		Gl3dDevice::ThrowIfError();
	}

};

/*

hello world
hello
*/
//
class GameViewManager2 : public IRenderThreadInstance
{

public:

	Renderer* m_renderer;
	Window* m_window;
	CommandQueue m_queue;
	std::atomic_bool m_closeFlag;

	GameViewManager2() :
		m_closeFlag(false),
		m_renderer(nullptr),
		m_window(nullptr)		
	{
		m_queue.Attach();
	}

	~GameViewManager2()
	{
		
	}

	virtual CommandQueue & GetCommandQueue() override
	{
		return m_queue;
	}

	virtual bool GetCloseStatus() override
	{
		return m_closeFlag;
	}

	virtual void Update(GameInstance* instance) override
	{
		m_queue.QueueFunction([=]()
		{
			m_closeFlag = !m_window->Update();
		});
	}


	virtual void Draw(GameInstance* instance) override
	{
		m_queue.QueueFunction([=]()
		{
			TimeSpan s = Time::GetTotalMicroseconds();
	
			m_renderer->OnDraw();
			m_window->SwapBuffers();
			//printf("                                      fr: %f\n", TimeSpawnToFloatSeconds(Time::GetTotalMicroseconds() - s));
		});
	}

	virtual void Start(GameInstance* instance) override
	{
		m_queue.QueueFunctionWait([=]()
		{
			m_window = new Window();
			m_window->AttachKeyCallbacks(instance);
			Gl3dDevice::LinkAddresses((Gl3dDeviceLinker)glfwGetProcAddress);
			m_renderer = new Renderer();
		});
	}

	virtual void Stop(GameInstance* instance) override
	{
		m_queue.QueueFunctionWait([=]()
		{
			m_window->MakeCurrent();
			if (m_renderer)
			{
				delete m_renderer;
				m_renderer = nullptr;
			}
			if (m_window)
			{
				delete m_window;
				m_window = nullptr;
			}
		});
	}


};

class Comp : public Component
{
public:
	void Awake() override
	{	
		SetEnabled(true);


	}

	void FrameUpdate() override
	{
		if (Input::IsKeyDown(SGE_KEY_1))
		{
			//Vector3 size = Vector3(0.5f + Mathf::Random() * 2, 0.5f + Mathf::Random() * 2, 0.5f + Mathf::Random() * 2);
			Vector3 size = Vector3(4, 4, 4);
			GameObject* lookObject = new GameObject();
			lookObject->SetPosition(Vector3(0, 5, 0));
			lookObject->SetRotation(Quaternion(0.5, 0.1, 0.3));
			lookObject->AddComponent<RigidBody>()->SetMass(1.0f);
			lookObject->AddComponent<BoxCollider>()->SetSize(size);
			lookObject->AddComponent<SampleSyzaRender>()->m_isCube = false;
			lookObject->GetComponent<SampleSyzaRender>()->m_scale = Matrix4::Scale(size);
		}
		if (Input::IsKeyDown(SGE_KEY_2))
		{
			GameObject::ClearScene();
		}
		if (Input::IsKeyDown(SGE_KEY_3))
		{
			
		}
	}
};


int main()
{

	GameViewManager2 mag;

	std::atomic_bool core_thread_finished(false);
	std::thread core([&]()
	{	
		try
		{
			Guard g([]()
			{
				Network::UnboundDisconent();
			});


			GameInstance instnace(&mag);

			instnace.renderTickRate = 60;
			instnace.SetThisCurrent();


			GameObject* camera = new GameObject("Camera");
			camera->SetTransform(Vector3(-7, 7, -7), Quaternion(-45.0f, 45.0f, 0.0f));
			Display::GetCurrent()->SetCamera(camera);


			/*	for(int i = 0; i < 10; i++)
					for (int j = 0; j < 10; j++)
					{
						GameObject* lookObject = new GameObject();
						lookObject->SetPosition(Vector3(i - 5, -1, j + 5));
						lookObject->AddComponent<FixedRotation>();
						lookObject->AddComponent<SampleSyzaRender>();
					}
				*/
			{
				GameObject* lookObject = new GameObject();
				lookObject->SetPosition(Vector3(0, -3, 0));
				lookObject->AddComponent<RigidBody>()->SetMass(0.0f);
				lookObject->AddComponent<BoxCollider>()->SetSize(Vector3(62.0f, 1.0f, 62.0f));
				lookObject->AddComponent<SampleSyzaRender>()->m_isCube = true;
				lookObject->GetComponent<SampleSyzaRender>()->m_scale = Matrix4::Scale(Vector3(62.0f, 1.0f, 62.0f));
			}


			GameObject* d = new GameObject();
			d->AddComponent<Comp>();
			camera->AddComponent<DebugFlyCamera>();



			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{6a3a4afd-bb960a17-53512721}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{7d122e3d-a1d3f317-5674f320}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{9fbb0595-80ae7301-842e2393}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{21b3bd3d-c8c41717-42e3a324}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{89dfd3ac-9d23e51f-a9ea946b}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{369bd9fd-d281ee17-47c67725}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{446a837d-8f1df817-591a8f23}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{5342e6bd-95580117-5c3f5b11}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/{5342e7bd-95580117-5c3f5b22}.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/v0.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/v1.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/v2.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/v4.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/v5.indent");
			ResourceLoader::LoadResourcesPk2("../BotQ/Data/v6.indent");

			LoadDefaultPrefabs();

			GameObject::SpawnPrefab("/NT_Barrils.prefab", "", Vector3::Zero(), Quaternion::Identity());
			GameObject::SpawnPrefab("/NT_PbrSpheres.prefab", "", Vector3::Zero(), Quaternion::Identity());
			GameObject::SpawnPrefab("/Terrain.prefab", "", Vector3::Zero(), Quaternion::Identity());
			GameObject::SpawnPrefab("/Doom.prefab", "", Vector3(-36, -3.1, -13), Quaternion(0, (Mathf::Pi() / 3.5f) * RAD_TO_DEG, 0));
			GameObject::SpawnPrefab("/Palms.prefab", "", Vector3::Zero(), Quaternion::Identity());
			GameObject::SpawnPrefab("/SandHome.prefab", "", Vector3(0, -2, 0), Quaternion::Identity());
			GameObject::SpawnPrefab("/Sphere.prefab", "", Vector3(-42, -2, 0), Quaternion::Identity());

			for (int i = 0; i < 13; i++)
				if (i % 2 == 0)
					GameObject::SpawnPrefab("/Matbol.prefab", "", Vector3(i * 1.8, 28, -6), Quaternion::Identity());
				else
					GameObject::SpawnPrefab("/Ogne.prefab", "", Vector3(i * 1.8, 28, -6), Quaternion::Identity());

			for (int i = 0; i < 10; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					GameObject::SpawnPrefab("/Cars/PickUp.prefab", "", Vector3(j * 4.0, 10, i * 4.0), Quaternion::Identity());
				}
			}

			while (instnace.GameLoop())
			{
			}
		}
		catch (std::exception& ex)
		{
			printf("==========[CORE THREAD EXIT WAS ERROR]==========\n\s", ex.what());
		}
		core_thread_finished.store(true);
	});

	//SetThreadPriority(core.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
	core.detach();
	while (!core_thread_finished.load())
	{
		mag.m_queue.Playback();
	}
	return 0;
}
