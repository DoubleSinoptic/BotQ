#pragma once
#ifndef __RIGID_BODY_H__
#define __RIGID_BODY_H__

#include "../Component.h"
#include "../GameObject.h"

class RigidBodyExMotionState;

class RigidBodyImpl;

class SGE_EXPORT RigidBody : public Component
{
	TYPED_OBJECT;
	RigidBodyImpl* mImpl;
#ifndef OLD_RIGIDBODY_SYSTEM
	Ref<EventHandler<>> mApplayHandler;
#endif
	friend class RigidBodyExMotionState;
public:
	void Rebuild();
	void ApplayTransform(GameObject* transform);

	void* GetNativeBody() const;

	REGISTER_PROPETRY(float, Mass)
	void SetMass(float mass);
	float GetMass() const;

	REGISTER_PROPETRY(Vector3, AngularVelocity)
	Vector3 GetAngularVelocity() const;
	void SetAngularVelocity(const Vector3 &vel);

	REGISTER_PROPETRY(Vector3, LinearVelocity)
	Vector3 GetLinearVelocity() const;
	void SetLinearVelocity(const Vector3 &vel);

	REGISTER_PROPETRY(bool, IsKinematic)
	void SetIsKinematic(bool value);
	bool GetIsKinematic()  const;

	void AddForce(const Vector3& value);
	void AddForceAtPosition(const Vector3& value, const Vector3& position);
	void AddTorgue(const Vector3& value);
	void DisableRotation();

	RigidBody();
	~RigidBody();
	void Awake() override;
	void PhysicUpdate() override;
	void Destroy() override;
};

#endif
