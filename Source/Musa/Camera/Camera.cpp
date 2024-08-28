// Copyright 2020, Nathan Blane

#include "Camera.h"

#undef near
#undef far

void Camera::SetPerspective(float fovDeg, float aspect, float near, float far)
{
	aspectRatio = aspect;
	fov = fovDeg;
	nearPlane = near;
	farPlane = far;
}

void Camera::SetViewport(const Recti& vp)
{
	viewport = vp;
}

void Camera::SetOrientationAndPosition(const Vector4& inLookAt, const Vector4& pos, const Vector4& up)
{
	lookAt = inLookAt;
	position = pos;

	viewForward = lookAt - position;
	viewForward.Normalize();
	
	viewRight = viewForward.Cross(up);
	viewRight.Normalize();

	viewUp = viewRight.Cross(viewForward);
	viewUp.Normalize();
}

// void Camera::CalculatePlaneDimentions()
// {
// 	nearHeight = 2.f * Math::Tan(Math::DegreesToRadians(fov) * .5f) * nearPlane;
// 	nearWidth = nearHeight * aspectRatio;
// 	farHeight = 2.f * Math::Tan(Math::DegreesToRadians(fov) * .5f) * farPlane;
// 	farWidth = farHeight * aspectRatio;
// }

Vector4 Camera::GetPosition() const
{
	return position;
}

Vector4 Camera::GetForward() const
{
	return viewForward;
}

Vector4 Camera::GetUp() const
{
	return viewUp;
}

Vector4 Camera::GetRight() const
{
	return viewRight;
}

Vector4 Camera::GetLookAt() const
{
	return lookAt;
}

int Camera::GetScreenWidth() const
{
	return viewport.width;
}

int Camera::GetScreenHeight() const
{
	return viewport.height;
}

float Camera::GetFov() const
{
	return fov;
}

void Camera::SetFov(float fovDeg)
{
	fov = fovDeg;
}

float Camera::GetAspectRatio() const
{
	return aspectRatio;
}

void Camera::SetAspectRatio(float aspect)
{
	aspectRatio = aspect;
}

float Camera::GetNearPlane() const
{
	return nearPlane;
}

void Camera::SetNearPlane(float near)
{
	nearPlane = near;
}

float Camera::GetFarPlane() const
{
	return farPlane;
}

void Camera::SetFarPlane(float far)
{
	farPlane = far;
}

Recti Camera::GetCameraViewport() const
{
	return viewport;
}

