// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
#include "Containers/FixedArray.hpp"
// TODO - This file needs to live in the Graphics library
#include "Texture/ImageFormats.h"
#include "Graphics/GraphicsAPIDefinitions.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Graphics/RenderPassAttachments.hpp"
#include "Graphics/ApparitionAPI.hpp"

struct RenderTarget
{
	NativeTexture* nativeTarget = nullptr;
	u32 width;
	u32 height;
	u32 sampleCount;
	ImageFormat format;
	LoadOperation load;
	LoadOperation stencilLoad;
	StoreOperation store;
	StoreOperation stencilStore;
};

APPARITION_API FixedArray<ColorDescription, MaxColorTargetCount> CreateColorTargetDescriptions(const FixedArray<const RenderTarget*, MaxColorTargetCount>& colorTargets, RenderTargetAccess access);
APPARITION_API DepthStencilDescription CreateDepthTargetDescription(const RenderTarget* depthTarget, RenderTargetAccess access);
APPARITION_API NativeRenderTargets CreateNativeRenderTargets(const FixedArray<const RenderTarget*, MaxColorTargetCount>& colorTargets, const RenderTarget* depthTarget);
