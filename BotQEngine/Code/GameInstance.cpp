#include "GameInstance.h"
#include "Display.h"
#include "GameObject.h"
#include "Time.hpp"

#include <thread>
#include <mutex>
#include <condition_variable>

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

/*
	����� ���������-���������� ���������. ��� �������� ���������� �� ������� 
	��������� ��������� ������� ��-�� �������������� ������������.
	����� ����� �������� ��� � ������� ���� ������ �������� � ������-��� 
	����� ��������� ����� �����. 

	������ ����� ��������� �� ������ ��������� ������ �������� � 
	��������� ������ �� � ��������� ��������� �������� � �� ���������
	�� ������������� ���������� ��� ��� ������ �������� ��������� ��
	����� ���������� ������������� �� ������� ������������.

	��-�� ���������-���������� ��������� ������� �����
	���������� �� �������� ������������ �����������.
	��� �������� ��� ��� ����������� ������� � 60 ������
	����� �������� ����� 30 ����������� �� ������� ����� �����
	��������� ���������. ��� ������ ��� ���� ��� ������� � 60
	�� ������ ������ ����� ��������� 2 ���������.
*/
class TimeProcessor
{
	bool mFirstFixedFrame = true;

	TimeSpan m_lastUpdateTime;

	TimeSpan mLastFrameTime;
public:
	bool mIsFrameRenderingFinished = true;
	std::mutex  mFrameRenderingFinishedMutex;
	std::condition_variable mFrameRenderingFinishedCondition;

	double FpsLock()
	{		
		if (Time::GetRenderStepSpan() > 0)
		{
			TimeSpan currentTime = Time::GetTotalMicroseconds();
			TimeSpan nextFrameTime = mLastFrameTime + Time::GetRenderStepSpan();
			while (nextFrameTime > currentTime)
			{
				uint32_t waitTime = (uint32_t)(nextFrameTime - currentTime);

				if (waitTime >= 2000)
				{
					std::this_thread::sleep_for(std::chrono::microseconds(waitTime - 2000));
					currentTime = Time::GetTotalMicroseconds();
				}
				else
				{
					while (nextFrameTime > currentTime)
						currentTime = Time::GetTotalMicroseconds();
				}
			}
			double delta =  TimeSpawnToFloatSeconds(currentTime - mLastFrameTime);
			mLastFrameTime = currentTime;
			return delta;
		}
		return 0.0f;
	}

	void GetFixedUpdateStep(TimeSpan& step)
	{
		const TimeSpan currentTime = Time::GetTotalMicroseconds();

		if (mFirstFixedFrame)
		{
			m_lastUpdateTime = currentTime;
			mFirstFixedFrame = false;
		}

		TimeSpan elapsed = currentTime - m_lastUpdateTime;
		m_lastUpdateTime = currentTime;
		step = elapsed;
	}

};

TimeProcessor defaultProcessor;
bool GameInstance::GameLoop()
{
	renderDelta = defaultProcessor.FpsLock();
	{
		TimeSpan step;
		defaultProcessor.GetFixedUpdateStep(step);
		TimeSpan twoSteps = Time::GetRenderStepSpan() * 2;
		int counter = 0;
		while (step > twoSteps)
		{
			printf("proc: %d\n", counter);
			counter++;
			step -= Time::GetRenderStepSpan();
			Update(Time::GetRenderStep());
		}
		Update(TimeSpawnToFloatSeconds(step));
	}

	RenderUpdate(renderDelta);
	renderThreadInstance->Update(this);
	renderThreadInstance->Draw(this);

	{
		std::unique_lock<std::mutex> lock(defaultProcessor.mFrameRenderingFinishedMutex);
		while (!defaultProcessor.mIsFrameRenderingFinished)
			defaultProcessor.mFrameRenderingFinishedCondition.wait(lock);
		defaultProcessor.mIsFrameRenderingFinished = false;
	}

	renderThreadInstance->GetCommandQueue().QueueFunction([&]() 
	{
		std::unique_lock<std::mutex> lock(defaultProcessor.mFrameRenderingFinishedMutex);

		defaultProcessor.mIsFrameRenderingFinished = true;
		defaultProcessor.mFrameRenderingFinishedCondition.notify_one();
	});


	return !renderThreadInstance->GetCloseStatus();
}

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

bool GameInstance::Update(double dt)
{
	SetThisCurrent();
	delta = dt;
	Internal_SimulatePhysicsContext(physics, dt, -1, -1);
	updateQueue.Playback();
	for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
	{
		Component* c = updatebleComponents[i];
		c->PhysicUpdate();
	}
	return 0;
}

/*
����������� ������ �� ���� ������� ������� 
���-��� �� �������� �������� � �� �������� ������� ����
�������
*/
bool GameInstance::RenderUpdate(double dt)
{
	SetThisCurrent();
	renderDelta = dt;
	for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
	{
		Component* c = updatebleComponents[i];
		c->FrameUpdate();
	}
	return 0;
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
