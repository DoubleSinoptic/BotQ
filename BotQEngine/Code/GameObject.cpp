#include "GameObject.h"
#include "GameInstance.h"
#include "PhysicsComponents/RigidBody.h"
#include "GameObjectEvents.h"
#include "./Bits/TiXml/tinyxml.h"
#include <stdlib.h>

#define TRANSLATE_GAME_OBJECT_EVENTS

#define CALL_HANDLER_OF(t, x) \
	if((t)->mHandlers) \
		(t)->mHandlers-> x .Emit();

#define CALL_HANDLER(x) \
	if(this->mHandlers) \
			(this)->mHandlers-> x .Emit();
//-----------------------------------------------------------

GameObject* GameObject::recurseveClone(GameObject* obj)
{
	Assert(obj != nullptr);
	GameObject* clone = new GameObject();
	clone->SetName(obj->GetName());
	clone->SetLocalPosition(obj->GetLocalPosition());
	clone->SetLocalRotation(obj->GetLocalRotation());
	for (GameObject* objChild : *obj)
	{
		if (objChild->IsClonable())
		{
			GameObject* clonedChild = recurseveClone(objChild);
			clonedChild->SetParent(clone);
		}		
	}

	const DynamicArray<Component*>& objComps = obj->GetAllComponents();
	for (Component* objOnceComponent : objComps) 
	{
		Component* clonedCompoent = objOnceComponent->mConstructor();
		clonedCompoent->mConstructor = objOnceComponent->mConstructor;
		clonedCompoent->mGameObject = clone;
		clonedCompoent->Awake();
		clone->mComponents.Add(clonedCompoent);

		objOnceComponent->CloneTo(clonedCompoent);
		ReferencableProxy* beg = objOnceComponent->BeginProxy();
		while (beg != nullptr)
		{
			beg->ExportTo(clonedCompoent->FindPropetry(beg->name));
			beg = beg->last;
		}	
	}


	return clone;
}

//------------------------------------------------------------

void GameObject::SetClonable(bool value) 
{
	this->mNoClonable = value;
}

bool GameObject::IsClonable() const 
{
	return mNoClonable;
}

GameObject* GameObject::Clone(GameObject* obj) 
{
	Assert(obj != nullptr);
	return recurseveClone(obj);
}

//------------------------------------------------------------

Component * GameObject::AddComponent(Component::ComponentConstructor cmp)
{
	Component* cm = cmp();
	cm->mConstructor = cmp;
	mComponents.Add(cm);
	cm->mGameObject = this;
	cm->Awake();
	return cm;
}

//------------------------------------------------------------

GameObject::GameObject(const String & name)
	: mName(name), mHandlers(nullptr), mNoClonable(true),
#ifdef OLD_RIGIDBODY_SYSTEM
	mBody(nullptr),
#endif
	mPar(nullptr),
	mLocalRot(Quaternion::Identity()), mCalculatedRot(Quaternion::Identity()),
	mLocalPos(Vector3::Zero()), mCalculatedPos(Vector3::Zero())
{
	GameInstance::GetCurrent()->sceneobjects.Add(this);
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
    if(GameInstance::GetCurrent()->objectEventHandler)
        GameInstance::GetCurrent()->objectEventHandler->CreateObject(this);
#endif
}

//------------------------------------------------------------

GameObject::GameObject()
	: mName("GameObject"), mHandlers(nullptr), mNoClonable(true),
#ifdef OLD_RIGIDBODY_SYSTEM
	mBody(nullptr), 
#endif
	mPar(nullptr),
	mLocalRot(Quaternion::Identity()), mCalculatedRot(Quaternion::Identity()),
	mLocalPos(Vector3::Zero()), mCalculatedPos(Vector3::Zero())
{
	GameInstance::GetCurrent()->sceneobjects.Add(this);
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
    if(GameInstance::GetCurrent()->objectEventHandler)
        GameInstance::GetCurrent()->objectEventHandler->CreateObject(this);
#endif
}

//------------------------------------------------------------

