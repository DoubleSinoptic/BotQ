
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



class FacControl : public Component
{
public:

	void Awake()override
	{
		SetEnabled(true);
	}
	GameObject* postProxy;
	GameObject*  preProxy;
	GameObject* camera;
	float shootingSpeedPerMinute = 900;
	float verticalRecoil = 1.0f;
	float horizantalRecoil = 0.4f;
	float upAngle = 4.0f;
	float recoilDeltaDistance = 0.003f;
	float recoilDistance = 0.05f;
	float movementNomenator = 0.6f;
    float movementDenomenat = 4.0f;

	Vector3 moveLoc = Vector3(0.094f, -0.077f, 0.341f);
	Vector3 shootLoc =  Vector3(0.0598f, 0.032f, 0.0f);
	Vector3 moveFireLoc =  Vector3(0.0924f, -0.077f, 0.283f);
	Quaternion moveFireQuat = Quaternion(-3.0f, 0, 0);
	Quaternion moveingRot = Quaternion::Identity();
	Vector3 moveingRotationDonor = Vector3::Zero();

	void CamRight(float ds)
	{
		preProxy->SetLocalRotation(Quaternion(0, ds, 0));
	}

	float randOm(float x, float b)
	{
		int rn = rand();
		float ge = float(rn) / float(RAND_MAX);
		return x + (b - x) * ge;
	}

	bool IsMoving = true;
	float strafingCoof = 0.0f;

	float last = 0.0f;
	void PhysicUpdate() override
	{

		float verticalDonorScale = 0.003f;
		float horizontalDonorScale = 0.01f;

		float timeScaleMoveing = Time::GetTotalTime() * 8.0f;
		moveingRotationDonor =  Vector3(
			sinf(timeScaleMoveing) * horizontalDonorScale - (horizontalDonorScale / 2.0f),
			Mathf::Abs(cosf(timeScaleMoveing) * verticalDonorScale) - (verticalDonorScale / 2.0f),
			0);
		Vector3 straifAdditioanl = Vector3(strafingCoof, 0, 0) * 0.4f + (strafingCoof == 0.0f ? Vector3::Zero() : 
			(moveingRotationDonor * 0.5f));

		if (!IsMoving)
			moveingRotationDonor = Vector3::Zero();

		if (Input::IsMouseKeyDown(SGE_MOUSE_BUTTON_2))
		{
			preProxy->SetLocalPosition(Vector3::Lerp(preProxy->GetLocalPosition(), shootLoc
				+ moveingRotationDonor * 0.2f
				+ straifAdditioanl * 0.2f, 0.2f));
		}
		else
		{

			preProxy->SetLocalPosition(Vector3::Lerp(preProxy->GetLocalPosition(), Vector3::Zero()
				+ moveingRotationDonor
				+ straifAdditioanl, 0.2f));
		}

		float timeScale = 60.0f * Time::GetDeltaTime();
		float realMovementScale = movementNomenator * timeScale;

		Quaternion staticLop = Quaternion(0.0f, 0.0f, 0.0f);
		Quaternion upLop = Quaternion(upAngle * -timeScale, 0.0f, 0.0f);
		Vector3 flowPos = Vector3(0.0f, 0.0f, -1.0f * recoilDeltaDistance);

		if (Input::IsKeyDown(SGE_KEY_SPACE))
		{
			float current = Time::GetTotalTime();
			if (last == 0.0f)
				last = current;

			float detla = current - last;
			if (detla >= (60.0f / shootingSpeedPerMinute))
			{
				last = current;
				//weapomPl.Play();

				postProxy->SetLocalRotation( Quaternion::Slerp(postProxy->GetLocalRotation(), moveFireQuat, realMovementScale));
				postProxy->SetLocalPosition( Vector3::Lerp(postProxy->GetLocalPosition(), moveFireLoc, realMovementScale));


				float verF = randOm(0.0f, 1.0f) * verticalRecoil;
				float horF = randOm(-1.0f, 1.0f) * horizantalRecoil;
	
				camera->SetLocalRotation( Quaternion(
					camera->GetLocalRotation().GetEuler() + Vector3(-verF, 0, 0) ));

				camera->SetLocalRotation(Quaternion(
					camera->GetLocalRotation().GetEuler() + Vector3(0, horF, 0)));
			}
			else
			{
				postProxy->SetLocalRotation(Quaternion::Slerp(postProxy->GetLocalRotation(), Quaternion::Identity(), realMovementScale / movementDenomenat));
				postProxy->SetLocalPosition(Vector3::Lerp(postProxy->GetLocalPosition(), moveLoc, realMovementScale / movementDenomenat));
			}
		}
		else
		{

			postProxy->SetLocalRotation(Quaternion::Slerp(postProxy->GetLocalRotation(), Quaternion::Identity(), realMovementScale / movementDenomenat));
			postProxy->SetLocalPosition(Vector3::Lerp(postProxy->GetLocalPosition(), moveLoc, realMovementScale / movementDenomenat));
		}

	}


};



