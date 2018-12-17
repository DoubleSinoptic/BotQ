#pragma once

#include "Core.h"
#include "Component.h"
#include "Resource.h"
#include "./CommandQueue.h"
#include "SyncObject.h"
#include "./Audio/AudioInstance.h"
#include "./Time.hpp"
class Display;
class PhysicsInstance;
class MeshRenderer;
class SGE_EXPORT GameInstance
{
public:
	GameInstance();
	~GameInstance();

	AudioInstance audioInstance;
	class PrefabPreference { public: StorageFunction<GameObject*(void)> spawner; String path; };
	PhysicsInstance*				physics = nullptr;

	DynamicArray<PrefabPreference>	prefabs;
	DynamicArray<GameObject*>		sceneobjects;
	DynamicArray<Component*>		updatebleComponents;
	DynamicArray<Resource*>			resources;
	DynamicArray<class Material*>	materials;

	CommandQueue					renderThreadQueue;
	CommandQueue					physicsThreadQueue;
	SignalAccamulator				physicsSig;

	bool hasClosed;

	Display*						display = nullptr;

	double							copyTime;

	double							tickRate;
	double							renderTickRate;


	TimeSpan						lastTimePointTS;
	TimeSpan						lastRenderTimePointTS;

	double							epsilonTS;

	double							delta;
	double							renderDelta;

	void SetThisCurrent();
	static GameInstance* GetCurrent();
    class GameObjectEvents*               objectEventHandler = nullptr;
	bool Update();
	bool RenderUpdate();
	bool FakeRenderUpdate();
	[[deprecated]]
	void PostProcessAndDispatch(double time);
	[[deprecated]]
	void PreProcessDraw();
};

