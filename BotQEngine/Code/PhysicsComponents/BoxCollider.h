#pragma once
#ifndef __BOXCOLLIDEREX_H__
#define __BOXCOLLIDEREX_H__

#include "Collider.h"

class BoxColliderImpl;

class SGE_EXPORT BoxCollider : public Collider
{
	TYPED_OBJECT
	BoxColliderImpl* mImpl;
public:
    BoxCollider();
	~BoxCollider();
    void Awake() override;

	REGISTER_PROPETRY(Vector3, Size)
    void SetSize(const Vector3& size);
    Vector3 GetSize();
};

#endif //__BOXCOLLIDEREX_H__
