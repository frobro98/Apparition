// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Graphics/GraphicsAPIDefinitions.hpp"

enum class TextureFilter : u8
{
	Linear,
	Bilinear
};

inline SamplerFilter TextureToGraphicsFilter(TextureFilter filter)
{
	switch (filter)
	{
		case TextureFilter::Linear:
			return SamplerFilter::Nearest;
		case TextureFilter::Bilinear:
			return SamplerFilter::Linear;
		default:
			Assert(false);
			return SamplerFilter::Linear;
	}
}

enum class TextureAddress : u8
{
	Repeat,
	Clamp,
	Mirror
};

inline SamplerAddressMode TextureToGraphicsAddressMode(TextureAddress addrMode)
{
	switch (addrMode)
	{
		case TextureAddress::Repeat:
			return SamplerAddressMode::Repeat;
		case TextureAddress::Clamp:
			return SamplerAddressMode::Clamp;
		case TextureAddress::Mirror:
			return SamplerAddressMode::Mirror;
		default:
			Assert(false);
			return SamplerAddressMode::Clamp;
	}
}

enum class TextureMipMode : u8
{
	Nearest,
	Linear
};

inline SamplerMipmapMode TextureToGraphicsMipMode(TextureMipMode mipMode)
{
	switch (mipMode)
	{
		case TextureMipMode::Nearest:
			return SamplerMipmapMode::Nearest;
		case TextureMipMode::Linear:
			return SamplerMipmapMode::Linear;
		default:
			Assert(false);
			return SamplerMipmapMode::Linear;
	}
}