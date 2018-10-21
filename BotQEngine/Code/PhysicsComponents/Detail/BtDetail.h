#pragma once

#include "../../GameObject.h"
#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


#include "BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#include "BulletDynamics/Dynamics/btSimulationIslandManagerMt.h"  // for setSplitIslands()
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h"
#include "BulletDynamics/MLCPSolvers/btMLCPSolver.h"
#include "BulletDynamics/MLCPSolvers/btSolveProjectedGaussSeidel.h"
#include "BulletDynamics/MLCPSolvers/btDantzigSolver.h"
#include "BulletDynamics/MLCPSolvers/btLemkeSolver.h"

inline btVector3 internal_convert(const Vector3& vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}
inline Vector3 internal_convert(const btVector3& vec)
{
	return Vector3(vec.x(), vec.y(), vec.z());
}
inline btQuaternion internal_convert(const Quaternion& vec)
{
	return btQuaternion(vec.x, vec.y, vec.z, vec.w);
}
inline Quaternion internal_convert(const btQuaternion& vec)
{
	return Quaternion(vec.x(), vec.y(), vec.z(), vec.w());
}
inline btTransform internal_convert(const GameObject& vec)
{
	return btTransform(internal_convert(vec.GetRotation()), internal_convert(vec.GetPosition()));
}