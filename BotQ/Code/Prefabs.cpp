
#include <Component.h>
#include <Level.h>
#include <Graphics/Bitmap.h>
#include <Graphics/Texture.h>
#include <Resource.h>
#include <Sge2Common.h>
#include <MeshImporter.h>
#include <Tools.h>
#include <Input.h>
#include <GameInstance.h>
#include <Common/TinyFile.h>
#include <Time.hpp>
#include <GameObject.h>
#include <PhysicsComponents/BoxCollider.h>
#include <PhysicsComponents/TrianglesCollider.h>
#include <PhysicsComponents/CarCollider.h>
#include <Display.h>
#include <PhysicsComponents/RigidBody.h>
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
	GameObject* ogne = MeshImporter::Import("/assets/" + name + "/Sphere.FBX")->construct();

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
	GameObject* ogne = MeshImporter::Import("/assets/" + name + "/Barril.FBX")->construct();

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
		GameObject*  obj = MeshImporter::Import("/assets/GrassBlock/GrassBlock.fbx")->construct();
		obj->AddComponent<RigidBody>()->SetMass(1.0);
		obj->AddComponent<BoxCollider>()->SetSize(Vector3(1.0, 1.0, 1.0));
		return obj;
	});

	GameObject::AddPrefab("/DebugCube.prefab", []()
	{
		GameObject*  obj = MeshImporter::Import("/assets/DebugCube/DebugCube.fbx")->construct();
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
		GameObject* obj = MeshImporter::Import("/assets/matbol/export3dcoat.obj")->construct();
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
		GameObject* pixkup = MeshImporter::Import("/assets/PickUp/PickUpBody.fbx")->construct();

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

		GameObject* wheelExample1 = MeshImporter::Import("/assets/PickUp/PickUpWheel.fbx")->construct();
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
		GameObject* ct = MeshImporter::Import("/assets/Terrain/Terrain.fbx")->construct();
		ct->AddComponent<RigidBody>()->SetMass(0.0);
		ct->AddComponent<TrianglesCollider>()->CollectMeshesFromRenderer();

		//Tools::SetMaps(ct,
		//	LoadTextureE("/assets/Custom/leaf-fall1-albedo.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall3-normal-ue.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall1-metalness.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall1-roughness.png"),
		//	LoadTextureE("/assets/Custom/leaf-fall1-ao.png"));

		Tools::SetMaps(ct,
			LoadTextureE("/assets/Sand/sandyground1_Base_Color.png"),
			LoadTextureE("/assets/Sand/sandyground1_Normal.png"),
			LoadTextureE("/assets/Sand/sandyground1_Metallic.png"),
			LoadTextureE("/assets/Sand/sandyground1_Roughness.png"),
			LoadTextureE("/assets/Sand/sandyground1_Ambient_Occlusion.png"));


		return ct;
	});

	GameObject::AddPrefab("/Doom.prefab", []()
	{
		GameObject* d2 = MeshImporter::Import("/assets/Doom/Doom.FBX")->construct();
		d2->AddComponent<RigidBody>()->SetMass(0.0);
		d2->AddComponent<TrianglesCollider>()->CollectMeshesFromRenderer(Vector3(4.0f, 4.0f, 4.0f));
		Tools::ScaleAll(d2, 4.0f);
		return d2;
	});


	GameObject::AddPrefab("/SandHome.prefab", []()
	{
		GameObject* home = MeshImporter::Import("/assets/SandHome/SandHome.FBX")->construct();
		home->AddComponent<RigidBody>()->SetMass(0.0);
		home->AddComponent<TrianglesCollider>()->CollectMeshesFromRenderer();
		return home;
	});

	GameObject::AddPrefab("/Ak12.prefab", []()
	{
		GameObject* home = MeshImporter::Import("/assets/ak12/ak12.obj")->construct();
		return home;
	});

	GameObject::AddPrefab("/Ogne.prefab", []()
	{
		GameObject* ogne = MeshImporter::Import("/assets/Ogne/Ogne.FBX")->construct();
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
		GameObject* d2 = MeshImporter::Import("/assets/Palms/Palms.FBX")->construct();
		Tools::ScaleAll(d2, 0.3);
		return d2;
	});

	GameObject::AddPrefab("/Matbol.prefab", []()
	{
		GameObject* obj = MeshImporter::Import("/assets/matbol/export3dcoat.obj")->construct();
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
				GameObject* obj = MeshImporter::Import("/assets/Grass/Sphere.FBX")->construct();
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
