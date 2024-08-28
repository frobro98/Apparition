// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
#include "Containers/DynamicArray.hpp"
#include "Math/Vector4.hpp"
#include "Math/Matrix4.hpp"
#include "Math/Rect.hpp"
#include "Graphics/RenderPassAttachments.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"

class Camera;
struct NativeVertexShader;
struct NativeFragmentShader;

struct ViewDescription
{
	Vector4 origin;
	Vector4 direction;
	Rectf viewport;
	f32 fieldOfView = 0;
	f32 aspectRatio = 0;
};

struct ViewTransforms
{
	Matrix4 viewMatrix;
	Matrix4 projectionMatrix;
};

// TODO - The view should be produced by the main Viewport somehow
struct View
{
	ViewDescription description;
	ViewTransforms transforms;
	NativeUniformBuffer* viewBuffer;
};

// TODO - Determine if this and the Viewport class are similar and try and consolidate if they are
// Represents the entire screen's view into the scene.
// Can be broken up into multiple views
class ScreenView
{
public:
	ScreenView(i32 screenWidth, i32 screenHeight);
	~ScreenView();

	i32 GetScreenWidth() const;
	i32 GetScreenHeight() const;

	void AssociateCameraWithView(const Camera& cam);

public:
	View view;

private:
	NativeVertexShader* screenVertexShader = nullptr;
	NativeFragmentShader* screenFragmentShader = nullptr;
	i32 width;
	i32 height;
};