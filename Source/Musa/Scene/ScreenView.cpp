// Copyright 2020, Nathan Blane

#include "ScreenView.hpp"
#include "Debugging/Assertion.hpp"
#include "Math/MatrixFunctions.hpp"
#include "Camera/Camera.h"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/UniformBuffers.h"

ScreenView::ScreenView(i32 screenWidth, i32 screenHeight)
	: width(screenWidth), height(screenHeight)
{
	Assert(screenWidth > 0);
	Assert(screenHeight > 0);
}

ScreenView::~ScreenView()
{
	GetGraphicsInterface().DestroyUniformBuffer(view.viewBuffer);
}

i32 ScreenView::GetScreenWidth() const
{
	return width;
}

i32 ScreenView::GetScreenHeight() const
{
	return height;
}

void ScreenView::AssociateCameraWithView(const Camera& camera)
{
	view.transforms.projectionMatrix = Math::ConstructPerspectiveMatrix(camera.GetFov(), camera.GetAspectRatio(), camera.GetNearPlane(), camera.GetFarPlane());
	view.transforms.viewMatrix = Math::ConstructViewMatrix(camera.GetPosition(), camera.GetForward(), camera.GetRight(), camera.GetUp());

	view.description.direction = camera.GetForward();
	view.description.origin = camera.GetPosition();
	view.description.viewport = ToRect(camera.GetCameraViewport());
	view.description.fieldOfView = camera.GetFov();
	view.description.aspectRatio = camera.GetAspectRatio();

	if (view.viewBuffer == nullptr)
	{
		view.viewBuffer = GetGraphicsInterface().CreateUniformBuffer(sizeof(ViewUniformBuffer));
	}

	ViewUniformBuffer props;
	props.viewTransform = view.transforms.viewMatrix;
	props.projectionTransform = view.transforms.projectionMatrix;
	props.viewPosition = view.description.origin;
	GetGraphicsInterface().PushBufferData(*view.viewBuffer, &props);
}
