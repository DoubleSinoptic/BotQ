#include "SphereCollider.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

class SphereColliderImpl 
{
public:
	SphereColliderImpl(float radius)
		: _shape(radius)
	{}
	btSphereShape _shape;
};

SphereCollider::SphereCollider()
    : mImpl(new SphereColliderImpl(0.5))
{
	SetEnabled(false);
}

void SphereCollider::Awake()
{
	InstallShape(&mImpl->_shape);
}

void SphereCollider::SetRadius(const float &size)
{
	mImpl->_shape = btSphereShape(size);
	InstallShape(&mImpl->_shape);
}

float SphereCollider::GetRadius()
{
    return mImpl->_shape.getRadius();
}

SphereCollider::~SphereCollider()
{
	if (mImpl)
		delete mImpl;
}
