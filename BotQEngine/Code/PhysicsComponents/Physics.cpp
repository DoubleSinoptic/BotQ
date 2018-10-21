#include "Detail/BtDetail.h"
#include "Physics.h"

btDiscreteDynamicsWorld* Internal_GetPhysicsWorldFromGameInstance();
StorageFunction<void(const Vector3&v, const Vector3&v1, const Vector3&c, const Vector3&c1)> debugCallback;

class BtDebugDrawer : public btIDebugDraw
{
	btVector3 ft[4];
public:
	BtDebugDrawer()
	{}

	~BtDebugDrawer()
	{
	}

	// btIDebugDraw interface
public:
	void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override
	{
		if(debugCallback)
			debugCallback(internal_convert(from), internal_convert(to), internal_convert(color), internal_convert(color));
	}
	void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color) override
	{
	}
	void reportErrorWarning(const char *warningString) override
	{
		Log("bullet debug warning: %s", warningString);
	}
	void draw3dText(const btVector3 &location, const char *textString) override
	{
		Log("bullet debug warning: %s", textString);
	}
	int debugMod;
	void setDebugMode(int debugMode) override
	{
		debugMod = debugMode;
	}
	int getDebugMode() const override
	{
		return debugMod;
	}
} debugDrawer_o;
 

void Physics::SetDebugDrawCallback(const StorageFunction<void(const Vector3&v, const Vector3&v1, const Vector3&c, const Vector3&c1)> f)
{
	debugCallback = f;
}

void Physics::DrawDebugPhysicsWorld()
{
	Internal_GetPhysicsWorldFromGameInstance()->setDebugDrawer(&debugDrawer_o);
	
	debugDrawer_o.setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);
	Internal_GetPhysicsWorldFromGameInstance()->debugDrawWorld();
}

bool Physics::RayCast(const Vector3& origin, const Vector3& dir, RayHitRezult& rezult, float maxLength)
{
	btVector3 from = btVector3(origin.x, origin.y, origin.z);
	btVector3 to = from + (btVector3(dir.x, dir.y, dir.z).normalized() * maxLength);
	btCollisionWorld::ClosestRayResultCallback c(from, to);
	Internal_GetPhysicsWorldFromGameInstance()->rayTest(from, to, c);
	if (c.hasHit())
	{
		rezult.body = (RigidBody*)c.m_collisionObject->getUserPointer();
		rezult.origin = origin;
		rezult.dir = dir;
		rezult.normal = Vector3(c.m_hitNormalWorld.x(), c.m_hitNormalWorld.y(), c.m_hitNormalWorld.z());
		rezult.point = Vector3(c.m_hitPointWorld.x(), c.m_hitPointWorld.y(), c.m_hitPointWorld.z());

		if ((origin - rezult.point).Length() > maxLength)
			return false;
		return true;
	}
	return false;
}


void Physics::Simulate(float timestep) 
{
	Internal_GetPhysicsWorldFromGameInstance()->stepSimulation(timestep, 0, 0.0);
}
