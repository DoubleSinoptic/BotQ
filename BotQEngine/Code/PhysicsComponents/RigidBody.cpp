#include "Detail/BtDetail.h"
#include "RigidBody.h"
#include "Collider.h"
#include "../GameInstance.h"
#include "Physics.h"


#ifdef KH_PHYSICS

#include "Utility.h"

int cpuCount = GetProcessorCount();

class MyTaskShedulerFromBt : public btITaskScheduler
{
public:
	MyTaskShedulerFromBt() : btITaskScheduler("KH") 
	{
		AddDistributorThreads(cpuCount);
		
	}

	~MyTaskShedulerFromBt() 
	{
		StopAllDistributorThreads();
	}

	virtual int getMaxNumThreads() const override
	{
		return cpuCount;
	}

	virtual int getNumThreads() const override
	{
		return cpuCount;
	}

	virtual void setNumThreads(int numThreads) override
	{

	}

	virtual void parallelFor(int iBegin, int iEnd, int grainSize, const btIParallelForBody & body) override
	{
		ParallelFor(iBegin, iEnd, [&](int b, int c) 
		{
			body.forLoop(b, c);
		});
	}

};
Ref<MyTaskShedulerFromBt> KH_TASK_SHEDULLER = nullptr;
#else
int cpuCount = 1;
#endif


btBoxShape _defultShape(btVector3(0.5, 0.5, 0.5));
class RigidBodyImpl
{
public:
	RigidBodyImpl()
		: _body(_mass, nullptr, &_defultShape), _state(nullptr), m_compoundShape(nullptr)
	{}

	bool _isUpdated = false;
	float _mass = 0.0f;

	
	btRigidBody _body;
	btCompoundShape* m_compoundShape;
	RigidBodyExMotionState* _state;
	friend class RigidBodyExMotionState;

	const float _tinhalf = 0.00001f;
};



class RigidBodyExMotionState : public btMotionState 
{
public:
	GameObject* m_obj = nullptr;
	RigidBody* m_ex = nullptr;
	void getWorldTransform(btTransform& worldTrans) const override
	{
		worldTrans = internal_convert(*m_obj);
	}

	void setWorldTransform(const btTransform& worldTrans) override
	{
		m_ex->mImpl->_isUpdated = true;
		m_obj->SetTransform(internal_convert(worldTrans.getOrigin()), internal_convert(worldTrans.getRotation()));
		m_ex->mImpl->_isUpdated = false;
	}
};


class PhysicsInstance
{
public:



	Ref<btGhostPairCallback> _ghostCallback;
	Ref<btDefaultCollisionConfiguration> _collisionConfiguration;
	Ref<btCollisionDispatcherMt> _dispatcher;
	Ref<btBroadphaseInterface> _broadphase;
	Ref<btConstraintSolverPoolMt> _pool;
	Ref<btDiscreteDynamicsWorld> world;
	DynamicArray<btSequentialImpulseConstraintSolver*> _solvers;

	btDiscreteDynamicsWorld* _world;

	PhysicsInstance()
	{
#ifdef KH_PHYSICS
		if (!KH_TASK_SHEDULLER)
			KH_TASK_SHEDULLER = new MyTaskShedulerFromBt();
		btSetTaskScheduler(KH_TASK_SHEDULLER.GetPtr());
#else
		btSetTaskScheduler(btGetSequentialTaskScheduler());
#endif

		for (int i = 0; i < cpuCount; i++)
			_solvers.Add(new btSequentialImpulseConstraintSolver());
	
		_ghostCallback = new btGhostPairCallback();
		_collisionConfiguration = new btDefaultCollisionConfiguration();
		_dispatcher = new btCollisionDispatcherMt((btCollisionConfiguration*)_collisionConfiguration.GetPtr(), 40);

		btVector3 bboxSize = btVector3(10000.0f, 10000.0f, 10000.0f);
		_broadphase = new btDbvtBroadphase();
		
		_pool = new btConstraintSolverPoolMt((btConstraintSolver**)_solvers.GetData(), cpuCount);
		world = new btDiscreteDynamicsWorldMt(_dispatcher.GetPtr(), _broadphase.GetPtr(), _pool.GetPtr(), _collisionConfiguration.GetPtr());
		world->performDiscreteCollisionDetection();

		world->setGravity(btVector3(0, -9.8196, 0));
		world->getPairCache()->setInternalGhostPairCallback(_ghostCallback.GetPtr());
		_world = world.GetPtr();
		
	}
};

