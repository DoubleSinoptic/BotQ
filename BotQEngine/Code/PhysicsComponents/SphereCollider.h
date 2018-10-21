#ifndef __SPHERECOLLIDEREX_H__
#define __SPHERECOLLIDEREX_H__

#include "Collider.h"

class SphereColliderImpl;

class SGE_EXPORT SphereCollider : public Collider
{
	TYPED_OBJECT;
	SphereColliderImpl* mImpl;
public:
	SphereCollider();
	~SphereCollider();
    void Awake() override;

	REGISTER_PROPETRY(float, Radius)
    void SetRadius(const float& size);
    float GetRadius(); 
};

#endif //__SPHERECOLLIDEREX_H__
