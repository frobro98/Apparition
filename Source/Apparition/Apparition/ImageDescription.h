#pragma once

#include "BasicTypes/Intrinsics.hpp"

namespace Apparition::ImageFormat
{
enum Type
{
	RGB_8norm,
	RGB_8u,
	RGB_16f,
	BGR_8u,
	RGBA_8norm,
	RGBA_8u,
	RGBA_16f,
	BGRA_8norm,
	Gray_8u,
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
}
