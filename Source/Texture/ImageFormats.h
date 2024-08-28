// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

enum class ImageFormat : u32
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
	Invalid
};

enum class CompressionFormat : u32
{
	Invalid,
	BC1,
	BC3,
	BC7
};

constexpr const char* CompressionFormatString(CompressionFormat format)
{
	switch (format)
	{
		case CompressionFormat::BC1:
			return "BC1";
		case CompressionFormat::BC3:
			return "BC3";
		case CompressionFormat::BC7:
			return "BC7";
		case CompressionFormat::Invalid:
		default:
			return "Invalid";
	}
}