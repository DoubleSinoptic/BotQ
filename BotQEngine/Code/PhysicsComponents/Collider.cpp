#include "Collider.h"
#include "RigidBody.h"
#include "../GameObject.h"

#include "btBulletCollisionCommon.h"

class ColliderImpl 
{
public:
	ColliderImpl() 
	{
		
	}

	~ColliderImpl() 
	{
		
	}

	btCollisionShape* shape;
};

Collider::~Collider()
{
	if(mImpl)
		delete mImpl;
}

Collider::Collider()
	: mImpl(new ColliderImpl()), mCenter(Vector3::Zero())
{
	SetEnabled(false);
}

void Collider::InstallShape(void * shape)
{
	mImpl->shape = (btCollisionShape*)shape;
	GameObject* workobject = GetGameObject();
	RigidBody* body = workobject->GetComponent<RigidBody>();
	if (!body)
		return;
	body->Rebuild();
}

void * Collider::GetInstalledShape() const
{
	return mImpl->shape;
}

void Collider::SetCenter(const Vector3 & center)
{
	mCenter = center;
}

Vector3 Collider::GetCenter() const
{
	return mCenter;
}