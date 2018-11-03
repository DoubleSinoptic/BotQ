
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
#include <Display.h>
#include <GameObject.h>
#include <PhysicsComponents/BoxCollider.h>
#include <PhysicsComponents/TrianglesCollider.h>
#include <PhysicsComponents/CarCollider.h>
#include <PhysicsComponents/Physics.h>
#include <PhysicsComponents/RigidBody.h>
#include <Display.h>
#include <Network/Network.h>
#include <Network/NetworkEntityBase.h>

class CarControlFlow : public NetworkEntityBase
{
public:
	//int CarKeysCodes[4] = { SGE_KEY_W, SGE_KEY_S, SGE_KEY_A, SGE_KEY_D };
	bool keyStates[4] = { false, false, false, false };
	void rpc(RPC_ENTITY_ARGS) override
	{
		NetworkEntityBase::rpc(calledUser, rpcName, arguments);
		if (rpcName == ts::cmd_hash("@Pressed"))
		{
			keyStates[arguments.read_object<int>()] = true;;

		}
		else if (rpcName == ts::cmd_hash("@Unpressed"))
		{
			keyStates[arguments.read_object<int>()] = false;;
		}
	}

	void OnStruct(ts::binary_stream& s) override
	{
		s.write_object<bool>(keyStates[0]);
		s.write_object<bool>(keyStates[1]);
		s.write_object<bool>(keyStates[2]);
		s.write_object<bool>(keyStates[3]);
		CarCollider* c = GetGameObject()->GetComponent<CarCollider>();
		if (c)
			c->ApplayDriverState(keyStates[0], keyStates[1], keyStates[2], keyStates[3]);
	}

	void OnDestruct(ts::binary_stream& s) override
	{
		keyStates[0] = s.read_object<bool>();
		keyStates[1] = s.read_object<bool>();
		keyStates[2] = s.read_object<bool>();
		keyStates[3] = s.read_object<bool>();
		CarCollider* c = GetGameObject()->GetComponent<CarCollider>();
		if (c)
			c->ApplayDriverState(keyStates[0], keyStates[1], keyStates[2], keyStates[3]);
	}

	//void PhysicUpdate() override
	//{
	//	/*	CarCollider* c = GetGameObject()->GetComponent<CarCollider>();
	//	if (c)
	//	c->ApplayDriverState(keyStates[0], keyStates[1], keyStates[2], keyStates[3]);

	//	NetworkEntityBase::PhysicUpdate();*/
	//}

};
class UserControlFlowNetworkIng : public NetworkEntityBase
{
public:
	void rpc(RPC_ENTITY_ARGS) override
	{
		NetworkEntityBase::rpc(calledUser, rpcName, arguments);

	}

	bool isPresed2 = false;
	bool isPresed = false;
	ts::entity_id carId;
	int CarKeysCodes[4] = { SGE_KEY_W, SGE_KEY_S, SGE_KEY_A, SGE_KEY_D };
	bool keyStates[4] = { false, false, false, false };

	CarControlFlow* inCar = nullptr;

	void PhysicUpdate() override
	{
		if (inCar)
		{

			for (int i = 0; i < 4; i++)
			{
				if (Input::IsKeyDown(CarKeysCodes[i]))
				{
					if (!keyStates[i])
					{
						keyStates[i] = true;
						ts::package_stream<100> gs;
						gs.write_object(i);
						inCar->call(ts::cmd_hash("@Pressed"), gs);
					}
				}
				else
				{
					if (keyStates[i])
					{
						keyStates[i] = false;
						ts::package_stream<100> gs;
						gs.write_object(i);
						inCar->call(ts::cmd_hash("@Unpressed"), gs);
					}
				}
			}
			/*CarCollider* c = inCar->GetGameObject()->GetComponent<CarCollider>();
			if (c)
			{
			c->ApplayDriverState(keyStates[0], keyStates[1], keyStates[2], keyStates[3]);
			}*/

		}


		if (Input::IsKeyDown(SGE_KEY_E))
		{
			if (!isPresed2)
			{
				isPresed2 = true;

				if (inCar)
				{


					Display::GetCurrent()->SetCamera(GetGameObject());
					inCar = nullptr;
				}
				else
				{
					RayHitRezult h;
					if (Physics::RayCast(GetGameObject()->GetPosition(), GetGameObject()->GetForward(), h, 2))
					{
						RigidBody* body = h.body;
						if (body)
						{
							GameObject* g = body->GetGameObject();
							inCar = g->GetComponent<CarControlFlow>();
							GetGameObject()->SetLocalPosition(Vector3::Zero());
							GetGameObject()->SetLocalRotation(Quaternion::Identity());

							if (!GetGameObject()->FindChild("Serbio"))
							{
								GameObject* s = new GameObject("Serbio");
								s->SetParent(GetGameObject());
								s->SetLocalPosition({ 0, 1.3f, -5 });
							}

							Display::GetCurrent()->SetCamera(GetGameObject()->FindChild("Serbio"));
							/*

							for (int i = 0; i < (sizeof(buffers) / sizeof(Vector3)); i++)
							{
							buffers[i] = GetGameObject()->GetPosition();
							}*/
						}
					}
				}

			}
		}
		else
			isPresed2 = false;

		if (Input::IsKeyDown(SGE_KEY_SPACE))
		{
			if (!isPresed)
			{
				isPresed = true;
				ts::package_stream<100> paks;
				paks.write_object(GetGameObject()->GetPosition());
				paks.write_object(GetGameObject()->GetRotation());
				paks.write_object(GetGameObject()->GetForward() * 10.0f);
				Network::GetFromNetworkId<NetworkEntityBase>(2)->call(ts::cmd_hash("@Fire"), paks);
			}
		}
		else
			isPresed = false;
		if (inCar)
			Display::GetCurrent()->SetCamera(GetGameObject()->FindChild("Serbio"));
		else
			Display::GetCurrent()->SetCamera(GetGameObject());
	}



