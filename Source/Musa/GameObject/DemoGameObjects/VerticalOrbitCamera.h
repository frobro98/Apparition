#pragma once

#include "CameraObject.h"

class VerticalOrbitCamera : public CameraObject
{
public:
	VerticalOrbitCamera(const char* camName);
	VerticalOrbitCamera(int32 width, int32 height);
	~VerticalOrbitCamera();

	void SetRotateSpeed(float rotSpeed);

	virtual void Update(float tick) override;

private:
	float32 rotationSpeed;
};