class CameraController : public Component
{
public:
	FacControl * attac;
	float angleX = 0.0;
	float angleY = 0.0;
	bool hasPressed = true;
	Point curre23nt = Point(0, 0);
	void Awake()override
	{
		SetEnabled(true);
	}
	int dx = 0;
	int dy = 0;
	float slesh = 0;

	void PhysicUpdate() override
	{

		float mull = 1.0f;
		const float FlashSpeed = 40.5f;
		const float thisSpeed = FlashSpeed;
		float renderDelta = Time::GetRenderDeltaTime();

		Vector3 forward = (GetGameObject()->GetForward() *  Vector3(1.0f, 1.0f, 1.0f)).Normalized();
		Vector3 right = (GetGameObject()->GetRight() *  Vector3(1.0f, 1.0f, 1.0f)).Normalized();
		attac->IsMoving = false;
		Vector3 acum = Vector3::Zero();
		if (Input::IsKeyDown(SGE_KEY_W))
		{
			attac->IsMoving = true;
			acum += forward;
		}
		if (Input::IsKeyDown(SGE_KEY_S))
		{
			attac->IsMoving = true;
			acum -= forward;
		}
		if (Input::IsKeyDown(SGE_KEY_A))
		{
			acum += right;
		}
		if (Input::IsKeyDown(SGE_KEY_D))
		{
			acum -= right;
		}
		
		Vector3 e = acum.Normalized() * 2.9f * Time::GetDeltaTime();
		if (Input::IsKeyDown(SGE_KEY_LEFT_SHIFT))
		{
			e *= 0.1;
		}
	
		attac->strafingCoof = e.Dot(GetGameObject()->GetRight());
		GetGameObject()->SetPosition(GetGameObject()->GetPosition() + e);

		if (Input::IsMouseKeyDown(SGE_MOUSE_BUTTON_LEFT))
		{
			if (hasPressed)
			{
				const Point mousePoint = Input::GetMausePosition();
				dx = mousePoint.x - curre23nt.x;
				dy = mousePoint.y - curre23nt.y;
				curre23nt = mousePoint;

				GetGameObject()->SetLocalRotation(Quaternion(
					GetGameObject()->GetLocalRotation().GetEuler()
					+ Vector3(dy * 0.1f, -dx * 0.1f, 0)));
				float a = slesh;
				float b = -dx;
				slesh = (a + (b - a) * 0.1);
				attac->CamRight(slesh * 0.2f);
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
	{///Ak12.prefab

		GameObject::SpawnPrefab("/NT_Barrils.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject::SpawnPrefab("/NT_PbrSpheres.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject::SpawnPrefab("/Terrain.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject::SpawnPrefab("/Doom.prefab", "", Vector3(-36, -3.1, -13), Quaternion(0, (Mathf::Pi() / 3.5f) * RAD_TO_DEG, 0));
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

		GameObject* ak12 = GameObject::SpawnPrefab("/Ak12.prefab", "", Vector3::Zero(), Quaternion::Identity());
		GameObject* cam = Display::GetCurrent()->GetCamera();
		GameObject* stablePositionAndRotation = new GameObject();
		GameObject* stableProxy = new GameObject();

		stablePositionAndRotation->SetParent(cam);
		stableProxy->SetParent(stablePositionAndRotation);
		ak12->SetParent(stableProxy);

		ak12->SetLocalPosition(Vector3(-0.006f, -1.678f, -0.329f));
	

		FacControl* e = cam->AddComponent<FacControl>();
		e->camera = cam;
		e->postProxy = stableProxy;
		e->preProxy = stablePositionAndRotation;
		
		cam->GetComponent<CameraController>()->attac = e;
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