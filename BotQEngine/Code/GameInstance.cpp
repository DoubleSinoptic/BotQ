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
	—хема дискретно-стабильной симул€ции. ћне пришлось отказатьс€ от системы 
	симул€ции реального времени из-за нестабильности дисретизации.
	ќчень важно понимать что в системе есть другие процессы и просто-так 
	гредь процессор очень плохо. 

	ƒанна€ схема позвол€ет не только упростить работу таймеров и 
	симул€ции физики но и позвол€ет стабильно работать с не линейными
	от дискретизации процессами что даЄт больее топорную симул€цию но
	очень правильную интерпритацию со стороны программиста.

	из-за дискретно-стабильной симул€ции секунда будет
	разроблена на половину дискретности блокровщика.
	это означает что при номинальной частоте в 60 кадров
	будет выдленно всего 30 промежутков на которых нужно будет
	выполнить симул€цию. это значит что если дан тикрейт в 60
	за каждый перьюд нужно выполн€ть 2 обработки.
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
		m_lastUpdateTime += Time::GetStepSpan();
	}

	int64_t GetFixedUpdateStep(TimeSpan& step)
	{
		const TimeSpan currentTime = Time::GetTotalMicroseconds();

		if (mFirstFixedFrame)
		{
			m_lastUpdateTime = currentTime;
			mFirstFixedFrame = false;
		}

		TimeSpan elapsed = currentTime - m_lastUpdateTime;

		if (elapsed > Time::GetStepSpan())
		{
			step = Time::GetStepSpan();
			return elapsed / Time::GetStepSpan();
		}
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
ќчень важно поинмать что 
даже мелкие погрешности в таймере между быстрыми, по сравнению 
с вызоывми, операци€ми может существенно повли€ть на производительность.
поэтому GetTotalTime вызываетс€ один раз.
≈сли мы бутем форсить эту функцию то даже за 1 секунду может накопитс€
значительна€ погрешность и даже человек сможет заметить странную
медленость симул€ции.
*/

bool GameInstance::Update()
{
	throw Exception("not supported exception");
}

/*
рендер€щиму потоку не нуен дампфер времени 
так-как мы пытаемс€ уложитс€ в те тайминги которые задЄт
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
