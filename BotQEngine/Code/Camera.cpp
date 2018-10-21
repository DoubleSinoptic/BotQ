#include "Camera.h"

Camera::Camera()
{

}

Camera::~Camera()
{

}

void Camera::SetupPerspective(float fov, float aspect, float znear, float zfar)
{
	mProjection = Matrix4::Perspective(fov, aspect, znear, zfar);
}

void Camera::SetupOrtho(int w, int h)
{
	mProjection = Matrix4::Identity();
	
}

Matrix4 Camera::GetProgectionMatrix() const
{
	return mProjection;
}

void Camera::SetProgectionMatrix(const Matrix4& mat)
{
	mProjection = mat;
}

Matrix4 Camera::GetLookMatrixMatrix() const
{
	return mLookMatrix;
}

void Camera::SetLookMatrixMatrix(const Matrix4& mat)
{
	mLookMatrix = mat;
}

void Camera::SetThisCurrent()
{
}
