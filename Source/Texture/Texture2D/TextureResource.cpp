// Copyright 2020, Nathan Blane

#include "TextureResource.hpp"
#include "Texture.h"
#include "Graphics/ResourceBlob.hpp"
#include "Graphics/ResourceInitializationDescriptions.hpp"

static SamplerAddressMode GetSamplerAddressMode(TextureAddress mode)
{
	switch (mode)
	{
		case TextureAddress::Clamp:
			return SamplerAddressMode::Clamp;
		case TextureAddress::Repeat:
			return SamplerAddressMode::Repeat;
		case TextureAddress::Mirror:
			return SamplerAddressMode::Mirror;
	}
	return static_cast<SamplerAddressMode>(-1);
}

TextureResource::~TextureResource()
{
	GetGraphicsInterface().DestroyTexture(texResource);
}

void TextureResource::UpdateTextureResource(const Texture& tex)
{
	ResourceBlob blob = tex.ConstructBlobOfMipLevels();

	texResource = GetGraphicsInterface().CreateInitializedTexture2D(
		blob, tex.GetWidth(), tex.GetHeight(),
		tex.GetFormat(), tex.GetMipmapLevels(), TextureUsage::SampledResource);

	SamplerDescription desc = {
		tex.filter == TextureFilter::Bilinear ? SamplerFilter::Linear : SamplerFilter::Nearest,
		GetSamplerAddressMode(tex.addrMode), GetSamplerAddressMode(tex.addrMode),
		tex.mipMode == TextureMipMode::Linear ? SamplerMipmapMode::Linear : SamplerMipmapMode::Nearest
	};
	samplerResource = GetGraphicsInterface().CreateTextureSampler(desc);
}
