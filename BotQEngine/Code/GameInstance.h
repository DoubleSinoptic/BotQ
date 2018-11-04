#pragma once

#include "Core.h"
#include "Component.h"
#include "Resource.h"
#include "./TaskTranslator.h"
#include "SyncObject.h"
#include "./Audio/AudioInstance.h"
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

	TaskTranslator					translator;
	SignalAccamulator				physicsSig;

	bool hasClosed;

	Display*						display = nullptr;

	double							copyTime;

	double							tickRate;
	double							renderTickRate;

	double							lastTimePoint;
	double							lastRenderTimePoint;

	double							epsilon;
	double							renderEpsilon;

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

