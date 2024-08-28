// Copyright 2020, Nathan Blane

#include "Texture.h"
#include "TextureUtilities.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"

static ImageFormat DeserializeImageFormat(DeserializeBase& ser)
{
	u32 formatInt;
	Deserialize(ser, formatInt);
	return static_cast<ImageFormat>(formatInt);
}


Texture::Texture(u8 r, u8 g, u8 b, u8 a)
{
	u8* pixel = new u8[4]{ b, g, r, a };
	ResourceBlob blob(pixel, 4);
	MipmapLevel m;
	m.mipData = MOVE(blob);
	m.width = 1;
	m.height = 1;

	mipLevels.Add(MOVE(m));
	format = ImageFormat::BGRA_8norm;

	UpdateNativeResources();
}

void Texture::UpdateNativeResources()
{
	if (!gpuResource)
	{
		gpuResource = MakeUnique<TextureResource>();
	}

	gpuResource->UpdateTextureResource(*this);
}

size_t Texture::GetTotalBytes() const
{
	size_t totalSize = 0;
	for (const auto& level : mipLevels)
	{
		totalSize += level.mipData.GetSize();
	}
	return totalSize;
}

ResourceBlob Texture::ConstructBlobOfMipLevels() const
{
	size_t totalSize = 0;
	for (const auto& level : mipLevels)
	{
		totalSize += level.mipData.GetSize();
	}
	
	u8* data = (u8*)Memory::Malloc(totalSize);
	u8* iter = data;
	for (const auto& level : mipLevels)
	{
		Memcpy(iter, level.mipData.GetSize(), level.mipData.GetData(), level.mipData.GetSize());
		iter += level.mipData.GetSize();
	}

	return ResourceBlob(data, totalSize);
}

void Serialize(SerializeBase& ser, const Texture& tex)
{
	Serialize(ser, tex.name);
	Serialize(ser, static_cast<u32>(tex.format));
	Serialize(ser, tex.mipLevels);
}

void Deserialize(DeserializeBase& ser, Texture& tex)
{
	Deserialize(ser, tex.name);
	tex.format = DeserializeImageFormat(ser);
	Deserialize(ser, tex.mipLevels);
}

Texture* WhiteTexture()
{
	static Texture* const whiteTexture = new Texture(255, 255, 255, 255);
	return whiteTexture;
}

Texture* BlackTexture()
{
	static Texture* const blackTexture = new Texture(0, 0, 0, 255);
	return blackTexture;
}

