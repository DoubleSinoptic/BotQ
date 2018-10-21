#pragma once
#ifndef __COLLIDER_H__
#define __COLLIDER_H__

#include "../Component.h"

class ColliderImpl;

class SGE_EXPORT Collider : public Component
{
	TYPED_OBJECT;
	Vector3			mCenter;
	ColliderImpl*	mImpl;
public:
	~Collider();
	Collider();
   
	void InstallShape(void* shape);
	void* GetInstalledShape() const;

	REGISTER_PROPETRY(Vector3, Center)
	void SetCenter(const Vector3& center);
	Vector3 GetCenter() const;
};

#endif
