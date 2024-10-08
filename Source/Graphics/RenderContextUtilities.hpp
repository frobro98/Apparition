// Copyright 2020, Nathan Blane

#pragma once

// TODO - Not a great name for what its doing. Hopefully I'll get a better name later on?

#include "BasicTypes/UniquePtr.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/GraphicsResourceFlags.hpp"
#include "Graphics/ApparitionAPI.hpp"

class RenderContext;
struct NativeRenderTargets;

APPARITION_API void TransitionTargetsToRead(RenderContext& renderer, NativeRenderTargets& targets);
APPARITION_API void TransitionTargetsToWrite(RenderContext& renderer, NativeRenderTargets& targets);

APPARITION_API RenderTarget* CreateRenderTarget(
	ImageFormat format, 
	u32 width, u32 height,
	LoadOperation loadOp, StoreOperation storeOp,
	LoadOperation stencilLoadOp, StoreOperation stencilStoreOp,
	TextureUsage::Type usage
);

APPARITION_API void DestroyRenderTarget(RenderTarget* rt);

