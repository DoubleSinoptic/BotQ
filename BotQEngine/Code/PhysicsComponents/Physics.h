#pragma once

#include "../Common/StorageFunction.h"
#include "../Math/Vector3.h"
#include <float.h>

class RigidBody;
struct RayHitRezult 
{
	RigidBody* body;
	Vector3 point;
	Vector3 normal;
	Vector3 origin;
	Vector3 dir;
};

class SGE_EXPORT Physics
{
public:
	
	static void SetDebugDrawCallback(const StorageFunction<void(const Vector3& v, const Vector3& v1, const Vector3& c, const Vector3& c1)>);
	static void DrawDebugPhysicsWorld();

	static bool RayCast(const Vector3& origin, const Vector3& dir, RayHitRezult& rezult, float maxLength = FLT_MAX);
	static void Simulate(float timestep);

};

