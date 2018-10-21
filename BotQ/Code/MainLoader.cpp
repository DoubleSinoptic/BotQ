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
#include "RenderBeast.h"
#include <Network/Network.h>

#undef GetObject
#include <Gui/Box.h>
#include <Gui/ImageCanvas.h>
#include <Gui/Button.h>
#include <Gui/Label.h>
#include <Gui/CheckBox.h>
#include <Gui/HScrollBox.h>
#include <Gui/HSlider.h>
#include <atomic>
#include <fstream>
#include <string>
#include <sstream>
#include <Network/LocalHostCaster.h>
/*
схема многопоточного пуска:
после окончания обновления движка отправляем рендеру сигнал о пуске отрисовки нового
кадра. при этом пока сигнала о отрисовки не приходит выполняем переброску операций в графичесий поток
 чтобы физический поток мог создавать в GPU меши и текстуры.
*/


extern "C" void* __cdecl LevelLoaderMain();
LevelBase* CreateKubizm();

bool NV_CheckPipelineCompression() { return true; }

#undef GetObject
class GuiPipeLine : public GuiInstance
{
	LocalHostCaster caster;
public:
	GuiPipeLine() 		
	{
		auto fontBytes = Resource::Find("/Skin/arial.ttf")->GetObject<Ref<DynamicArray<char>>>();
		new Resource(New<Font>(fontBytes->GetData(), fontBytes->Length(), FontStyle::Base, 24.0f, "arialbd"), "/DefaultFont_24");

	}

	void SetLoadingTitle(const String& srtr) 
	{
		if(loadingText)
			loadingText->SetText(srtr);
	}

	Ref<Label> InsertText(Ref<Control> box) 
	{
		Ref<Label> l = New<Label>();
		Ref<Control> ct = New<Control>();
		ct->AddControl(l);
		l->SetAnchor((Anchor)0);
		box->AddControl(ct);
		return l;
	}

	void loadingBackground() 
	{
		Ref<ImageCanvas> bas = New<ImageCanvas>();
		this->host->AddControl(bas);
		bas->SetPosition({ 0, 0 });
		bas->SetSize({ 1000000, 100000 });
		bas->SetImage(Resource::Find("/Gui/GuiBackground.png")->GetObject<Ref<Texture>>());

	}

	void InstallGui() override 
	{
		useEffectA = true;
		Ref<ImageCanvas> bas = New<ImageCanvas>();
		this->host->AddControl(bas);
		bas->SetPosition({ 0, 0 });
		bas->SetSize({ 1000000, 100000 });
		bas->SetImage(Resource::Find("/Gui/LoadingLevelBackground.png")->GetObject<Ref<Texture>>());

		Ref<ImageCanvas> can = New<ImageCanvas>();
		can->SetPosition({ 10, 10 });
		can->SetSize({ 260 , 55 });
		can->SetImage(Resource::Find("/Gui/LoadingLevelText.png")->GetObject<Ref<Texture>>());

		bas->AddControl(can);		
	}


	String connectIp;
	String tempString;

	SyncObject kubizm;


	SyncObject o;

	void PhysicThread() 
	{
		if (kubizm.SwapSignal())
		{
			pipelineState = 5;
			LevelBase::LoadLevel(CreateKubizm());
		}
		if (o.SwapSignal())
		{
			try 
			{
				Network::Connect(ts::ip_end_point(ts::ip_address::from_string(connectIp.c_str()), 3131));
				LevelBase::LoadLevel((LevelBase*)LevelLoaderMain());
				pipelineState = 5;
			}
			catch (std::exception& ex)
			{
				pipelineState.store(3);
			}
		}
	}

	int yRow = 0;
	Ref<HScrollBox> serverCasters;