PhysicsInstance* Internal_AllocateAndConstructPhysicsContext() 
{
	return new PhysicsInstance();
}

void Internal_DestructAndDeallocatePhysicsContext(PhysicsInstance* c)
{
	delete c;
}

void Internal_SimulatePhysicsContext(PhysicsInstance* c, float time, int substepscount, float substeptime) 
{
	(void)substepscount;
	(void)substeptime;
	c->_world->stepSimulation(time);
}

void* Internal_GetPhysicsShare(PhysicsInstance* c)
{
	return c->_world;
}

btDiscreteDynamicsWorld* Internal_GetPhysicsWorldFromGameInstance() 
{
	return GameInstance::GetCurrent()->physics->_world;
}

void RigidBody::Rebuild()
{
	GameInstance::GetCurrent()->physics->_world->removeRigidBody(&mImpl->_body);
	DynamicArray<Collider*> shapes = GetGameObject()->GetComponents<Collider>();	
	if (shapes.Length() == 0)
	{
		mImpl->_body.setCollisionShape(&_defultShape);
		if (mImpl->m_compoundShape)
		{
			delete mImpl->m_compoundShape;
			mImpl->m_compoundShape = nullptr;
		}
	}
	if (shapes.Length() == 1)
	{
		if (shapes[0]->GetInstalledShape() != nullptr)
		{
			mImpl->_body.setCollisionShape((btCollisionShape*)shapes[0]->GetInstalledShape());
			if (mImpl->m_compoundShape)
			{
				delete mImpl->m_compoundShape;
				mImpl->m_compoundShape = nullptr;
			}
		}	
	}
	if (shapes.Length() > 1)
	{
		if (mImpl->m_compoundShape)
		{
			delete mImpl->m_compoundShape;
			mImpl->m_compoundShape = nullptr;
		}
		mImpl->m_compoundShape = new btCompoundShape();
		for (Collider* i : shapes)
		{
			if(i->GetInstalledShape() != nullptr)
				mImpl->m_compoundShape->addChildShape(btTransform(btQuaternion::getIdentity(), internal_convert(i->GetCenter())), (btCollisionShape*)i->GetInstalledShape());
		}	
		mImpl->_body.setCollisionShape(mImpl->m_compoundShape);
	}
	
	if (mImpl->_mass == 0.0f)
		mImpl->_body.setMassProps(0.0f, { 0.0, 0.0, 0.0 });
	else 
	{
		btVector3 inertia(0.0, 0.0, 0.0);
		mImpl->_body.getCollisionShape()->calculateLocalInertia(mImpl->_mass, inertia);
		mImpl->_body.setMassProps(mImpl->_mass, inertia);		
		mImpl->_body.activate();
	}	
	GameInstance::GetCurrent()->physics->_world->addRigidBody(&mImpl->_body);
}

void RigidBody::ApplayTransform(GameObject * transform)
{
	if (!mImpl->_isUpdated)
	{
		mImpl->_body.setWorldTransform(internal_convert(*transform));
		mImpl->_body.activate();
	}
		
}

void * RigidBody::GetNativeBody() const
{
	return &mImpl->_body;
}

void RigidBody::SetMass(float massa)
{
	
	GameInstance::GetCurrent()->physics->_world->removeRigidBody(&mImpl->_body);
	mImpl->_mass = massa;
	if (mImpl->_mass == 0.0f)
		mImpl->_body.setMassProps(0.0f, { 0.0, 0.0, 0.0 });
	else
	{
		btVector3 inertia(0.0, 0.0, 0.0);
		mImpl->_body.getCollisionShape()->calculateLocalInertia(mImpl->_mass, inertia);
		mImpl->_body.setMassProps(mImpl->_mass, inertia);
	}
	GameInstance::GetCurrent()->physics->_world->addRigidBody(&mImpl->_body);
	Rebuild();
}

