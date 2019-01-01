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

class IRenderThreadInstance
{
public:
	virtual ~IRenderThreadInstance() {}
	virtual CommandQueue& GetCommandQueue() = 0;
	virtual bool GetCloseStatus() = 0;
	virtual void Start(GameInstance* instance) = 0;
	virtual void Stop(GameInstance* instance) = 0;
	virtual void Update(GameInstance* instance) = 0;
	virtual void Draw(GameInstance* instance) = 0;


};

class SGE_EXPORT GameInstance
{
public:
	GameInstance(IRenderThreadInstance* threadInstance);
	~GameInstance();

	AudioInstance audioInstance;
	class PrefabPreference { public: StorageFunction<GameObject*(void)> spawner; String path; };
	PhysicsInstance*				physics = nullptr;

	IRenderThreadInstance*			renderThreadInstance;
	DynamicArray<PrefabPreference>	prefabs;
	DynamicArray<GameObject*>		sceneobjects;
	DynamicArray<Component*>		updatebleComponents;
	DynamicArray<Resource*>			resources;
	DynamicArray<class Material*>	materials;

	CommandQueue&					renderQueue;
	CommandQueue					updateQueue;

	bool hasClosed;

	Display*						display = nullptr;

	double							copyTime;

	double							tickRate;
	double							renderTickRate;
	
	double							delta;
	double							renderDelta;

	bool GameLoop();

	void SetThisCurrent();
	static GameInstance* GetCurrent();
    class GameObjectEvents*               objectEventHandler = nullptr;
	bool Update();
	bool RenderUpdate();
	bool FakeRenderUpdate();
};

#define renderThreadQueue GameInstance::GetCurrent()->renderQueue
#define updateQueue GameInstance::GetCurrent()->updateQueue
#define currentDisplay GameInstance::GetCurrent()->display