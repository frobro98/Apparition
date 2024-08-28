// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Color.hpp"
#include "BasicTypes/UniquePtr.hpp"
#include "Math/Matrix4.hpp"
#include "Math/Rect.hpp"
#include "RenderPipeline/MaterialPrimitiveBatches.hpp"


struct ViewTransformations
{
	Matrix4 worldToView;
	Matrix4 viewToWorld;
	Matrix4 viewToClip;
	Matrix4 clipToView;
	Matrix4 worldToClip;
	Matrix4 clipToWorld;
};

// Currently only works correctly if there's one view into the scene. This is because of the culling pass.
// Culling a single view is easy to pull off, but if you have multiple views, one view may be drawing an entity
// while other views aren't. This can't be solved by the current ECS CullingSystem existing. There needs to be 
// a system that does culling for views in the scene that have been registered
struct RenderView
{
	// Actual view data (e.g. position, direction, matrices)
	ViewTransformations transforms;
	Vector4 position;
	Vector4 direction;
	NativeUniformBuffer* viewUniformBuffer = nullptr;
	

	Recti scaledViewport;
	Rectf normViewport;
	// View-culled material primitive batches
	MaterialPrimitiveBatches opaqueMeshBatches;
	// View-culled transparent material batches

	// Visible lights

	// Clear color
	Color32 clearColor;
};

