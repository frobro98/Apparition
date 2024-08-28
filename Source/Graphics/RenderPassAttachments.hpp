// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/FixedArray.hpp"
#include "BasicTypes/Extents.hpp"
#include "BasicTypes/Color.hpp"
#include "Utilities/HashFuncs.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "Texture/ImageFormats.h"
#include "Graphics/GraphicsAPIDefinitions.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"

constexpr u32 GBufferCount = 3;
constexpr u32 MaxColorTargetCount = 4;

enum class RenderTargetAccess : u32
{
	None,
	Read,
	Write
};

struct ColorDescription
{
	ImageFormat format = ImageFormat::Invalid;
	ColorTargetOperations op = ColorTargetOperations::DontLoad_DontStore;
	RenderTargetAccess access = RenderTargetAccess::None;
	u32 sampleCount = 0;
};

inline bool operator==(const ColorDescription& lhs, const ColorDescription& rhs)
{
	return lhs.format == rhs.format 
		&& lhs.op == rhs.op 
		&& lhs.access == rhs.access 
		&& lhs.sampleCount == rhs.sampleCount;
}

struct DepthStencilDescription
{
	ImageFormat format = ImageFormat::Invalid;
	DepthStencilTargetOperations op = DepthStencilTargetOperations::DontLoad_DontStore;
	RenderTargetAccess access = RenderTargetAccess::None;
	u32 sampleCount = 0;
};

inline bool operator==(const DepthStencilDescription& lhs, const DepthStencilDescription& rhs)
{
	return lhs.format == rhs.format
		&& lhs.op == rhs.op
		&& lhs.access == rhs.access
		&& lhs.sampleCount == rhs.sampleCount;
}

struct DepthStencilClearColor
{
	f32 clearDepth;
	u32 clearStencil;
};

// Allocated textures associated with a render texture
struct NativeRenderTargets
{
	FixedArray<const NativeTexture*, MaxColorTargetCount> colorTargets;
	const NativeTexture* depthTarget;
	Extents2D extents;
};

struct BeginRenderPassInfo
{
	FixedArray<ColorDescription, MaxColorTargetCount> colorAttachments;
	DepthStencilDescription depthAttachment = {};
	FixedArray<Color32, MaxColorTargetCount> clearColors;
	DepthStencilClearColor clearDS = { 1.f, 0 };
	NativeRenderTargets targets;
};


