#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/EnumUtils.h"

enum class AptnImageFormat
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

enum class AptnImageAspectFlags
{
	Color = 1 << 0,
	Depth = 1 << 1,
	Stencil = 1 << 2,
	DepthStencil = Depth | Stencil,

	Max = 0x7FFFFFFF
};
ENUM_CLASS_OPERATORS(AptnImageAspectFlags);

enum class AptnImageAccess
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

enum class AptnImageUsageFlags
{
	TransferSrc = 1 << 0,
	TransferDst = 1 << 1,
	Sampled = 1 << 2,
	ColorAttachment = 1 << 3,
	DepthStencilAttachment = 1 << 4,

	Max = 0x7FFFFFFF
};
ENUM_CLASS_OPERATORS(AptnImageUsageFlags);