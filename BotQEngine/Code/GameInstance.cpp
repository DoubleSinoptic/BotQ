#include "GameInstance.h"
#include "Display.h"
#include "GameObject.h"
#include "Time.hpp"

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
					lastTimePoint(Time::GetTotalTime()),
					lastRenderTimePoint(Time::GetTotalTime()),
					epsilon(0.0),
					renderEpsilon(0.0),
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
	double updateCallTimePoint = Time::GetTotalTime();
	bool updateFlag = false;

	delta = updateCallTimePoint - lastTimePoint;
	double updatePeriod = 1.0 / tickRate;
	if ((delta + epsilon) >= updatePeriod)
	{
		lastTimePoint = updateCallTimePoint;
		epsilon += (delta - updatePeriod);
		
		/*
		говорим рендерущему потоку о том что он должен обновится
		*/
		physicsSig.Notify();
		/*
		выравниваем дельту чтобы физический движок в один момент 
		не принял за dt всё время которое движок сидел в afk
		*/
		delta = Mathf::Clamp(delta, 0.0f, 10.0);
		
		updateFlag = true;
		SetThisCurrent();

		Internal_SimulatePhysicsContext(physics, delta, -1, -1);
		for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
		{
			Component* c = updatebleComponents[i];
			c->PhysicUpdate();
		}
		
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
	double RenderUpdateStartTimePoint = Time::GetTotalTime();
	bool render_flag = false;

	
	translator.Excecute();
	copyTime = Time::GetTotalTime() - RenderUpdateStartTimePoint;

	renderDelta = Time::GetTotalTime() - lastRenderTimePoint;
	//											   чекаем пришёл ли сигнал
	if ((renderTickRate > 0 && (renderDelta >= (1.0 / renderTickRate))) || physicsSig.Take())
	{
		lastRenderTimePoint = RenderUpdateStartTimePoint;

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
	//bool render_flag = false;

	//double ege = Time::GetTotalTime();
	//translator.ClearAccamulator();
	//copyTime = Time::GetTotalTime() - ege;

	//renderDelta = Time::GetTotalTime() - lastRenderTimePoint;
	////											   чекаем пришёл ли сигнал
	//if ((renderTickRate > 0 && (renderDelta >= (1.0 / renderTickRate))) || physicsSig.Take())
	//{
	//	
	//	lastRenderTimePoint = Time::GetTotalTime();

	//	render_flag = true;
	//	SetThisCurrent();
	//	for (size_t i = 0; i < updatebleComponents.LengthReference; i++)
	//	{
	//		Component* c = updatebleComponents[i];
	//		c->FrameUpdate();
	//	}
	//}
	//return render_flag;
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