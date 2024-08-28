// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Scene/GBufferTargets.hpp"
#include "Scene/SceneRenderTargets.hpp"

struct FrameRenderTargets
{
	GBuffer gbuffer;
	SceneRenderTargets sceneTargets;
	RenderTarget* userInterfaceTarget = nullptr;
};