	float angleX = 0.0;
	float angleY = 0.0;
	bool hasPressed = true;
	Point curre23nt = Point(0, 0);

	//Vector3 buffers[40];

	//void move_flow_effect_buffer() 
	//{
	//	for (int i = (sizeof(buffers) / sizeof(Vector3)) - 1; i > 0 ; i--)
	//		buffers[i] = buffers[i - 1];	
	//}

	void FrameUpdate() override
	{
		/*
		GetGameObject()->SetLocalPosition({ 0, 1.3f, -4 });
		GetGameObject()->SetRotation(Quaternion::Identity());
		*/
		if (inCar)
		{

			GameObject* cam = GetGameObject();
			GameObject* car = inCar->GetGameObject();
			cam->SetPosition(car->GetPosition());
			if (Input::IsMouseKeyDown(SGE_MOUSE_BUTTON_LEFT))
			{
				if (hasPressed)
				{
					const Point mousePoint = Input::GetMausePosition();
					const int dx = (mousePoint.x - curre23nt.x);
					const int dy = (mousePoint.y - curre23nt.y);
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

			/*move_flow_effect_buffer();
			buffers[0] = car->GetPosition();*/

			return;
		}
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

				GetGameObject()->SetLocalRotation(Quaternion(
					GetGameObject()->GetLocalRotation().GetEuler()
					+ Vector3(dy * 0.1f, -dx * 0.1f, 0)));

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


class LevelManagent : public NetworkEntityBase
{
public:
	void rpc(RPC_ENTITY_ARGS) override
	{
		NetworkEntityBase::rpc(calledUser, rpcName, arguments);
		if (Network::IsServer())
		{
			if (rpcName == ts::cmd_hash("@QueryCameraObject"))
			{
				ts::entity_id id = NetworkEntityBase::AllocateId();
				Network::Instantiate("/Camera.prefab", "UserControlFlowNetworkIng", id);
				printf("quuey id: %d\n", id);
				ts::package_stream<100> s;
				s.write_uint32(id);
				call(ts::cmd_hash("@you_camera"), calledUser, s);

			}
			else if (rpcName == ts::cmd_hash("@Fire"))
			{
				GameObject* o = Network::Instantiate("/DebugBox.prefab", "NetworkEntityBase", NetworkEntityBase::AllocateId());
				o->SetPosition(arguments.read_object<Vector3>());
				o->SetRotation(arguments.read_object<Quaternion>());
				NetworkEntityBase* c = o->GetComponent<NetworkEntityBase>();
				c->SetStateSync(8);
				c->EnableVelocitySync();
				RigidBody* b = o->GetComponent<RigidBody>();
				b->SetLinearVelocity(arguments.read_object<Vector3>());
			}

		}
		else
		{
			if (rpcName == ts::cmd_hash("@you_camera"))
			{

				ts::entity_id gid = arguments.read_uint32();
				Log("asfasf: %d", gid);
				UserControlFlowNetworkIng* nt = Network::GetFromNetworkId<UserControlFlowNetworkIng>(gid);
				nt->SetStateSync(0);
				nt->SetEnabled(true);
				delete nt->GetGameObject()->FindChild("MatBoll");
				Display::GetCurrent()->SetCamera(nt->GetGameObject());
			}
		}
	}


};

class ClientHAndlers : public NetworkHandlers
{
public:


	virtual void OnLoadLevelFinal() override
	{
		Log("FINISIH HIM!");
		auto x = ts::binary_stream(nullptr, 0, 0);
		Network::GetFromNetworkId<LevelManagent>(2)->call(ts::cmd_hash("@QueryCameraObject"), x);

	}

};


class MyLevel : public LevelBase
{
	Ref<ClientHAndlers> handlers;
public:
	GameObject * cart;
	bool kalel = true;
	DynamicArray<GameObject*> _cars;

	virtual void LoadLevel() override
	{
#ifdef IS_SERVER_BUILD
		Network::Listen(3131);
#endif

		handlers = new ClientHAndlers();
		Network::SetHandlers(handlers.GetPtr());
		Network::RegisterNetworkComponent("NetworkEntityBase", []() { return (Component*)new NetworkEntityBase(); });
		Network::RegisterNetworkComponent("UserControlFlowNetworkIng", []() { return (Component*)new UserControlFlowNetworkIng(); });
		Network::RegisterNetworkComponent("LevelManagent", []() { return (Component*)new LevelManagent(); });
		Network::RegisterNetworkComponent("CarControlFlow", []() { return (Component*)new CarControlFlow(); });


		GameObject* levelmanagment = new GameObject("LevelManager");
		LevelManagent* l = levelmanagment->AddComponent<LevelManagent>();
		l->set_id(2);



		GameObject::SpawnPrefab("/Terrain.prefab", "", Vector3::Zero(), Quaternion::Identity());

	}

	virtual void UnloadLevel() override
	{
	}

	virtual void UpdateLevel() override
	{
		
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

REGISTER_LEVEL(MyLevel)