GameObject::~GameObject()
{

	if (mPar != nullptr)
	{
		mPar->mChildrens.Remove(this);
		CALL_HANDLER_OF(mPar, OnChildRemoved);
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
        if(GameInstance::GetCurrent()->objectEventHandler)
            GameInstance::GetCurrent()->objectEventHandler->ObjectChildRemoved(mPar, this);
#endif
		mPar = nullptr;
	}
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
    if(GameInstance::GetCurrent()->objectEventHandler)
        GameInstance::GetCurrent()->objectEventHandler->DisposeObject(this);
#endif
	for (size_t i = 0; i < mComponents.Length(); i++) 
	{
		Component* b = mComponents[i];
		b->Destroy();
		if (b->GetDeletable())
			delete b;
	}

	while (mChildrens.Length() != 0)
		delete mChildrens.Back();

	GameInstance::GetCurrent()->sceneobjects.Remove(this);
}

//------------------------------------------------------------

void GameObject::SetName(const String & name)
{
	mName = name;
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
    if(GameInstance::GetCurrent()->objectEventHandler)
        GameInstance::GetCurrent()->objectEventHandler->ObjectNameChanged(this);
#endif
}

//------------------------------------------------------------

String GameObject::GetName() const
{
	return mName;
}

void GameObject::Destroy(GameObject * obj)
{
	delete obj;
}

String GameObject::GetPath() const
{
	return mPath;
}

//------------------------------------------------------------
#ifdef OLD_RIGIDBODY_SYSTEM
RigidBody* GameObject::GetRigidBody()
{
	return mBody;
}

//------------------------------------------------------------

void GameObject::SetRigidBody(RigidBody * body)
{
	mBody = body;
}
#endif
//------------------------------------------------------------

void GameObject::ClearScene()
{
	DynamicArray<GameObject*>& o = GameInstance::GetCurrent()->sceneobjects;
	while(o.LengthReference)
		for (int i = 0; i < o.LengthReference; i++) 
		{
			if (o[i]->GetParent() == nullptr)
				delete o[i];
		}
}

//------------------------------------------------------------

void GameObject::AddPrefab(const String& path, const StorageFunction<GameObject *()> &createor)
{
	GameInstance::GetCurrent()->prefabs.Add({ createor, path });
}

GameObject* GameObject::SpawnPrefab(const String &path, const String &name, const Vector3 &position, const Quaternion &rotation)
{
	int id = -1;
	for (int i = 0; i < GameInstance::GetCurrent()->prefabs.Length(); i++)
	{
		if (GameInstance::GetCurrent()->prefabs[i].path == path)
			id = i;
	}
	if (id < 0)
		return nullptr;
	
	GameObject* obj = GameInstance::GetCurrent()->prefabs[id].spawner();
	obj->SetPosition(position);
	obj->SetRotation(rotation);
	obj->SetName(name);
	obj->mPath = path;
	return obj;
}

//---------------------------------------------------------------

GameObject* GameObject::GetParent() const
{
	return mPar;
}

//---------------------------------------------------------------

void GameObject::SetParent(GameObject* par)
{
	if (par == this)
		Error("ivalid gameobject logic");
	if (mPar != nullptr)
	{
		mPar->mChildrens.Remove(this);
		CALL_HANDLER_OF(mPar, OnChildRemoved);
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
        if(GameInstance::GetCurrent()->objectEventHandler)
            GameInstance::GetCurrent()->objectEventHandler->ObjectChildRemoved(mPar, this);
#endif
		mPar = nullptr;
	}
	if (par == nullptr)
	{
		mPar = nullptr;
	}		
	else
	{
		mPar = par;
		mPar->mChildrens.Add(this);
		CALL_HANDLER_OF(mPar, OnChildAdded);
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
        if(GameInstance::GetCurrent()->objectEventHandler)
            GameInstance::GetCurrent()->objectEventHandler->ObjectChildAdded(mPar, this);
#endif
	}
	CALL_HANDLER(OnParentChanged);
#ifdef TRANSLATE_GAME_OBJECT_EVENTS
    if(GameInstance::GetCurrent()->objectEventHandler)
        GameInstance::GetCurrent()->objectEventHandler->ObjectParentChanged(this);
#endif
	this->OnTransformChanged();
}

//---------------------------------------------------------------

GameObject * GameObject::FindObject(const String & name)
{
	for (GameObject* i : GameInstance::GetCurrent()->sceneobjects) 
	{
		if (i->GetName() == name)
			return i;
	}
	return nullptr;
}

