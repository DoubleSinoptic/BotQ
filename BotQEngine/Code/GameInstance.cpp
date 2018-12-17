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

GameInstance::GameInstance() : 
					tickRate(60.0),
					renderTickRate(0.0),
					lastTimePointTS(Time::GetTotalMicroseconds()),
					lastRenderTimePointTS(Time::GetTotalMicroseconds()),
					epsilonTS(0),
					delta(0.0),
					renderDelta(0.0),

					hasClosed(false),
					physics(Internal_AllocateAndConstructPhysicsContext()),
					display(new Display())
{

	SetThisCurrent();
	prefabs.EnsureCapacity(10);
	Log("GameInstance instance is inited: force size is: %zu", sizeof(GameInstance));

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
	//PERFORM("GameInstance::Update");
	physicsThreadQueue.Excecute();

	TimeSpan currentTime = Time::GetTotalMicroseconds();
	bool updateFlag = false;
	TimeSpan frameStep = TimeSpamFromSeconds(1.0 / tickRate);
	TimeSpan deltaTime = currentTime - lastTimePointTS;
	if ((deltaTime + epsilonTS) >= frameStep)
	{
			lastTimePointTS = currentTime;
	        epsilonTS += (deltaTime - frameStep);
			
			updateFlag = true;
			SetThisCurrent();
			
		    Internal_SimulatePhysicsContext(physics, TimeSpawnToFloatSeconds(deltaTime), -1, -1);
			for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
			{
				Component* c = updatebleComponents[i];
				c->PhysicUpdate();
			}

			physicsSig.Notify();
	}
	return updateFlag;
}

/*
рендерящиму потоку не нуен дампфер времени 
так-как мы пытаемся уложится в те тайминги которые задёт
конвеер
*/
bool GameInstance::RenderUpdate()
{
	//PERFORM("GameInstance::RenderUpdate");
	renderThreadQueue.Excecute();

	TimeSpan currentTime = Time::GetTotalMicroseconds();
	bool render_flag = false;
	TimeSpan frameStep = TimeSpamFromSeconds(1.0 / renderTickRate);
	TimeSpan deltaTime = currentTime - lastRenderTimePointTS;
	if ((renderTickRate > 0 && deltaTime >= frameStep) || physicsSig.Take())
	{
		lastRenderTimePointTS = currentTime;
		render_flag = true;
		SetThisCurrent();
		for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
		{
			Component* c = updatebleComponents[i];
			c->FrameUpdate();
		}
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
	Log("All deleted instance is inited: force size is: %zu", sizeof(GameInstance));
}

void GameInstance::PostProcessAndDispatch(double time)
{
	delta = time;
	

	SetThisCurrent();
	Internal_SimulatePhysicsContext(physics, time, -1, -1);
	size_t ln = updatebleComponents.Length();
	for (size_t i = 0; i < ln; i++)
	{
		Component* c = updatebleComponents[i];
		c->PhysicUpdate();
	}

}

void GameInstance::PreProcessDraw()
{
	SetThisCurrent();
	size_t ln = updatebleComponents.Length();
	for (size_t i = 0; i < ln; i++)
	{
		Component* c = updatebleComponents[i];
		c->FrameUpdate();
	}
}