	int currentState = -1;
	std::atomic_int pipelineState{0};
	void DrawGpu() override
	{
		LocalHostCaster::CasterHeaderData h;
		ts::ip_address addr;
		if (caster.Uncast(addr, h))
		{
			if (serverCasters)
			{

				Ref<Box> s = New<Box>();
				s->SetSize({ 462, 30 });
				s->SetPosition({ 2, 2 + yRow++ * (30 + 1) });
				
				serverCasters->GetItems()->AddControl(s);

				Ref<Button> a = New<Button>();
				s->AddControl(a);
				a->EnableAutoSize();
				a->SetText("Join");
				a->SetPosition({ 1, 1 });

				a->OnClikedEvent = [=]()
				{
					tempString = String::Format("%s", "Connecting");
					pipelineState = 4;

					std::stringstream s;
					s << addr;
					connectIp = s.str().c_str();

					o.Notify();
				};

				{
					Ref<Label> l = New<Label>();
					s->AddControl(l);
					l->EnableAutoSize();
					l->SetText(String::Format("(%d/%d)", h.loby_aver, h.loby_total));
					l->SetPosition({ 462 - 131, 1 });
				}


				Ref<Label> l = New<Label>();
				s->AddControl(l);
				l->EnableAutoSize();
				l->SetText(String::Format("[%s]", h.server_name));
				l->SetPosition({ 60, 1 });

				Ref<ImageCanvas> can = New<ImageCanvas>();
				can->SetPosition({ 462 - 31, 1 });
				can->SetSize({ 30 , 30 });

				can->SetImage(Resource::Find("/Gui/NetGrad.png")->GetObject<Ref<Texture>>());
				s->AddControl(can);
			}
		}

		GuiInstance::DrawGpu();
		if (currentState != pipelineState.load())
		{
			currentState = pipelineState.load();
			
			if (currentState == 0)
			{
				
			}
			else if (currentState == 1)
			{
				useEffectA = false;
				useEffectA = false;
				MainMenu();
			}
			else if (currentState == 2)
			{
				useEffectA = false;
				useEffectA = false;
				DebugView();
			}
			else if (currentState == 3)
			{
				useEffectA = false;
				useEffectA = false;
				caster.Clear();
				ServerScreen();
			}
			else if (currentState == 4)
			{
				useEffectA = false;
				useEffectA = false;
				ConnectingTo();
			}
			else if (currentState == 5)
			{
				useEffectA = false;
				useEffectA = false;
				GuiInstance::InstallGui();
			}
			else if (currentState == 6)
			{
			}
		}
	}

	void ErrorOf()
	{
		this->host->ClearControls();
		loadingBackground();
		Ref<Box> c = New<Box>();
		c->SetAnchor((Anchor)0);
		c->SetSize({ 200, 100 });
		this->host->AddControl(c);

		loadingText = new Label();

		loadingText->SetSize({ 500, 500 });
		loadingText->SetText(tempString);
		c->AddControl(loadingText);
	}

	void ConnectingTo() 
	{
		this->host->ClearControls();
		loadingBackground();
		Ref<Box> c = New<Box>();
		c->SetAnchor((Anchor)0);
		c->SetSize({ 200, 100 });
		this->host->AddControl(c);

		loadingText = new Label();

		loadingText->SetSize({ 500, 500 });
		loadingText->SetText(tempString);
		c->AddControl(loadingText);
	}

	void ServerScreen()
	{
		this->host->ClearControls();
		loadingBackground();
		Ref<Box> c = New<Box>();
		c->SetAnchor((Anchor)0);
		c->SetSize({ 500, 500 + 25 });
		this->host->AddControl(c);

		
		{
			Ref<Button> a = New<Button>();
			a->SetAnchor(Anchor::Top);

			a->SetPosition({ 300, 492 });
			a->SetSize({ 80, 22 });
			a->SetText(u8"Назад");
			a->OnClikedEvent = [=]()
			{
				pipelineState = 1;
			};
			a->SetAnchor(Anchor::Top);
			c->AddControl(a);
		}

		Ref<HScrollBox> b = New<HScrollBox>();
		b->SetPosition({ 10, 10 });
		b->SetSize({480, 480 });
		c->AddControl(b);
		serverCasters = b;

		std::ifstream f("./servers.txt");
		std::string str;
		yRow = 0;
		while(std::getline(f, str))
		{
			if (str.empty())
				continue;
			Ref<Box> s = New<Box>();
			s->SetSize({462, 30});
			s->SetPosition({2, 2 + yRow++ * (30 + 1)});
			b->GetItems()->AddControl(s);
		
			Ref<Button> a = New<Button>();
			s->AddControl(a);
			a->EnableAutoSize();
			a->SetText("Join");
			a->SetPosition({1, 1});

			a->OnClikedEvent = [=]()
			{			
				tempString = String::Format("%s", "Connecting");
				pipelineState = 4;
				connectIp = str.c_str();
				
				o.Notify();
			};

			
			Ref<Label> l = New<Label>();
			s->AddControl(l);
			l->EnableAutoSize();
			l->SetText(String::Format("[%s]", str.c_str()));
			l->SetPosition({60, 1});

			Ref<ImageCanvas> can = New<ImageCanvas>();
			can->SetPosition({ 462 - 31, 1 });
			can->SetSize({ 30 , 30 });

			can->SetImage(Resource::Find("/Gui/NetError.png")->GetObject<Ref<Texture>>());
			s->AddControl(can);
		}

	}

