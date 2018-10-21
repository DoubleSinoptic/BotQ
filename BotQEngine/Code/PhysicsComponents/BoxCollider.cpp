#include "Detail/BtDetail.h"
#include "BoxCollider.h"
#include "../GameObject.h"

class BoxColliderImpl 
{
public:
	BoxColliderImpl(const btVector3& sz)
		: _shape(sz)
	{
	}
	btBoxShape _shape;
};

BoxCollider::BoxCollider()
    : mImpl(new BoxColliderImpl({0.5, 0.5, 0.5}))
{
	SetEnabled(false);
}

void BoxCollider::Awake()
{
	InstallShape(&mImpl->_shape);
}


void BoxCollider::SetSize(const Vector3 &size)
{
	mImpl->_shape = btBoxShape(internal_convert(size) / 2);
	InstallShape(&mImpl->_shape);
}

Vector3 BoxCollider::GetSize()
{
    return internal_convert(mImpl->_shape.getHalfExtentsWithoutMargin() * 2.0f);
}

BoxCollider::~BoxCollider()
{
	if (mImpl)
		delete mImpl;
}

