// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

// TODO - move this enum to a better place
enum class LoadOperation : u8
{
	Load,
	Clear,
	DontCare
};

// TODO - move this enum to a better place
enum class StoreOperation : u8
{
	Store,
	DontCare
};

#define COLOR_OP_MASK 2
#define CREATE_COLOR_OP(LoadOp, StoreOp) (((u8)LoadOperation::LoadOp << COLOR_OP_MASK) | ((u8)StoreOperation::StoreOp))
// TODO - These ops aren't necessarily for Color Targets. Should they be RenderTarget operations?
enum class ColorTargetOperations : u8
{
	DontLoad_DontStore = CREATE_COLOR_OP(DontCare, DontCare),
	DontLoad_Store = CREATE_COLOR_OP(DontCare, Store),
	Load_DontStore = CREATE_COLOR_OP(Load, DontCare),
	Load_Store = CREATE_COLOR_OP(Load, Store),
	Clear_DontStore = CREATE_COLOR_OP(Clear, DontCare),
	Clear_Store = CREATE_COLOR_OP(Clear, Store)
};

constexpr LoadOperation GetLoadOperation(ColorTargetOperations op)
{
	return (LoadOperation)((u8)op >> COLOR_OP_MASK);
}

constexpr StoreOperation GetStoreOperation(ColorTargetOperations op)
{
	return (StoreOperation)((u8)op & ((1 << COLOR_OP_MASK) - 1));
}

constexpr ColorTargetOperations CreateColorTargetOperations(LoadOperation loadOp, StoreOperation storeOp)
{
	return (ColorTargetOperations)(((u8)loadOp << COLOR_OP_MASK) | ((u8)storeOp));
}

#undef COLOR_OP_MASK
#undef CREATE_COLOR_OP

#define DEPTH_OP_MASK 4
#define CREATE_DEPTH_OP(Depth, Stencil) (((u8)ColorTargetOperations::Depth << DEPTH_OP_MASK) | ((u8)ColorTargetOperations::Stencil))
enum class DepthStencilTargetOperations : u8
{
	DontLoad_DontStore = CREATE_DEPTH_OP(DontLoad_DontStore, DontLoad_DontStore),
	LoadDepthStencil_StoreDepthStencil = CREATE_DEPTH_OP(Load_Store, Load_Store),
	LoadDepthNotStencil_StoreDepthNotStencil = CREATE_DEPTH_OP(Load_Store, DontLoad_DontStore)
};

constexpr ColorTargetOperations GetDepthOperation(DepthStencilTargetOperations depthOp)
{
	return (ColorTargetOperations)((u8)depthOp >> DEPTH_OP_MASK);
}

constexpr ColorTargetOperations GetStencilOperation(DepthStencilTargetOperations stencilOp)
{
	return (ColorTargetOperations)((u8)stencilOp & ((1 << DEPTH_OP_MASK) - 1));
}

constexpr DepthStencilTargetOperations CreateDepthStencilOperations(ColorTargetOperations depthOp, ColorTargetOperations stencilOp)
{
	return (DepthStencilTargetOperations)(((u8)depthOp << DEPTH_OP_MASK) | ((u8)stencilOp));
}

#undef DEPTH_OP_MASK
#undef CREATE_DEPTH_OP

enum class CompareOperation : u8
{
	None,
	Equal,
	NotEqual,
	Less,
	LessThanOrEqual,
	Greater,
	GreaterThanOrEqual,
	Always
};

enum class StencilOperation : u8
{
	Keep,
	Zero,
	Replace,
	Invert,
	IncrementAndClamp,
	DecrementAndClamp,
	IncrementAndWrap,
	DecrementAndWrap
};

enum class PrimitiveTopology : u32
{
	TriangleList,
	TriangleStrip,
	TriangleFan,
	LineList,
	LineStrip,
	PointList
};

enum class CullingMode : u32
{
	None,
	Back,
	Front,
	FrontAndBack
};

enum class ShadingModel : u32
{
	Lit,
	Unlit
};

enum class FillMode : u32
{
	Full,
	Wireframe,
	Point
};

enum BlendMode : u32
{
	Opaque,
	Transparent
};

enum class ColorMask : u8
{
	Red = 0x1,
	Green = 0x2,
	Blue = 0x4,
	Alpha = 0x8,

	RGB = Red | Green | Blue,
	RGBA = Red | Green | Blue | Alpha
};

enum class BlendOperation : u8
{
	None,
	Add,
	Subtract,
};

enum class BlendFactor : u8
{
	Zero,
	One,
	SrcColor,
	OneMinusSrcColor,
	DstColor,
	OneMinusDstColor,
	SrcAlpha,
	OneMinusSrcAlpha,
	DstAlpha,
	OneMinusDstAlpha,
	ConstColor,
	OneMinusConstColor,
	ConstAlpha,
	OneMinusConstAlpha
};

inline ColorMask operator|(ColorMask e0, ColorMask e1)
{
	return (ColorMask)((u32)e0 | (u32)e1);
}

enum class SamplerAddressMode : u8
{
	Repeat,
	Clamp,
	Mirror
};

enum class SamplerFilter : u8
{
	Nearest,
	Linear
};

enum class SamplerMipmapMode : u8
{
	Nearest,
	Linear
};