	Ref<Label> loadingText;
	void LoadLoadingScreen() 
	{
	
		
		
	
	}

	void DebugView() 
	{
		this->host->ClearControls();
		loadingBackground();
		Ref<Box> c = New<Box>();
		c->SetAnchor((Anchor)0);
		c->SetSize({ 200, 300 });
		this->host->AddControl(c);
		{
			Ref<Button> a = New<Button>();
			a->SetAnchor(Anchor::Top);

			a->SetPosition({ 10, 10 });
			a->SetSize({ 80, 22 });
			a->SetText(u8"[NULL]");
			c->AddControl(a);
		}
		{
			Ref<Button> a = New<Button>();
			a->SetAnchor(Anchor::Top);

			a->SetPosition({ 10, 42 });
			a->SetSize({ 80, 22 });
			a->SetText(u8"Кубизм");
			a->OnClikedEvent = [&]()
			{
				kubizm.Notify();
			};
			c->AddControl(a);
		}
		{
			Ref<Button> a = New<Button>();
			a->SetAnchor(Anchor::Top);

			a->SetPosition({ 10, 42 + 32 });
			a->SetSize({ 80, 22 });
			a->SetText(u8"Назад");
			a->OnClikedEvent = [=]()
			{
				pipelineState = 1;
			};
			c->AddControl(a);
		}
	
	}

