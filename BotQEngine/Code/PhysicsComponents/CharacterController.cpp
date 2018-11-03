#include "CharacterController.h"
#include "SphereCollider.h"
#include "RigidBody.h"
#include "../Time.hpp"
CharacterController::CharacterController()
{
}

CharacterController::~CharacterController()
{
}

void CharacterController::Awake()
{
	SetEnabled(true);
	GetGameObject()->AddComponent<SphereCollider>()->SetRadius(0.5);
	RigidBody* r = GetGameObject()->AddComponent<RigidBody>();
	r->DisableRotation();
	r->SetMass(75.0f);
}

void CharacterController::PhysicUpdate()
{
	if (moveAcum.Length() == 0.0f)
		GetGameObject()->GetComponent<RigidBody>()->SetLinearVelocity(Vector3::Zero());

	GetGameObject()->GetComponent<RigidBody>()->SetLinearVelocity(moveAcum * (1.0f/Time::GetDeltaTime()));

	moveAcum = Vector3::Zero();
}

void CharacterController::Destroy()
{
}

void CharacterController::Move(const Vector3 & v)
{
	moveAcum += v;
	updateCounts++;
}
