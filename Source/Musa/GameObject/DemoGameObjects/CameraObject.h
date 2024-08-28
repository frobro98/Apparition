#pragma once

#include "GameObject/GameObject.h"

class Camera;

class CameraObject : public GameObject
{
public:
	CameraObject(const char* camName);
	//CameraObject(const char* cameraName, int32 width, int32 height);
	virtual ~CameraObject() = default;

	void SetCamera();
	virtual void Update(float tick) override;
	virtual void ShowDebugVolume(bool show) override;

	Camera* GetCamera() const { return camera; }

	void SetupGodCam();
	void MoveForward(int32 mod);
	void MoveRight(int32 mod);
	void MoveUp(int32 mod);

	void LookAtAdjust(float changeX, float changeY);

protected:
	virtual void TransformDebugVolume() override;

protected:
	Vector lookat;
	const char* cameraName;
	Camera* camera;
};