float RigidBody::GetMass() const
{
	return mImpl->_mass;
}

RigidBody::RigidBody()
	: mImpl(new RigidBodyImpl()), mApplayHandler(nullptr)
{
	mImpl->_body.setUserPointer(this);
}

RigidBody::~RigidBody()
{
	if (mImpl)
		delete mImpl;
}

void RigidBody::Awake()
{
	mImpl->_state = nullptr;
	mImpl->_state = new RigidBodyExMotionState();
	mImpl->_state->m_obj = GetGameObject();
	mImpl->_state->m_ex = this;

	GameInstance::GetCurrent()->physics->_world->addRigidBody(&mImpl->_body);

#ifdef OLD_RIGIDBODY_SYSTEM
    GetGameObject()->SetRigidBody(this);
#endif
	mImpl->_body.setWorldTransform(internal_convert(*GetGameObject()));
	mImpl->_body.setMotionState(mImpl->_state);
	Rebuild();
#ifndef OLD_RIGIDBODY_SYSTEM
	mApplayHandler = new EventHandler<>([=]()
	{
		this->ApplayTransform(GetGameObject());
	});
	GetGameObject()->GetHandlers()->OnTransformChanged += mApplayHandler.GetPtr();
#endif
}

void RigidBody::PhysicUpdate()
{
	
}

void RigidBody::Destroy()
{
#ifndef OLD_RIGIDBODY_SYSTEM
	Assert(mApplayHandler);
	GetGameObject()->GetHandlers()->OnTransformChanged -= mApplayHandler.GetPtr();
#endif
	if (mImpl->m_compoundShape)
	{
		delete mImpl->m_compoundShape;
		mImpl->m_compoundShape = nullptr;
	}
#ifdef OLD_RIGIDBODY_SYSTEM
	GetGameObject()->SetRigidBody(nullptr);
#endif
	GameInstance::GetCurrent()->physics->_world->removeRigidBody(&mImpl->_body);
	if(mImpl->_state)
		delete mImpl->_state;

}

void RigidBody::SetIsKinematic(bool value) 
{
	if(value)
		mImpl->_body.setCollisionFlags(mImpl->_body.getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	else
		mImpl->_body.setCollisionFlags(mImpl->_body.getCollisionFlags() & (~btCollisionObject::CF_KINEMATIC_OBJECT));
}

bool RigidBody::GetIsKinematic()  const
{
	return mImpl->_body.isKinematicObject();

}

void RigidBody::AddForce(const Vector3 & value)
{
	mImpl->_body.applyCentralForce(internal_convert(value));
}

void RigidBody::AddForceAtPosition(const Vector3 & pxForce, const Vector3 & pxPos)
{
	Vector3 torque = (pxPos - internal_convert(mImpl->_body.getCenterOfMassPosition())).Cross(pxForce);
	AddForce(pxForce);
	AddTorgue(torque);
}

void RigidBody::AddTorgue(const Vector3 & value)
{
	mImpl->_body.applyTorque(internal_convert(value));
}

void RigidBody::DisableRotation()
{
	mImpl->_body.setAngularFactor(0.0f);
	mImpl->_body.setSleepingThresholds(0.0, 0.0);
}

Vector3 RigidBody::GetAngularVelocity() const
{
    return internal_convert(mImpl->_body.getAngularVelocity());
}

void RigidBody::SetAngularVelocity(const Vector3 &vel)
{
	mImpl->_body.setAngularVelocity(internal_convert(vel));
}

Vector3 RigidBody::GetLinearVelocity() const
{
    return internal_convert(mImpl->_body.getLinearVelocity());
}

void RigidBody::SetLinearVelocity(const Vector3 &vel)
{
	mImpl->_body.setLinearVelocity(internal_convert(vel));
}

