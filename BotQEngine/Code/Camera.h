#pragma once
#include "GameObject.h"
#include "./Math/Matrix4.h"
class SGE_EXPORT Camera : public GameObject
{
	TYPED_OBJECT;
public:
	Camera();
	~Camera();

	void SetupPerspective(float fov, float aspect, float znear, float zfar);
	void SetupOrtho(int w, int h);

	REGISTER_PROPETRY(Matrix4, ProgectionMatrix)
	Matrix4 GetProgectionMatrix() const;
	void SetProgectionMatrix(const Matrix4& mat);

	REGISTER_PROPETRY(Matrix4, LookMatrixMatrix)
	Matrix4 GetLookMatrixMatrix() const;
	void SetLookMatrixMatrix(const Matrix4& mat);

	void SetThisCurrent();
private:
	Matrix4 mLookMatrix;
	Matrix4 mProjection;

};

