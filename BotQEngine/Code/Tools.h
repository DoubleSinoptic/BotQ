#ifndef TOOLS_H
#define TOOLS_H

#include "Component.h"

class SGE_EXPORT FixedRotation : public Component
{
public:
	void Awake() override;
    void PhysicUpdate() override;
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
