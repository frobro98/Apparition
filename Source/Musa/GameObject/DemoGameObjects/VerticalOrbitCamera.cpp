#include "VerticalOrbitCamera.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Input/Input.hpp"

VerticalOrbitCamera::VerticalOrbitCamera(const char * camName)
	:CameraObject(camName),
	rotationSpeed(.01f)
{
}

// VerticalOrbitCamera::VerticalOrbitCamera(int32 width, int32 height)
// 	: CameraObject("VerticalOrbit", width, height),
// 	rotationSpeed(.01f)
// {
// }

VerticalOrbitCamera::~VerticalOrbitCamera()
{
}

void VerticalOrbitCamera::SetRotateSpeed(float rotSpeed)
{
	rotationSpeed = rotSpeed;
}

void VerticalOrbitCamera::Update(float tick)
{
	Vector up = camera->GetUp();
	Vector right = camera->GetRight();

	Vector pos = position;
	Vector lookAt = camera->GetLookAt();
	Vector upPos = pos + up;

	Matrix trans(TRANS, lookAt);
	Matrix negTrans(TRANS, -lookAt);
	Matrix rot;
	rot.Set(right, rotationSpeed);

	Matrix M = negTrans * rot * trans;

	upPos *= M;
	pos *= M;
	lookAt *= M;

	up = upPos - pos;
	camera->SetOrientationAndPosition(lookAt, pos, up);

	position = pos;

	CameraObject::Update(tick);
}
