// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Rect.hpp"
#include "Math/Vector4.hpp"
#include "Math/Matrix4.hpp"
#include "BasicTypes/Color.hpp"
#include "BasicTypes/Extents.hpp"

class Camera final
{
public:
	Camera() = default;

	void SetPerspective(float fovDeg, float aspectRatio, float nearPlane, float farPlane);
	void SetViewport(const Recti& vp);
	void SetOrientationAndPosition(const Vector4& lookAt, const Vector4& pos, const Vector4& up);

	Vector4 GetPosition() const;
	Vector4 GetForward() const;
	Vector4 GetUp() const;
	Vector4 GetRight() const;
	Vector4 GetLookAt() const;

	// TODO - This most definitely should be somewhere else, like a Window class
	int GetScreenWidth() const;
	int GetScreenHeight() const;

	float GetFov() const;
	void SetFov(float fovDeg);

	float GetAspectRatio() const;
	void SetAspectRatio(float aspect);

	float GetNearPlane() const;
	void SetNearPlane(float near);

	float GetFarPlane() const;
	void SetFarPlane(float far);

	Recti GetCameraViewport() const;

private:
	Matrix4 projection;
	Matrix4 view;

	// view matrix
	Vector4 viewRight;
	Vector4 viewUp;
	Vector4 viewForward;
	
	Vector4 position;
	Vector4 lookAt;

	Recti viewport;

	Color clearColor{ Color::Black() };

	float nearPlane = 0;
	float farPlane = 1;
	float fov = 90;
	float aspectRatio = 0;

};