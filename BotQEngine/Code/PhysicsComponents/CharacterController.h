#pragma once
#ifndef __CHACTRL_H__
#define __CHACTRL_H__

#include "../Component.h"
#include "../GameObject.h"

class SGE_EXPORT CharacterController : public Component
{
	TYPED_OBJECT;

	int updateCounts = 0;
	Vector3 moveAcum;
public:
	CharacterController();
	~CharacterController();
	void Awake() override;
	void PhysicUpdate() override;
	void Destroy() override;
	void Move(const Vector3& v);
};

#endif
