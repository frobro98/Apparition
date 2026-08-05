#pragma once

#include "BasicTypes/Intrinsics.hpp"

namespace Apparition
{
namespace ImageFormat
{
enum Type
{
	RGB_8norm,
	RGB_8u,
	RGB_16f,
	BGR_8norm,
	RGBA_8norm,
	RGBA_8u,
	RGBA_16f,
	BGRA_8norm,
	Gray_8norm,
	BC1,
	BC3,
	BC7,
	DS_32f_8u,
	DS_24f_8u,
	D_32f,
	Invalid,

	Count,

	Max = 0x7FFFFFFF
};
} // ImageFormat

// TODO - Make this a mask
namespace ImageAspect
{
enum Type
{
	Color,
	Depth,
	Stencil,
	DepthStencil
};
}

namespace ImageAccess
{
enum Type
{
	Undefined,
	Present,
	TransferSrc,
	TransferDst,
	ColorWrite,
	ColorRead,
	DepthStencilWrite,
	DepthStencilRead
};
} // ImageAccess

namespace ImageUsageFlagBits
{
enum Type
{
	TransferSrc = 1 << 0,
	TransferDst = 1 << 1,
	Sampled = 1 << 2,
	ColorAttachment = 1 << 3,
	DepthStencilAttachment = 1 << 4,

	Max = 0x7FFFFFFF
};
} // ImageUsageFlagBits
using ImageUsageFlags = u32;
} // Apparition