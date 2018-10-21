
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
auto LoadTextureFileE = [](const String& path) {
	Ref<Bitmap> bit = new Bitmap(path);
	Texture* r = new Texture();
	r->SetFromBitmap(bit);
	return r;
};

auto LoadTextureE = [](const String& path) {
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

void LoadModelsE(const String& name, const Vector3& pos)
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
/*

*/

//class ControllDelete : public Component {
//	Ref<EventHandler <>> mApplayHandler;
//public:
//	void Awake() override {
//		mApplayHandler = new EventHandler<>([=]()
//		{
//			if (GetGameObject()->GetPosition().y < 100.0)
//			{
//				Log("Deleted game object of distance");
//				delete GetGameObject();
//
//			}
//		});
//		GetGameObject()->GetHandlers()->OnTransformChanged += mApplayHandler.GetPtr();
//	}
//};


class CameraController : public Component
{
public:
	
	float angleX = 0.0;
	float angleY = 0.0;
	bool hasPressed = true;
	Point curre23nt = Point(0, 0);
	void Awake()override 
	{
		SetEnabled(true);
	}

	void FrameUpdate() override
	{

		float mull = 1.0f;
		const float FlashSpeed = 40.5f;
		const float thisSpeed = FlashSpeed;
		float renderDelta = Time::GetRenderDeltaTime();
		if (Input::IsKeyDown(SGE_KEY_LEFT_SHIFT))
		{
			mull = 0.1;
		}

		if (Input::IsKeyDown(SGE_KEY_W))
		{
			GetGameObject()->SetPosition(GetGameObject()->GetPosition() + GetGameObject()->GetForward() * renderDelta* thisSpeed* mull);
		}
		if (Input::IsKeyDown(SGE_KEY_S))
		{
			GetGameObject()->SetPosition(GetGameObject()->GetPosition() - GetGameObject()->GetForward() * renderDelta * thisSpeed* mull);
		}
		if (Input::IsKeyDown(SGE_KEY_A))
		{
			GetGameObject()->SetPosition(GetGameObject()->GetPosition() + GetGameObject()->GetRight() * renderDelta * thisSpeed* mull);
		}
		if (Input::IsKeyDown(SGE_KEY_D))
		{
			GetGameObject()->SetPosition(GetGameObject()->GetPosition() - GetGameObject()->GetRight()  * renderDelta * thisSpeed* mull);
		}
		if (Input::IsMouseKeyDown(SGE_MOUSE_BUTTON_LEFT))
		{
			if (hasPressed)
			{
				const Point mousePoint = Input::GetMausePosition();
				const int dx = mousePoint.x - curre23nt.x;
				const int dy = mousePoint.y - curre23nt.y;
				curre23nt = mousePoint;
				angleX -= dx * Mathf::Pi() / (360 * 4);
				angleY += dy * Mathf::Pi() / (360 * 4);
				GetGameObject()->SetRotation(Quaternion(angleY, angleX, 0));

			}
			else
			{
				hasPressed = true;
				curre23nt = Input::GetMausePosition();
			}


		}
		else
			hasPressed = false;

	}



};


class Kubizm : public LevelBase
{
public:
	GameObject * cart;
	bool kalel = true;
	DynamicArray<GameObject*> _cars;
	// Унаследовано через LevelBase
	virtual void LoadLevel() override
	{
		

		//{
		//	GameObject* bassic = new GameObject();
		//	bassic->SetPosition(Vector3(0, 100, 0));
		//	
		//	bassic->AddComponent<FixedRotation>();
		//	GameObject* obj = MeshImporter::Import("./Data/Cerberus/Cerberus_LP.FBX")->construct();
		//	obj->SetLocalPosition(Vector3(0, 0, 0));
		//	obj->SetParent(bassic);
		//	
		//	Tools::SetMaps(obj,
		//		LoadTexture("./Data/Cerberus/A.tga"),
		//		LoadTexture("./Data/Cerberus/N.tga"),
		//		LoadTexture("./Data/Cerberus/M.tga"),
		//		LoadTexture("./Data/Cerberus/R.tga"),
		//		/*LoadTexture("./Data/Cerberus/AO.jpg")*/nullptr);
		//}


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


		/*LoadModels("Gold", Vector3(5, 40, 0));
		LoadModels("Grass", Vector3(7, 40, 0));
		LoadModels("Plasic", Vector3(9, 40, 0));
		LoadModels("RustedIron", Vector3(11, 40, 0));
		LoadModels("Wall", Vector3(13, 40, 0));*/

		LoadModelsE("BarrilA", "drum1", Vector3(17, 40, 0));
		LoadModelsE("BarrilB", "drum2", Vector3(19, 40, 0));
		LoadModelsE("BarrilC", "drum3", Vector3(21, 40, 0));

		/*GameObject* whiteh = MeshImporter::ImportInstanced("/assets/WhiteHouse/House.FBX")->Construct();
		whiteh->SetPosition(Vector3(0, 40, 0));*/

		for (int i = 0; i < 13; i++)
		{
			if(i % 2 == 0)
			{
				GameObject* obj = MeshImporter::Import("/assets/matbol/export3dcoat.obj")->construct();
				obj->SetPosition(Vector3(i * 1.8, 28, -6));
				obj->AddComponent<FixedRotation>();
				Tools::ScaleAll(obj, 0.1);
				Tools::SetMaps(obj,
					LoadTextureE("/assets/Matbol/A.tga"),
					LoadTextureE("/assets/Matbol/N.tga"),
					LoadTextureE("/assets/Matbol/M.tga"),
					LoadTextureE("/assets/Matbol/R.tga"),
					/*LoadTexture("./Data/Cerberus/AO.jpg")*/nullptr);
			}
			else 
			{
				GameObject* ogne = MeshImporter::Import("/assets/Ogne/Ogne.FBX")->construct();

				ogne->SetPosition(Vector3(i * 1.8, 28, -6));
				ogne->AddComponent<FixedRotation>();
				Tools::ScaleAll(ogne, 0.05);

				Tools::SetMaps(ogne,
					LoadTextureE("/assets/Ogne/1_BC.png"),
					LoadTextureE("/assets/Ogne/1_N.jpg"),
					LoadTextureE("/assets/Ogne/1_M.jpg"),
					LoadTextureE("/assets/Ogne/1_R.jpg"),
					LoadTextureE("/assets/Ogne/1_AO.jpg"));
			}


			
		}
		

		
		{
			GameObject* d2 = MeshImporter::Import("/assets/Palms/Palms.FBX")->construct();
			Tools::ScaleAll(d2, 0.3);
		}

#define USE_LANDSCAPE
#ifdef USE_LANDSCAPE
		GameObject* ct = MeshImporter::Import("/assets/Terrain/Terrain.fbx")->construct();
		ct->AddComponent<RigidBody>()->SetMass(0.0);
		ct->AddComponent<TrianglesCollider>()->CollectMeshesFromRenderer();

		/*Tools::SetMaps(ct,
			LoadTexture("/assets/DebugCube/Debug.png"),
			nullptr,
			nullptr,
			nullptr,
			nullptr);*/
		Tools::SetMaps(ct,
			LoadTextureE("/assets/Custom/leaf-fall1-albedo.png"),
			LoadTextureE("/assets/Custom/leaf-fall3-normal-ue.png"),
			LoadTextureE("/assets/Custom/leaf-fall1-metalness.png"),
			LoadTextureE("/assets/Custom/leaf-fall1-roughness.png"),
			LoadTextureE("/assets/Custom/leaf-fall1-ao.png"));
		/*Tools::SetMaps(ct,
			LoadTexture("/assets/grass-bpr/mixedmoss-albedo2.png"),
			LoadTexture("/assets/grass-bpr/mixedmoss-normal2.png"),
			LoadTexture("/assets/grass-bpr/mixedmoss-metalness.png"),
			LoadTexture("/assets/grass-bpr/mixedmoss-roughness.png"),
			LoadTexture("/assets/grass-bpr/mixedmoss-ao2.png"));*/
#else
		GameObject* landscapeBody = new GameObject();
		landscapeBody->AddComponent<RigidBody>()->SetMass(0.0);
		landscapeBody->AddComponent<BoxCollider>()->SetSize(Vector3(200, 1.0, 200));

		auto cts = MeshImporter::Import("/assets/DebugCube/DebugCube.fbx");
		for (int i = 0; i < 200; i++)
		{
			for (int j = 0; j < 200; j++)
			{
				GameObject* ct = cts->construct();
				ct->SetPosition(Vector3(i, 0, j) - Vector3(100, 0, 100));

				Tools::RotateLocalAll(ct, Quaternion::Identity());
				Tools::SetMaps(ct,
					LoadTexture("/assets/DebugCube/Debug.png"),
					nullptr,
					nullptr,
					nullptr,
					nullptr);
			}
		}
#endif

	
		/*for (int i = 0; i < 60; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				GameObject*  obj = MeshImporter::Import("/assets/GrassBlock/GrassBlock.fbx")->construct();
				obj->AddComponent<RigidBody>()->SetMass(1.0);
				obj->AddComponent<BoxCollider>()->SetSize(Vector3(1.0, 1.0, 1.0));
				obj->SetPosition(Vector3(i * 2, 20, j * 2));
			}
		}*/
		/*for (int i = 0; i < 60; i++)
		{
			for (int j = 0; j < 60; j++)
			{
				GameObject*  obj = MeshImporter::Import("/assets/GrassBlock/GrassBlock.fbx")->construct();		
				obj->SetPosition(Vector3(i, 20, j));
			}
		}*/
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 3; j++)
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
			
				cart = car;
				cart->SetPosition(Vector3(j * 4.0, 10, i * 4.0));
				if(i != 59)
					_cars.Add(car);
			}
		}
		Display::GetCurrent()->GetCamera()->AddComponent<CameraController>();
	}

	virtual void UnloadLevel() override
	{
	}

	virtual void UpdateLevel() override
	{
		if (Input::IsKeyDown(SGE_KEY_F1))
		{
			GameObject*  obj = MeshImporter::Import("/assets/GrassBlock/GrassBlock.fbx")->construct();
			obj->AddComponent<RigidBody>()->SetMass(1.0);
			obj->AddComponent<BoxCollider>()->SetSize(Vector3(1.0, 1.0, 1.0));
		}
		if (Input::IsKeyDown(SGE_KEY_F3))
		{
			GameObject* t = GameObject::Clone(cart);
			_cars.Add(t);
			t->GetComponent<RigidBody>()->SetLinearVelocity(Vector3::Zero());
			t->GetComponent<RigidBody>()->SetAngularVelocity(Vector3::Zero());

			t->SetPosition(GameObject::FindObject("Camera")->GetPosition());
			t->SetRotation(GameObject::FindObject("Camera")->GetRotation());
		}
		if (Input::IsKeyDown(SGE_KEY_F4))
		{
			if (kalel)
			{
				for (auto i : _cars)
					delete i;
				kalel = false;
				Log("n5y5ab54esh Deleted!");
			}


		}
		
	}
};

LevelBase* CreateKubizm()
{
	return new Kubizm();
}