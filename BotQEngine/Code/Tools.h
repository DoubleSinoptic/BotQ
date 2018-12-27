#ifndef TOOLS_H
#define TOOLS_H

#include "Component.h"
#include "Math/PointAndSize.h"
#include "Math/Matrix4.h"
#include "GameObject.h"

class SGE_EXPORT FixedRotation : public Component
{
public:
	void Awake() override;
    void PhysicUpdate() override;
};


class SGE_EXPORT DebugFlyCamera : public Component
{
	int dx = 0;
	int dy = 0;
	float slesh = 0;
	float angleX = 0.0;
	float angleY = 0.0;
	bool hasPressed = true;
	Point curre23nt = Point(0, 0);
public:
	void Awake() override;
	void PhysicUpdate() override;
};

class SGE_EXPORT FilmicCamera : public Component
{
	Matrix4 transform;
public:
	void Awake() override 
	{
		SetEnabled(true);
	}

	const Matrix4& GetCameraTransform() 
	{
		return transform;
	}

	void FrameUpdate() override
	{
	    transform =  Matrix4::LookAt(
			GetGameObject()->GetPosition(), 
			GetGameObject()->GetPosition() + GetGameObject()->GetForward(),
			GetGameObject()->GetForward().Cross(GetGameObject()->GetRight())
		);
	}
};

class SGE_EXPORT Tools
{
public:
	static void SetMaterial(GameObject *cube, class Material* m);
    static void SetMaps(class GameObject* cube, class Texture* k, class Texture* n, class Texture* mt, class Texture* rt, class Texture* ao);
	static void SetMRAO(class GameObject* cube, float m, float r, float ao);
    static void ScaleAll(class GameObject* cube, float scale);
	static void ScaleAll(class GameObject* cube, const Vector3& vec);	
	static void RotateLocalAll(class GameObject* cube, const Quaternion& rot);
};

#endif // TOOLS_H
