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




class TimeProcessor
{
	bool mFirstFixedFrame = true;
	TimeSpan mLastFixedUpdateTime;
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
					std::this_thread::sleep_for(std::chrono::microseconds(waitTime));
					currentTime = Time::GetTotalMicroseconds();
				}
				else
				{
					while (nextFrameTime > currentTime)
						currentTime = Time::GetTotalMicroseconds();
				}
			}
			float delta = currentTime - mLastFrameTime;
			mLastFrameTime = currentTime;
			return delta;
		}
		return 0.0f;
	}

	void AdvanceFixedUpdate(TimeSpan ste)
	{
		mLastFixedUpdateTime += ste;
	}

	int64_t GetFixedUpdateStep(TimeSpan& step)
	{
		const TimeSpan currentTime = Time::GetTotalMicroseconds();

		if (mFirstFixedFrame)
		{
			mLastFixedUpdateTime = currentTime;
			mFirstFixedFrame = false;
		}

		const TimeSpan nextFrameTime = mLastFixedUpdateTime + Time::GetStepSpan();
		if (nextFrameTime <= currentTime)
		{
			TimeSpan simulationAmount = Mathf::Max(currentTime - mLastFixedUpdateTime, Time::GetStepSpan());
			auto numIterations = Mathf::DivideAndRoundUp(simulationAmount, Time::GetStepSpan());

			TimeSpan stepus = Time::GetStepSpan();
			if (numIterations > Time::GetMaxUpdatesPerFrame())
			{
				stepus = Mathf::DivideAndRoundUp(simulationAmount, (TimeSpan)Time::GetMaxUpdatesPerFrame());
				numIterations = Mathf::DivideAndRoundUp(simulationAmount, stepus);
			}
			step = stepus;
			return numIterations;
		}

		step = 0;
		return 0;
	}

};

TimeProcessor defaultProcessor;
bool GameInstance::GameLoop()
{
	renderDelta = defaultProcessor.FpsLock();
	{
		TimeSpan step;
		const int numIterations = defaultProcessor.GetFixedUpdateStep(step);

		delta = step / 1000000.0;
		for (int i = 0; i < numIterations; i++)
		{
			SetThisCurrent();

			Internal_SimulatePhysicsContext(physics, delta, -1, -1);
			for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
			{
				Component* c = updatebleComponents[i];
				c->PhysicUpdate();
				
			}
					
			defaultProcessor.AdvanceFixedUpdate(step);
		}
	}
	
	for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
	{
		Component* c = updatebleComponents[i];
		c->FrameUpdate();
	}
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
Очень важно поинмать что 
даже мелкие погрешности в таймере между быстрыми, по сравнению 
с вызоывми, операциями может существенно повлиять на производительность.
поэтому GetTotalTime вызывается один раз.
Если мы бутем форсить эту функцию то даже за 1 секунду может накопится
значительная погрешность и даже человек сможет заметить странную
медленость симуляции.
*/

bool GameInstance::Update()
{
	throw Exception("not supported exception");
}

/*
рендерящиму потоку не нуен дампфер времени 
так-как мы пытаемся уложится в те тайминги которые задёт
конвеер
*/
bool GameInstance::RenderUpdate()
{
	throw Exception("not supported exception");
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
