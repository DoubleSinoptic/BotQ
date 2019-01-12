#include "Tools.h"
#include "GameObject.h"
#include "Graphics/Texture.h"
#include "Time.hpp"
#include "Input.h"
#include "Audio/AudioSource.h"
#include "MeshRenderer.h"
#include "Material.h"

void Tools::SetMaterial(GameObject *cube, Material* m)
{
	for (GameObject* c : *cube)
		SetMaterial(c, m);

	DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();
	for (auto& i : r)
	{
		i->SetMaterial(m);	
	}

}
void Tools::SetMRAO(GameObject* cube, float m, float r, float ao)
{
	for (GameObject* c : *cube)
		SetMRAO(c,  m,  r,  ao);

	DynamicArray<MeshRenderer*> re = cube->GetComponents<MeshRenderer>();
	for (auto& i : re)
	{

		static_cast<ObjectMaterial*>(i->GetMaterial())->metalic_roughness_ao_static = Vector3(m, r, ao);

	}

}
void Tools::SetMaps(GameObject *cube, Texture *k, Texture *n, Texture *mt, Texture *rt, Texture *ao)
{
    for (GameObject* c : *cube)
        SetMaps(c, k, n, mt, rt, ao);

    DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();
    for (auto& i : r)
    {     
            static_cast<ObjectMaterial*>(i->GetMaterial())->albedo =(k);
			static_cast<ObjectMaterial*>(i->GetMaterial())->normals =(n);
			static_cast<ObjectMaterial*>(i->GetMaterial())->metalic = mt;
			static_cast<ObjectMaterial*>(i->GetMaterial())->roughness = rt;
			static_cast<ObjectMaterial*>(i->GetMaterial())->ao = ao;
        
    }

}

void Tools::ScaleAll(GameObject *cube, float scale)
{
    for (GameObject* c : *cube)
        ScaleAll(c, scale);

	DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();
	for (auto& i : r)
    {
        i->SetScale(Vector3(scale, scale, scale));
    }
	cube->OnTransformChanged();
}

void Tools::ScaleAll(GameObject * cube, const Vector3 & vec)
{
	for (GameObject* c : *cube)
		ScaleAll(c, vec);

	DynamicArray<MeshRenderer*> r = cube->GetComponents<MeshRenderer>();

	for (auto& i : r)
	{
		i->SetScale(vec);
	}
	cube->OnTransformChanged();
}

void FixedRotation::Awake() 
{
	SetEnabled(true);
}

void FixedRotation::PhysicUpdate()
{
    GetGameObject()->SetLocalRotation(Quaternion(0.0, Time::GetTotalTime() * (Mathf::Pi() / 6 ) * RAD_TO_DEG , 0.0));
}

void Tools::RotateLocalAll(class GameObject* cube, const Quaternion& rot)
{
	for (GameObject* c : *cube)
		RotateLocalAll(c, rot);
	cube->SetLocalRotation(rot);
}

void DebugFlyCamera::Awake()
{
	SetEnabled(true);
}

void DebugFlyCamera::PhysicUpdate()
{

	float mull = 1.0f;
	const float FlashSpeed = 40.5f;
	const float thisSpeed = FlashSpeed;
	float renderDelta = Time::GetRenderDeltaTime();

	Vector3 forward = (GetGameObject()->GetForward() *  Vector3(1.0f, 1.0f, 1.0f)).Normalized();
	Vector3 right = (GetGameObject()->GetRight() *  Vector3(1.0f, 1.0f, 1.0f)).Normalized();
	Vector3 acum = Vector3::Zero();
	if (Input::IsKeyDown(SGE_KEY_W))
	{
		acum += forward;
	}
	if (Input::IsKeyDown(SGE_KEY_S))
	{
		acum -= forward;
	}
	if (Input::IsKeyDown(SGE_KEY_A))
	{
		acum -= right * Matrix4::RightSign();
	}
	if (Input::IsKeyDown(SGE_KEY_D))
	{
		acum += right * Matrix4::RightSign();
	}

	Vector3 e = acum.Normalized() * 12.9f * Time::GetDeltaTime();
	GetGameObject()->SetLocalPosition(GetGameObject()->GetLocalPosition() + e);

	if (Input::IsMouseKeyDown(SGE_MOUSE_BUTTON_LEFT))
	{
		if (hasPressed)
		{
			const Point mousePoint = Input::GetMausePosition();
			dx = mousePoint.x - curre23nt.x;
			dy = mousePoint.y - curre23nt.y;
			curre23nt = mousePoint;

			GetGameObject()->SetLocalRotation(Quaternion(
				GetGameObject()->GetLocalRotation().GetEuler()
				+ Vector3(dy * 0.1f, dx * 0.1f  * Matrix4::RightSign(), 0)));
			float a = slesh;
			float b = dx;
			slesh = (a + (b - a) * 0.1);
		}
		else
		{
			hasPressed = true;
			curre23nt = Input::GetMausePosition();
		}


	}
	else
		hasPressed = false;
	AudioSource::SetupListener(GetGameObject()->GetPosition(), GetGameObject()->GetRotation());

}