//---------------------------------------------------------------

int GameObject::GetChildsCount() const
{
	return int(mChildrens.Length());
}

//---------------------------------------------------------------

GameObject* GameObject::FindChild(const String& name) const
{
	for (int i = 0; i < mChildrens.Length(); i++)
		if (mChildrens[i]->GetName() == name)
			return mChildrens[i];		
	
	for (int i = 0; i < mChildrens.Length(); i++)
	{
		GameObject* tr = mChildrens[i]->FindChild(name);
		if (tr)
			return tr;
	}
	return nullptr;
}

//---------------------------------------------------------------

void GameObject::FillMatrix4x4(float * mat) const
{
	Quaternion q = mCalculatedRot;
	Vector3 p = mCalculatedPos;

	float d = q.Length2();
	float s = float(2.0) / d;
	float xs = q.x * s, ys = q.y * s, zs = q.z * s;
	float wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
	float xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
	float yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;

	mat[0] = float(1.0f) - (yy + zz);
	mat[1] = xy + wz;
	mat[2] = xz - wy;
	mat[3] = 0.0f;

	mat[4] = xy - wz;
	mat[5] = float(1.0f) - (xx + zz);
	mat[6] = yz + wx;
	mat[7] = 0.0f;

	mat[8] = xz + wy;
	mat[9] = yz - wx;
	mat[10] = float(1.0f) - (xx + yy);
	mat[11] = 0.0f;

	mat[12] = p.x;
	mat[13] = p.y;
	mat[14] = p.z;
	mat[15] = 1.0f;
}

//---------------------------------------------------------------

GameObject* GameObject::GetChild(int index) const {
	return mChildrens[index];
}

//---------------------------------------------------------------

GameObject** GameObject::begin()
{
	return mChildrens.begin();
}

//---------------------------------------------------------------

GameObject** GameObject::end()
{
	return mChildrens.end();
}

//---------------------------------------------------------------

GameObject* const *  GameObject::begin() const
{
	return mChildrens.begin();
}

//---------------------------------------------------------------

GameObject* const *  GameObject::end() const
{
	return mChildrens.end();
}

//---------------------------------------------------------------

//void GameObject::UpdateGraphicsTransform()
//{
//	if (mPar != nullptr)
//	{
//		mRezPos = (mPar->mRezRot * mLocalPos) + mPar->mRezPos;
//		mRezRot = (mPar->mRezRot * mLocalRot);
//	}
//	else
//	{
//		mRezRot = mLocalRot;
//		mRezPos = mLocalPos;
//	}
//
//	for (int i = 0; i < mChildrens.Length(); i++)
//	{
//		GameObject* tr = mChildrens[i];
//		tr->UpdateGraphicsTransform();
//	}
//}

//---------------------------------------------------------------
//Vector3 GameObject::GetPosition() const
//{
//	if (mPar != nullptr)
//		return (mPar->GetRotation() * mLocalPos) + mPar->GetPosition();
//	else
//		return mLocalPos;
//}
//
//Quaternion GameObject::GetRotation() const
//{
//	if (mPar != nullptr)
//	{
//		return  mPar->GetRotation() * mLocalRot;
//	}
//	else
//		return mLocalRot;
//}
//---------------------------------------------------------------

Vector3 GameObject::GetLocalPosition() const
{
	return mLocalPos;
}
//---------------------------------------------------------------

Quaternion GameObject::GetRotation() const
{
	return mCalculatedRot;
}

//---------------------------------------------------------------

Vector3 GameObject::GetPosition() const
{
	return mCalculatedPos;
}

//---------------------------------------------------------------

Quaternion GameObject::GetLocalRotation() const
{
	return mLocalRot;
}

//---------------------------------------------------------------

void GameObject::OnTransformChanged()
{
	if (mPar != nullptr)
	{
		mCalculatedPos = mPar->GetRotation() * mLocalPos + mPar->GetPosition();
		mCalculatedRot = mPar->GetRotation() * mLocalRot;
	}
	else
	{
		mCalculatedPos = mLocalPos;
		mCalculatedRot = mLocalRot;
	}
#ifdef OLD_RIGIDBODY_SYSTEM
	if (GetRigidBody() != nullptr)
		GetRigidBody()->ApplayTransform(this);
#endif
	for (size_t i = 0; i < mChildrens.Length(); i++)
	{
		mChildrens[i]->OnTransformChanged();
	}

	CALL_HANDLER(OnTransformChanged);
}

