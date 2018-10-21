
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
		GameObject::SpawnPrefab("/NT_Barrils.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject::SpawnPrefab("/NT_PbrSpheres.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject::SpawnPrefab("/Terrain.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject::SpawnPrefab("/Doom.prefab", "", Vector3(-36, -3.1, -13), Quaternion(0, Mathf::Pi() / 3.5f, 0));
		GameObject::SpawnPrefab("/Palms.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject::SpawnPrefab("/SandHome.prefab", "", Vector3(0, -2, 0), Quaternion::Identity());
		GameObject::SpawnPrefab("/SandHome.prefab", "", Vector3(0, -2, 0), Quaternion::Identity());
		for (int i = 0; i < 13; i++)
			if(i % 2 == 0)
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
		Display::GetCurrent()->GetCamera()->AddComponent<CameraController>();
	}

	virtual void UnloadLevel() override
	{
	}

	virtual void UpdateLevel() override
	{
		if (Input::IsKeyDown(SGE_KEY_F1))
		{
			GameObject::SafePrefabsTo("./KubizmLevel.xml");
		}
		/*if (Input::IsKeyDown(SGE_KEY_F1))
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


		}*/
		
	}
};

LevelBase* CreateKubizm()
{
	return new Kubizm();
}