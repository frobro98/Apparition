// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/Map.h"

class Camera;

class CameraManager
{
public:
	void AddCamera(Camera* camera, const char* cameraName);
	void RemoveCamera(const char* camera);
	Camera* FindCamera(const char* cameraName);
	
	void SetActiveCamera(const char* cameraName);
	Camera* GetActiveCamera();

	void Resize(int width, int height, float aspectRatio);

	friend CameraManager& GetCameraManager();

private:

	struct CameraNode
	{
		const char* cameraName = nullptr;
		Camera* camera = nullptr;
	};

	Map<String, Camera*> cameras;
	Camera* activeCamera = nullptr;
};

CameraManager& GetCameraManager();

