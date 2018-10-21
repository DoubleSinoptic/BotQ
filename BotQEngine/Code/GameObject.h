#pragma once

#include "Core.h"
#include "Component.h"
#include "Event.h"

class RigidBody;
class AdoptObject;

class GameObjectHandlers 
{
public:
	Event<> OnTransformChanged;
	Event<> OnParentChanged;
	Event<> OnNameChanged;
	Event<> OnChildAdded;
	Event<> OnChildRemoved;
};
	
/*
	Базовый игровой обьект, Выполняет 
	все операции 
*/
class GameObject;
typedef GameObject* ObjectPtr;
class SGE_EXPORT GameObject : public TypedObject
{
	TYPED_OBJECT;
public:
	GameObject(const String& name);
	GameObject();
	virtual ~GameObject();

	REGISTER_PROPETRY(String, Name)
	void SetName(const String& name);
	String GetName() const;

	REGISTER_PROPETRY(ObjectPtr, Parent)
	GameObject* GetParent() const;
	void SetParent(GameObject* transform);
	

#ifdef OLD_RIGIDBODY_SYSTEM
	RigidBody* GetRigidBody();
	void SetRigidBody(RigidBody* body);
#endif

	static void Destroy(GameObject* obj);

	String GetPath() const;
	int GetChildsCount() const;
	GameObject* FindChild(const String& name) const;
	GameObject* GetChild(int index) const;
	GameObject** begin();
	GameObject** end();
	GameObject* const* begin() const;
	GameObject* const* end() const;

	void SetClonable(bool value);
	bool IsClonable() const;

	static GameObject* Clone(GameObject* obj);

	
	Component* AddComponent(Component::ComponentConstructor cmp);

	template<typename T>
	T* AddComponent() 
	{
		T* c = new T();
		mComponents.Add(c);
		c->mGameObject = this;
		c->mConstructor = []() -> Component* { return new T(); };
		c->Awake();
		return c;
	}

	template<typename T>
	T* GetComponent() 
	{
		for (Component* i : mComponents)
			if (dynamic_cast<T*>(i) != nullptr)
				return static_cast<T*>(i);
		return nullptr;
	}

	template<typename T>
	DynamicArray<T*> GetComponents() 
	{
		DynamicArray<T*> c;
		for (Component* i : mComponents)
			if (dynamic_cast<T*>(i) != nullptr)
				c.Add(static_cast<T*>(i));
		return c;
	}
	
	const DynamicArray<Component*>& GetAllComponents() const
	{
		return this->mComponents;
	}

	static GameObject* FindObject(const String& name);
	static void ClearScene();
	
	static void SafePrefabsTo(const String& path, bool userResources = false);
	static void LoadPrfabsOf(const String& path, bool userResources = false);
	static void AddPrefab(const String& path, const StorageFunction<GameObject *()> &createor);
	static GameObject* SpawnPrefab(const String &path, const String &name, const Vector3 &position, const Quaternion &rotation);

	void FillMatrix4x4(float* mat) const;

	Vector3 GetLocalPosition() const;
	void SetLocalPosition(const Vector3& position);

	Quaternion GetLocalRotation() const;
	void SetLocalRotation(const Quaternion& rotation);

	REGISTER_PROPETRY(Vector3, Position)
	Vector3 GetPosition() const;
	void SetPosition(const Vector3& position);

	REGISTER_PROPETRY(Quaternion, Rotation)
	Quaternion GetRotation() const;
	void SetRotation(const Quaternion& rotation);


	Vector3 GetForward() const;
	Vector3 GetUp() const;
	Vector3 GetRight() const;

	void SetTransform(const Vector3& position, const Quaternion& rotation);
	
	GameObjectHandlers* GetHandlers() const;
	void OnTransformChanged();
private:

	static GameObject* recurseveClone(GameObject* obj);

#ifdef ENGINE_COMPILLED
	AdoptObject*				mAdoptersBegin;
	AdoptObject*				mAdoptersEnd;
	class FML*					mFML;
	class DigitalInformator     mInformator;
	PredBulldedInfo				mBuildinfo;
#endif

	Vector3						mLocalPos;
	Quaternion					mLocalRot;

	Vector3						mCalculatedPos;
	Quaternion					mCalculatedRot;

	mutable GameObjectHandlers			mHandlersBlock;
	mutable GameObjectHandlers *			mHandlers;

	DynamicArray<Component*>	mComponents;
	DynamicArray<GameObject*>	mChildrens;
	GameObject*					mPar;
	String						mName;
	String						mPath;
	bool						mNoClonable;
#ifdef OLD_RIGIDBODY_SYSTEM
	RigidBody*					mBody;
#endif
};

#ifdef ENGINE_COMPILLED
void operator delete(GameObject* obj) 
{
	obj->~GameObject();
	KhAllignedSector* sector = KhCallculateSector(obj);
	sector->FreeFinal();
}
#endif