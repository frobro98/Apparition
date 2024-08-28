// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
#include "Graphics/RenderTarget.hpp"

struct SceneRenderTargets
{
	RenderTarget* sceneColorTexture;
	RenderTarget* depthTexture;
};

