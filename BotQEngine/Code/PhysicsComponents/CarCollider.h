#pragma once
#ifndef __CARCOLLIDEREX_H__
#define __CARCOLLIDEREX_H__

#include "../Component.h"

class CarColliderImpl;

class SGE_EXPORT CarCollider : public Component
{
	TYPED_OBJECT
	CarColliderImpl* mImpl;
	float wheelFriction = 20.0f;
	float suspensionStiffness = 50.f;
	float suspensionDamping = 1.3f;
	float suspensionCompression = 2.4f;
	float rollInfluence = 0.1f;
	float maxSuspensionForce = 600000.0f;
	float maxSuspensionTravelCm = 20.0f;
	float wheelRadius = 0.5 * 0.68845;

	float suspensionRestLength = 0.64f;
	Vector3 wheelDirection = Vector3(0, -1, 0);
	Vector3 wheelAxleCS = Vector3(-1, 0, 0);
public:
	CarCollider();
	~CarCollider();
	void Awake() override;
	void Destroy() override;
	void PhysicUpdate() override;

	void ApplayDriverState(bool _U, bool _D, bool _L, bool _R);
	void AddWheel(GameObject* wheel, const Vector3& position, bool isFrontWheel);
	void CloneTo(TypedObject* obj) const override;

	void FrameUpdate() override;
};

#endif //__CARCOLLIDEREX_H__
