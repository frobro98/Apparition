#pragma once

#include "CameraObject.h"

class Camera;

class HorizontalOrbitCamera : public CameraObject
{
public:
	HorizontalOrbitCamera(const char* camName);
	HorizontalOrbitCamera(i32 width, i32 height);
	~HorizontalOrbitCamera();

	void SetRotateSpeed(float rotSpeed);

	virtual void Update(float tick) override;

private:
	f32 rotationSpeed;
};