//---------------------------------------------------------------

void GameObject::SetLocalPosition(const Vector3 &position)
{
	mLocalPos = position;
	this->OnTransformChanged();
}

//---------------------------------------------------------------

void GameObject::SetLocalRotation(const Quaternion &rotation)
{
	mLocalRot = rotation;
	this->OnTransformChanged();
}

//---------------------------------------------------------------

void GameObject::SetPosition(const Vector3 &position)
{
	if (mPar != nullptr)
		mLocalPos = mPar->GetRotation().Inverse() * (position - mPar->GetPosition());
	else
		mLocalPos = position;
	this->OnTransformChanged();
}


//---------------------------------------------------------------

void GameObject::SetRotation(const Quaternion &rotation)
{
	if (mPar != nullptr)
		mLocalRot = (mPar->GetRotation() * rotation).Inverse();
	else
		mLocalRot = rotation;
	this->OnTransformChanged();
}

//---------------------------------------------------------------

void GameObject::SetTransform(const Vector3& position, const Quaternion& rotation)
{
	if (mPar != nullptr)
	{
		Quaternion q = mPar->GetRotation().Inverse();
		mLocalPos = (q * (position - mPar->GetPosition()));
		mLocalRot = (q * rotation);
	}
	else
	{
		mLocalPos = position;
		mLocalRot = rotation;
	}
	this->OnTransformChanged();
}

//---------------------------------------------------------------

GameObjectHandlers * GameObject::GetHandlers() const
{
	if (!mHandlers)
		mHandlers = &mHandlersBlock;
	return mHandlers;
}

//---------------------------------------------------------------

Vector3 GameObject::GetForward() const
{
	return GetRotation() * Vector3(0, 0, 1);
}

//---------------------------------------------------------------

Vector3 GameObject::GetUp() const
{
	return GetRotation() * Vector3(0, 1, 0);
}

//---------------------------------------------------------------

Vector3 GameObject::GetRight() const
{
	return GetRotation() * Vector3(1, 0, 0);
}

//---------------------------------------------------------------

void SaveObject(GameObject* xls, TiXmlElement& root)
{
	if (xls->GetPath() != "")
	{
		TiXmlElement elem("Prefab");
		elem.SetAttribute("path", xls->GetPath().c_str());
		Vector3 p = xls->GetPosition();
		Quaternion q = xls->GetRotation();
		elem.SetAttribute("localRotation", String::Format("%f:%f:%f:%f", q.x, q.y, q.z, q.w).c_str());
		elem.SetAttribute("localPosition", String::Format("%f:%f:%f", p.x, p.y, p.z).c_str());

		root.InsertEndChild(elem);
	}

}

void GameObject::SafePrefabsTo(const String& path, bool useR) 
{
	TiXmlDocument doc("SceneSpawns");
	TiXmlElement mainElem("Scene");
	for (auto& x : GameInstance::GetCurrent()->sceneobjects)
	{
		SaveObject(x, mainElem);
	}
	doc.InsertEndChild(mainElem);
	doc.SaveFile(*path);
}

//---------------------------------------------------------------

void GameObject::LoadPrfabsOf(const String& path, bool useR)
{
	TiXmlDocument doc("SceneSpawns");
	doc.LoadFile(*path);
	TiXmlNode* attr = doc.FirstChild("Scene");
	TiXmlNode* nd = doc.FirstChild("Prefab");
	while (nd)
	{
		TiXmlElement* e = nd->ToElement();
		String path = e->Attribute("path");
		String srot = e->Attribute("localRotation");
		String spos = e->Attribute("localPosition");
		Quaternion rot;
		Vector3 pos;
		sscanf(*srot, "%f:%f:%f:%f", rot.x, rot.y, rot.z, rot.w);
		sscanf(*spos, "%f:%f:%f", pos.x, pos.y, pos.z);
		GameObject::SpawnPrefab(path, "[Simple Prefab]", pos, rot);
		nd = nd->NextSibling();
	}
}
