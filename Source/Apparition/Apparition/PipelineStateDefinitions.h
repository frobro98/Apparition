#pragma once

#include "BasicTypes/Intrinsics.hpp"

////////////////////////
// Vertex Input State
////////////////////////
enum class AptnVertexInputFormat
{
	F32_1,
	F32_2,
	F32_3,
	F32_4,
	U32,

	MAX = 0x7FFFFFFF
};

enum class AptnVertexInputRate
{
	Vertex,
	Instance,

	MAX = 0x7FFFFFFF
};

////////////////////////
// Input Assembly State
////////////////////////

enum class AptnPrimitiveTopology
{
	TriangleList,
	TriangleStrip,
	TriangleFan,
	LineList,
	LineStrip,
	PointList,

	MAX = 0x7FFFFFFF
};


////////////////////////
// Rasterization State
////////////////////////

enum class AptnFillMode
{
	Full,
	Wireframe,
	Point,
	MAX = 0x7FFFFFFF
};

enum class AptnCullMode
{
	None,
	Back,
	Front,
	FrontAndBack,

	MAX = 0x7FFFFFFF
};

// TODO - rename this to something more straightforward
enum class AptnFrontFace
{
	Clockwise,
	CounterClockwise,

	MAX = 0x7FFFFFFF
};

////////////////////////
// Depth/Stencil State
////////////////////////

enum class AptnCompareOperation
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

enum class AptnStencilOperation
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

////////////////////////
// Multisampling State
////////////////////////

enum class AptnSampleCountFlags
{
	SampleCount_1 = 1 << 0,
	SampleCount_2 = 1 << 1,
	SampleCount_4 = 1 << 2,
	SampleCount_8 = 1 << 3,
	SampleCount_16 = 1 << 4,
	SampleCount_32 = 1 << 5,
	SampleCount_64 = 1 << 6,

	MAX = 0x7FFFFFFF
};
ENUM_CLASS_OPERATORS(AptnSampleCountFlags);

////////////////////////
// Color Blend State
////////////////////////

enum class AptnBlendMode
{
	Opaque,
	Transparent,

	MAX = 0x7FFFFFFF
};

enum class AptnColorComponentFlags
{
    Red = 1 << 0,
    Green = 1 << 1,
    Blue = 1 << 2,
    Alpha = 1 << 3,

    RGB = Red | Green | Blue,
    RGBA = Red | Green | Blue | Alpha
};
ENUM_CLASS_OPERATORS(AptnColorComponentFlags);

enum class AptnBlendOperation
{
    None,
    Add,
    Subtract
};

enum class AptnBlendFactor
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
