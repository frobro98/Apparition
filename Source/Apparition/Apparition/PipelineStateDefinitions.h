#pragma once

#include "BasicTypes/Intrinsics.hpp"

////////////////////////
// Vertex Input State
////////////////////////
namespace AptnVertexInputFormat
{
enum Type
{
	F32_1,
	F32_2,
	F32_3,
	F32_4,
	U32,

	MAX = 0x7FFFFFFF
};
}// VertexInputFormat

namespace AptnVertexInputRate
{
enum Type
{
	Vertex,
	Instance,

	MAX = 0x7FFFFFFF
};
}// VertexInputRate

////////////////////////
// Input Assembly State
////////////////////////

namespace AptnPrimitiveTopology
{
enum Type
{
	TriangleList,
	TriangleStrip,
	TriangleFan,
	LineList,
	LineStrip,
	PointList,

	MAX = 0x7FFFFFFF
};
}// PrimitiveTopology


////////////////////////
// Rasterization State
////////////////////////

namespace AptnFillMode
{
enum Type
{
	Full,
	Wireframe,
	Point,
	MAX = 0x7FFFFFFF
};
}// FillMode

namespace AptnCullMode
{
enum Type
{
	None,
	Back,
	Front,
	FrontAndBack,

	MAX = 0x7FFFFFFF
};
}// CullMode

// TODO - rename this to something more straightforward
namespace AptnFrontFace
{
enum Type
{
	Clockwise,
	CounterClockwise,

	MAX = 0x7FFFFFFF
};
}

////////////////////////
// Depth/Stencil State
////////////////////////

namespace AptnCompareOperation
{
enum Type
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
}// CompareOperation

namespace AptnStencilOperation
{
enum Type
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
}// StencilOperation

////////////////////////
// Multisampling State
////////////////////////

namespace AptnSampleCountFlagBits
{
enum Type
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
} // SampleCountFlagBits
using AptnSampleCountFlags = u32;

////////////////////////
// Color Blend State
////////////////////////

namespace AptnBlendMode
{
enum Type
{
	Opaque,
	Transparent,

	MAX = 0x7FFFFFFF
};
}// BlendMode

namespace AptnColorComponentFlagBits
{
enum Type
{
    Red = 1 << 0,
    Green = 1 << 1,
    Blue = 1 << 2,
    Alpha = 1 << 3,

    RGB = Red | Green | Blue,
    RGBA = Red | Green | Blue | Alpha
};
} // ColorComponentFlagBits
using AptnColorComponentFlags = u32;

namespace AptnBlendOperation
{
enum Type
{
    None,
    Add,
    Subtract
};
}// BlendOperation

namespace AptnBlendFactor
{
enum Type
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
}// BlendFactor
