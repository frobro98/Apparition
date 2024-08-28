// Copyright 2020, Nathan Blane

#include "CameraManager.h"
#include "Camera.h"
#include "String/CStringUtilities.hpp"

void CameraManager::AddCamera(Camera* camera, const char* cameraName)
{
	cameras.Add(String(cameraName), camera);
}

void CameraManager::RemoveCamera(const char* cameraName)
{
	cameras.Remove(cameraName);
}

Camera* CameraManager::FindCamera(const char * cameraName)
{
	Camera* cam = nullptr;
	if (cameras.TryFind(cameraName, cam))
	{
		return cam;
	}
	return nullptr;
}

void CameraManager::SetActiveCamera(const char * cameraName)
{
	Camera* camera = FindCamera(cameraName);
	Assert(camera != nullptr);
	activeCamera = camera;
}

Camera* CameraManager::GetActiveCamera()
{
	return activeCamera;
}

void CameraManager::Resize(int width, int height, float aspectRatio)
{
	for (auto& camera : cameras)
	{
		Camera& cam = *camera.second;
		Recti viewport = { 0, 0, width, height };
		cam.SetViewport(viewport);
		cam.SetAspectRatio(aspectRatio);
	}
}

CameraManager& GetCameraManager()
{
	static CameraManager cmInstance;
	return cmInstance;
}
