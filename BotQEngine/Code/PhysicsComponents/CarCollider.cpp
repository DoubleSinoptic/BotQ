#include "Detail/BtDetail.h"
#include "CarCollider.h"
#include "../GameObject.h"
#include "../GameInstance.h"
#include "RigidBody.h"
#include "../Time.hpp"
struct WheelInfo 
{
	WheelInfo() {}
	WheelInfo(GameObject* a1, int a2, const Vector3& cpos, bool front)
		: wheelModel(a1), id(a2), contrPosition(cpos), isFront(front)
	{}
	Vector3 contrPosition;
	GameObject* wheelModel = nullptr;;
	int id = 0;
	bool isFront;
};

class CarColliderImpl
{
public:
	CarColliderImpl()
	{

	}
	
	btRaycastVehicle::btVehicleTuning _tuning;
	DynamicArray<WheelInfo> mWheels;
	Ref<btDefaultVehicleRaycaster> _vehicleRayCaster;
	Ref<btRaycastVehicle> _vehicle;

	int numWheels = 0;
	int _wsteer = 0;

	~CarColliderImpl() 
	{
	}
};

CarCollider::CarCollider()
	: mImpl(new CarColliderImpl())
{
	
}

void* Internal_GetPhysicsShare(PhysicsInstance* c);

void CarCollider::Awake()
{
	SetEnabled(true);
	if (this->GetGameObject()->GetComponent<RigidBody>() == nullptr)
		Error("RigidBody not attached in object");

	btRigidBody* _carChassis = (btRigidBody*)GetGameObject()->GetComponent<RigidBody>()->GetNativeBody();

	mImpl->_vehicleRayCaster = new btDefaultVehicleRaycaster((btDynamicsWorld*)Internal_GetPhysicsShare(GameInstance::GetCurrent()->physics));

	mImpl->_vehicle = new btRaycastVehicle(mImpl->_tuning, _carChassis, mImpl->_vehicleRayCaster.GetPtr());

	_carChassis->setActivationState(DISABLE_DEACTIVATION);

	mImpl->_vehicle->setCoordinateSystem(0, 1, 2);
	//((btDynamicsWorld*)Internal_GetPhysicsShare(GameInstance::GetCurrent()->physics))->addAction(mImpl->_vehicle);
	
}

void CarCollider::Destroy()
{
	//((btDynamicsWorld*)Internal_GetPhysicsShare(GameInstance::GetCurrent()->physics))->removeAction(mImpl->_vehicle);
}

void CarCollider::ApplayDriverState(bool _U, bool _D, bool _L, bool _R)
{
	btRaycastVehicle* m_vehicle = mImpl->_vehicle.GetPtr();
	if (_U)
	{
		if (m_vehicle->getCurrentSpeedKmHour() < 40)
		{

			m_vehicle->applyEngineForce(3000.f, 2);
			m_vehicle->setBrake(0.0f, 2);
			m_vehicle->applyEngineForce(3000.f, 3);
			m_vehicle->setBrake(0.0f, 3);

		}
		else
		{
			m_vehicle->applyEngineForce(0.0f, 2);
			m_vehicle->applyEngineForce(0.0f, 3);
		}
	}
	else if (_D)
	{
		if (m_vehicle->getCurrentSpeedKmHour()  < 40)
		{

			m_vehicle->applyEngineForce(-3000.f, 2);
			m_vehicle->setBrake(0.0f, 2);
			m_vehicle->applyEngineForce(-3000.f, 3);
			m_vehicle->setBrake(0.0f, 3);

		}
		else
		{
			m_vehicle->applyEngineForce(0.0f, 2);
			m_vehicle->applyEngineForce(0.0f, 3);
		}
			

	}
	else
	{

		m_vehicle->applyEngineForce(0.0f, 2);
		m_vehicle->setBrake(30.0f, 2);
		m_vehicle->applyEngineForce(0.0f, 3);
		m_vehicle->setBrake(30.0f, 3);

	}

	//_wsteer
	if (_L)
	{
		if (mImpl->_wsteer < 45)
			mImpl->_wsteer += 5;

	}
	else if (_R)
	{

		if (mImpl->_wsteer > -45)
			mImpl->_wsteer -= 5;
	}
	else
	{
		if (mImpl->_wsteer < 0)
			mImpl->_wsteer += 5;
		if (mImpl->_wsteer > 0)
			mImpl->_wsteer -= 5;
	}

	m_vehicle->setSteeringValue(static_cast<float>(mImpl->_wsteer) * (3.14 / 180.0), 0);
	m_vehicle->setSteeringValue(static_cast<float>(mImpl->_wsteer) * (3.14 / 180.0), 1);
}