	void MainMenu() 
	{
		this->host->ClearControls();
		loadingBackground();
		Ref<Box> c = New<Box>();
		c->SetAnchor((Anchor)0);
		c->SetSize({ 200, 300 });
		this->host->AddControl(c);	
		{
			Ref<Button> a = New<Button>();
			a->SetAnchor(Anchor::Top);
			
			a->SetPosition({ 10, 10 });
			a->SetSize({ 80, 22 });
			a->SetText(u8"Мир отладка");
			a->OnClikedEvent = [=]() 
			{
				pipelineState = 2; 
			};
			c->AddControl(a);
		}
		{
			Ref<Button> a = New<Button>();
			a->SetAnchor(Anchor::Top);
			
			a->SetPosition({ 10, 42 });
			a->SetSize({ 80, 22 });
			a->SetText(u8"Сетевая игра");
			a->OnClikedEvent = [=]()
			{
				pipelineState = 3;
			};
			c->AddControl(a);
		}
		{
			Ref<Button> a = New<Button>();
			a->SetAnchor(Anchor::Top);
	
			a->SetPosition({ 10, 42 + 32 });
			a->SetSize({ 80, 22 });
			a->SetText(u8"Выход");
			a->OnClikedEvent = []()
			{
				Resource* r = Resource::Find("/Functions/CloseGame", false);
				if (r) r->GetObject<StorageFunction<void(void)>>()();
			};
			c->AddControl(a);
		}
	}

};

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

	/*if(NV_CheckPipelineCompression())
		MessageBox(nullptr, TEXT("Ошибка видео ускорителя. Ваша видеокарта агресивно использует\npipeline сжатие. это может вызывать падение производительности. Если хотите избежать данной проблему запустите игру с правами администратора"), nullptr, MB_ICONWARNING);
	*/
	Window wnd;
	

	{
		Guard g([]()
		{
			Network::UnboundDisconent();
		});
		GameInstance instnace;
		instnace.renderTickRate = 24;
		instnace.SetThisCurrent();
		

		wnd.AttachKeyCallbacks(&instnace);
		Gl3dDevice::LinkAddresses((Gl3dDeviceLinker)glfwGetProcAddress);
		

		GameObject* camera = new GameObject("Camera");

		camera->SetTransform(Vector3(3.483976, 27.737846, -9.487122), Quaternion(-0.025084, 0.013085, 0.000328, 0.999600));
		ResourceLoader::LoadStaticAssets("./Data/Static");
		ResourceLoader::LoadStaticAssets("./Static");

		GuiPipeLine guiInstance;

		Display::GetCurrent()->SetCamera(camera);
		RenderBeast beast;

		std::atomic_int physRatio{0};
		std::atomic_int gpuRatio{0};

		SyncObject o;
		std::thread physicsThread([&]()
		{			
			
			ResourceLoader::LoadStaticAssets("./Data");			
			ResourceLoader::LoadStaticAssets("./");
			LoadDefaultPrefabs();
		
			guiInstance.pipelineState.store(1);
			/*Ставим тик рейт в ноль чтобы в delt'у не попадал тик
			таймера*/
			instnace.renderTickRate = 0;
			while (!o.SwapSignal())
			{
				/*функции Update и RenderUpdate возвращяет True если 
				произошло обновление и false если для обновления ещё не пришло время*/
				if (instnace.Update())
				{
					guiInstance.PhysicThread();
					if (LevelBase::GetCurrent())
						LevelBase::GetCurrent()->UpdateLevel();
					physRatio++;
					Network::ProcessCommands(64);
				}
				else
					std::this_thread::yield();
			}
		});
		// Win32Helper::SwapPriority(SWP_THIS,physicsThread.native_handle());

		double p = Time::GetTotalTime();

		SyncObject additionalCloseSignal;

		new Resource(StorageFunction<void(void)>([&]() { additionalCloseSignal.Notify(); }), "/Functions/CloseGame");

	/*	new Resource(StorageFunction<void(void)>([&]() { beast.OutPostProcTexel = true; }), "/Functions/EnableOutPostProcTexel");
		new Resource(StorageFunction<void(void)>([&]() { beast.OutChromaticAberationTexel = true; }), "/Functions/EnableChromaticAberationTexel");
		new Resource(StorageFunction<void(void)>([&]() { beast.OutPostProcTexel = true; }), "/Functions/EnablePostProcTexel");

		new Resource(StorageFunction<void(void)>([&]() { beast.OutPostProcTexel = false; }), "/Functions/DisableOutPostProcTexel");
		new Resource(StorageFunction<void(void)>([&]() { beast.OutChromaticAberationTexel = false; }), "/Functions/DisableChromaticAberationTexel");
		new Resource(StorageFunction<void(void)>([&]() { beast.OutPostProcTexel = false; }), "/Functions/DisablePostProcTexel");*/

		double renderTime;
		double guiTime;
		double swapTime;
		double flushTime;
		double updtTime;

		while (wnd.Update() && !additionalCloseSignal.SwapSignal())
		{
			double delta = Time::GetTotalTime() - p;
			if (delta >= 1.0)
			{
				p = Time::GetTotalTime();
				system("cls");
				long long a, t;
				Gl3dDevice::GetMemory(a, t);
				Log("\n%s [FPS/UPS] fps: %d, ups: %d\n [EXEC] Copy: %f\n [RT] R: %f G: %f S: %f F: %f U: %f\n [MEM] %ld/%ld Mbytes", *beast.profileString, gpuRatio.load(), physRatio.load(),
					instnace.copyTime, renderTime, guiTime, swapTime, flushTime, updtTime, a / 1024, t / 1024);
				gpuRatio = 0;
				physRatio = 0;

			}

			double tsm = Time::GetTotalTime();
			if (instnace.RenderUpdate())
			{
				updtTime = Time::GetTotalTime() - tsm;
				tsm = Time::GetTotalTime();
				beast.Draw();	
				renderTime = Time::GetTotalTime() - tsm;
				tsm = Time::GetTotalTime();
				guiInstance.DrawGpu();
				guiTime = Time::GetTotalTime() - tsm;
				tsm = Time::GetTotalTime();
				Gl3dDevice::ExecCommands();
				flushTime = Time::GetTotalTime() - tsm;
				tsm = Time::GetTotalTime();
				wnd.SwapBuffers();
				swapTime = Time::GetTotalTime() - tsm;
				tsm = Time::GetTotalTime();
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

