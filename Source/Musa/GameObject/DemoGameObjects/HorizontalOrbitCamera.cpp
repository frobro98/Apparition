#include "HorizontalOrbitCamera.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"


HorizontalOrbitCamera::HorizontalOrbitCamera(const char* camName)
	:CameraObject(camName),
	rotationSpeed(.01f)
{
}

// HorizontalOrbitCamera::HorizontalOrbitCamera(int32 width, int32 height)
// 	: CameraObject("HorizontalOrbit", width, height),
// 	rotationSpeed(.01f)
// {
// }

HorizontalOrbitCamera::~HorizontalOrbitCamera()
{
	//CameraManager::RemoveCamera("HorizontalOrbit");
}

void HorizontalOrbitCamera::SetRotateSpeed(float rotSpeed)
{
	rotationSpeed = rotSpeed;
}

void HorizontalOrbitCamera::Update(float tick)
{
	Vector up = camera->GetUp();
	Vector right = camera->GetRight();

	Vector pos = position;
	Vector lookAt = camera->GetLookAt();
	Vector upPos = pos + up;

	Matrix trans(TRANS, lookAt);
	Matrix negTrans(TRANS, -lookAt);
	Matrix rot;
	rot.Set(up, rotationSpeed);

	Matrix M = negTrans * rot * trans;

	upPos *= M;
	pos *= M;
	lookAt *= M;

	up = upPos - pos;
	camera->SetOrientationAndPosition(lookAt, pos, up);

	position = pos;

	CameraObject::Update(tick);
}
