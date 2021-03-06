#include <Sge2Common.h>
#include <GameInstance.h>
#include <GameObject.h>
#include <PhysicsComponents/RigidBody.h>
#include <PhysicsComponents/BoxCollider.h>
#include <Time.hpp>
#include <TaskDistributor.h>
#include <Utility.h>
#include <Graphics/Bitmap.h>
#include <Graphics/Bitmap9Path.h>
#include <Input.h>
#include <Display.h>
#include <ResourceLoader.h>
#include <Gl3dDevice.h>
#include <Level.h>
#include "Window.h"
#include <Gui/GuiInstance.h>
#include <Network/Network.h>
#include <Network/NetworkEntityBase.h>
#include <Network/LocalHostCaster.h>
/*
схема многопоточного пуска:
после окончания обновления движка отправляем рендеру сигнал о пуске отрисовки нового
кадра. при этом пока сигнала о отрисовки не приходит выполняем переброску операций в графичесий поток
чтобы физический поток мог создавать в GPU меши и текстуры.
*/

class CameraControlFlow : public Component
{
public:
	CameraControlFlow()
	{
		SetEnabled(true);
	}
	float angleX = 0.0;
	float angleY = 0.0;
	bool hasPressed = true;
	Point curre23nt = Point(0, 0);



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
		if (Input::IsKeyDown(SGE_KEY_D))
		{
			GetGameObject()->SetPosition(GetGameObject()->GetPosition() - GetGameObject()->GetRight()  * renderDelta * thisSpeed * mull);
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


extern "C" void* __cdecl LevelLoaderMain();


/*
GPU->main thread
Phys->child thread
*/

void LoadDefaultPrefabs();
int main()
{

	/*
	Вырубаем сетевой инстансинг. чтобы не жрало при инициализации
	GameInstance::InstindateNetwork(50.0f, false, false);
	GameInstance::SolveProblems(-1);
	*/


	Window wnd;


	{
		Guard g([]()
		{
			Network::UnboundDisconent();
		});

		LocalHostCaster caster;

		GameInstance instnace;
		instnace.renderTickRate = 1;
		instnace.SetThisCurrent();

		
		wnd.AttachKeyCallbacks(&instnace);
		Gl3dDevice::LinkAddresses((Gl3dDeviceLinker)glfwGetProcAddress);


		GameObject* camera = new GameObject("Camera");
		camera->AddComponent<CameraControlFlow>();
		camera->SetTransform(Vector3(3.483976, 27.737846, -9.487122), Quaternion(-0.025084, 0.013085, 0.000328, 0.999600));
		ResourceLoader::LoadStaticAssets("./Data/Static");
		ResourceLoader::LoadStaticAssets("./Static");

		std::atomic_int physRatio = 0;
		std::atomic_int gpuRatio = 0;
		
		SyncObject o;
		std::thread physicsThread([&]()
		{
			ResourceLoader::LoadStaticAssets("./Data");
			ResourceLoader::LoadStaticAssets("./");
			LoadDefaultPrefabs();

			LevelBase* l = (LevelBase*)LevelLoaderMain();
			LevelBase::LoadLevel(l);

	
			/*Ставим тик рейт в ноль чтобы в delt'у не попадал тик
			таймера*/
			instnace.renderTickRate = 0;
			while (!o.SwapSignal())
			{
				/*функции Update и RenderUpdate возвращяет True если
				произошло обновление и false если для обновления ещё не пришло время*/
				if (instnace.Update())
				{
					if(LevelBase::GetCurrent())
						LevelBase::GetCurrent()->UpdateLevel();
					physRatio++;
					Network::ProcessCommands(256);
				}
				else
					std::this_thread::yield();
			}
		});
		// Win32Helper::SwapPriority(SWP_THIS,physicsThread.native_handle());

		double p = Time::GetTotalTime();

		SyncObject additionalCloseSignal;

		SignalAccamulator acamul;

		std::thread([&] 
		{
			while (true)
			{
				std::string ln;
				std::getline(std::cin, ln);
				if (ln == "exit")
					break;
				if (ln == "spawncar")
					acamul.Notify();
			}

			additionalCloseSignal.Notify();
		}).detach();

		while (wnd.Update() && !additionalCloseSignal.SwapSignal())
		{
			if (acamul.Take())
			{
				GameObject* o = Network::Instantiate("/Cars/PickUp.prefab", "CarControlFlow", NetworkEntityBase::AllocateId());
				o->GetComponent<NetworkEntityBase>()->SetStateSync(16);
				o->GetComponent<NetworkEntityBase>()->EnableVelocitySync(true);
			}

			double delta = Time::GetTotalTime() - p;
			if (delta >= 1.0)
			{
				p = Time::GetTotalTime();
				
				long long a, t;
				Gl3dDevice::GetMemory(a, t);
				Log("\n%s [FPS/UPS] fps: %d, ups: %d\n [EXEC] Copy: %f\n [MEM] %ld/%ld Mbytes", "[NULL]", gpuRatio.load(), physRatio.load(), instnace.copyTime, a / 1024, t / 1024);
				gpuRatio = 0;
				physRatio = 0;

				caster.Broadcast(3131, "The simple server", 64, ts::users_count());
			}
			if (instnace.RenderUpdate())
			{

				wnd.SwapBuffers();
				gpuRatio++;

			}
			else
				std::this_thread::yield();

		}

		o.Notify();
		physicsThread.join();


	}



	return 0;
}

