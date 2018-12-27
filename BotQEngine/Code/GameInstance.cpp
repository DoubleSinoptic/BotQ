#include "GameInstance.h"
#include "Display.h"
#include "GameObject.h"
#include "Time.hpp"


#define PERFORM(x) {printf("%s\n",(x));}

PhysicsInstance*	Internal_AllocateAndConstructPhysicsContext();
void				Internal_DestructAndDeallocatePhysicsContext(PhysicsInstance* c);
void				Internal_SimulatePhysicsContext(PhysicsInstance* c, float time, int substepscount, float substeptime);

/*
int framesPerSeconds;
int updatePerSeconds;
double							renderDelta;
double							delta;
double							fullTime;
*/

GameInstance::GameInstance(IRenderThreadInstance* threadInstance) :
					tickRate(60.0),
					renderTickRate(0.0),
					lastTimePointTS(Time::GetTotalMicroseconds()),
					lastRenderTimePointTS(Time::GetTotalMicroseconds()),
					epsilonTS(0),
					delta(0.0),
					renderDelta(0.0),
					renderThreadInstance(threadInstance),
					renderQueue(threadInstance->GetCommandQueue()),
					hasClosed(false),
					physics(Internal_AllocateAndConstructPhysicsContext()),
					display(new Display())
{

	SetThisCurrent();
	prefabs.EnsureCapacity(10);
	Log("GameInstance instance is inited: force size is: %zu", sizeof(GameInstance));
	threadInstance->Start(this);
}

GameInstance* currentGameInstance = nullptr;

void GameInstance::SetThisCurrent()
{
	currentGameInstance = this;
	audioInstance.MakeCurrent();
}

GameInstance * GameInstance::GetCurrent()
{
	return currentGameInstance;
}

/*
����� ����� �������� ��� 
���� ������ ����������� � ������� ����� ��������, �� ��������� 
� ��������, ���������� ����� ����������� �������� �� ������������������.
������� GetTotalTime ���������� ���� ���.
���� �� ����� ������� ��� ������� �� ���� �� 1 ������� ����� ���������
������������ ����������� � ���� ������� ������ �������� ��������
���������� ���������.
*/
static TimeSpan largeSpan = TimeSpamFromSeconds(2);
bool GameInstance::Update()
{
	TimeSpan currentTime = Time::GetTotalMicroseconds();
	bool updateFlag = false;
	TimeSpan frameStep = TimeSpamFromSeconds(1.0 / tickRate);
	TimeSpan deltaTime = currentTime - lastTimePointTS;
	if ((deltaTime + epsilonTS) >= frameStep)
	{
		delta = TimeSpawnToFloatSeconds(deltaTime);
		lastTimePointTS = currentTime;
	    epsilonTS += (deltaTime - frameStep);
		if (epsilonTS > largeSpan)
			epsilonTS = 0;
		updateFlag = true;
		SetThisCurrent();
			
		Internal_SimulatePhysicsContext(physics, TimeSpawnToFloatSeconds(deltaTime), -1, -1);
		for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
		{
			Component* c = updatebleComponents[i];
			c->PhysicUpdate();
		}

		updateQueue.Playback();
		renderThreadInstance->Update(this);
	}
	return updateFlag;
}

/*
����������� ������ �� ���� ������� ������� 
���-��� �� �������� �������� � �� �������� ������� ����
�������
*/
bool GameInstance::RenderUpdate()
{
	TimeSpan currentTime = Time::GetTotalMicroseconds();
	bool render_flag = false;
	TimeSpan frameStep = TimeSpamFromSeconds(1.0 / renderTickRate);
	TimeSpan deltaTime = currentTime - lastRenderTimePointTS;
	if ((renderTickRate > 0 && deltaTime >= frameStep))
	{
		renderDelta = TimeSpawnToFloatSeconds(deltaTime);
		lastRenderTimePointTS = currentTime;
		render_flag = true;
		SetThisCurrent();
		for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
		{
			Component* c = updatebleComponents[i];
			c->FrameUpdate();
		}
		renderThreadInstance->Draw(this);
	}
	return render_flag;
}

bool GameInstance::FakeRenderUpdate()
{
	throw Exception("not supported exception");
}

GameInstance::~GameInstance()
{
	SetThisCurrent();

	GameObject::ClearScene();
	Resource::FreeAllResources();

	if (currentGameInstance == this)
		currentGameInstance = nullptr;
	if (physics)
		Internal_DestructAndDeallocatePhysicsContext(physics);
	if (display)
		delete display;
	if (renderThreadInstance)
		renderThreadInstance->Stop(this);
	Log("All deleted instance is inited: force size is: %zu", sizeof(GameInstance));
}
