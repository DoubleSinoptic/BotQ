#pragma once

#include "Core.h"

class GameObject;

//struct ComponentId 
//{
//	ComponentId(Component* cmp, const char* fx) : 
//		Name(fx) 
//	{}
//	const char* Name;
//};
//
//#define DECL_ID(x) static ComponentId ComponentName = { # x }; 

class SGE_EXPORT Component : public TypedObject
{
	TYPED_OBJECT;
public:
	typedef Component* (*ComponentConstructor)();

	Component();
	virtual ~Component();

	virtual void FrameUpdate()  
	{}

	virtual void PhysicUpdate() 
	{}

	virtual void Awake()
	{}

	virtual void Destroy()
	{}

	REGISTER_PROPETRY(bool, Enabled)
	void SetEnabled(bool value);
	REGISTER_PROPETRY(bool, Deletable)
	void SetDeletable(bool value) { mDeleteble = value; }
	bool GetEnabled() const { return mEnabled; }
	bool GetDeletable() const { return mDeleteble; }
	GameObject* GetGameObject() { return mGameObject; }
	//const ComponentId*		GetComponentId() const { return mId; }
	//void					SetComponentId(const ComponentId* id) { mId = id; }
private:
	//const ComponentId*		 mId;
	ComponentConstructor mConstructor;
	GameObject* mGameObject;
	bool mDeleteble = true;
	bool mEnabled = false;
	friend class GameInstance;
	friend class GameObject;
};