void CarCollider::AddWheel(GameObject* wheelT, const Vector3 & position, bool isFrontWheel)
{
	if (wheelT == nullptr)
		Error("wheel not support null render model in object wheel");
	btWheelInfo& wheel = mImpl->_vehicle->addWheel(internal_convert(position), internal_convert(wheelDirection), internal_convert(wheelAxleCS), suspensionRestLength, wheelRadius, mImpl->_tuning, isFrontWheel);
	wheel.m_suspensionStiffness = suspensionStiffness;
	wheel.m_wheelsDampingRelaxation = suspensionDamping;
	wheel.m_wheelsDampingCompression = suspensionCompression;
	wheel.m_frictionSlip = wheelFriction;
	wheel.m_rollInfluence = rollInfluence;
	wheel.m_maxSuspensionForce = maxSuspensionForce;
	wheel.m_maxSuspensionTravelCm = maxSuspensionTravelCm;

	wheelT->SetClonable(false);
	wheelT->SetParent(this->GetGameObject());
	WheelInfo wg(wheelT, mImpl->numWheels, position, isFrontWheel);
	mImpl->mWheels.Add(wg);
	mImpl->numWheels++;

}

void CarCollider::CloneTo(TypedObject * obj) const
{
	CarCollider* e = dynamic_cast<CarCollider*>(obj);
	if (!obj)
		Error("Ivalid clone to covesion from CarCollider");
	for (const auto& i : mImpl->mWheels)
		e->AddWheel(GameObject::Clone(i.wheelModel), i.contrPosition, i.isFront);
}

void CarCollider::FrameUpdate()
{
	
}

void CarCollider::PhysicUpdate() 
{
	
	/*GameObject* par = GetGameObject();
	while (par)
	{
		if (par->GetParent() == nullptr)
			par->OnTransformChanged();
		par = par->GetParent();
	}*/
	mImpl->_vehicle->updateAction((btDynamicsWorld*)Internal_GetPhysicsShare(GameInstance::GetCurrent()->physics), Time::GetDeltaTime());
	for (const WheelInfo& i : mImpl->mWheels)
	{
		mImpl->_vehicle->updateWheelTransform(i.id, false);
		btWheelInfo& wheel = mImpl->_vehicle->getWheelInfo(i.id);

		if (wheel.m_chassisConnectionPointCS.x() < 0.0)
		{
			i.wheelModel->SetLocalPosition(
				internal_convert(wheel.m_localTransform.getOrigin())); 
			i.wheelModel->SetLocalRotation(
				internal_convert(wheel.m_localTransform.getRotation())  * Quaternion(0, Mathf::Pi(), 0)
			);
		}
		else
		{

			i.wheelModel->SetLocalPosition(
				internal_convert(wheel.m_localTransform.getOrigin()));
			i.wheelModel->SetLocalRotation(
				internal_convert(wheel.m_localTransform.getRotation()) 
			);

		}


		/*if (wheel.m_chassisConnectionPointCS.x() < 0.0)
		{
			i.wheelModel->SetTransform(
				internal_convert(wheel.m_worldTransform.getOrigin()),
				internal_convert(wheel.m_worldTransform.getRotation())  * Quaternion(0, Mathf::Pi(), 0)
			);
		}
		else
		{
			i.wheelModel->SetTransform(
				internal_convert(wheel.m_worldTransform.getOrigin()),
				internal_convert(wheel.m_worldTransform.getRotation())
			);

		}*/
	}

}

CarCollider::~CarCollider()
{
	if (mImpl)
		delete mImpl;
}

