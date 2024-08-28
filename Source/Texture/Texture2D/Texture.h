// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
#include "Containers/DynamicArray.hpp"
#include "String/String.h"
#include "Texture/ImageFormats.h"
#include "Texture/Texture2D/MipmapLevel.hpp"
#include "Texture/TextureSampleSettings.hpp"
#include "Texture/TextureAPI.hpp"
#include "Texture/Texture2D/TextureResource.hpp"

class SerializeBase;
class DeserializeBase;

class TEX_API Texture
{
public:
	Texture() = default;
	Texture(u8 r, u8 g, u8 b, u8 a);

	void UpdateNativeResources();
	size_t GetTotalBytes() const;

	// TODO - If the data is required to be in some form, which it is currently in mip map form, it should
	// be given in that form. Updating individual mip maps should be allowed by the graphics api...
	ResourceBlob ConstructBlobOfMipLevels() const;

	forceinline TextureResource& GetResource() const
	{
		return *gpuResource;
	}

	forceinline u32 GetWidth() const
	{
		// TODO - The width of the mip levels can be computed, so the texture dimensions should go into the texture object itself...
		if (mipLevels.Size() > 0)
		{
			return mipLevels[0].width;
		}
		return 0;
	}

	forceinline u32 GetHeight() const
	{
		// TODO - The width of the mip levels can be computed, so the texture dimensions should go into the texture object itself...
		if (mipLevels.Size() > 0)
		{
			return mipLevels[0].height;
		}
		return 0;
	}

	forceinline ImageFormat GetFormat() const
	{
		return format;
	}

	forceinline u32 GetMipmapLevels() const
	{
		return mipLevels.Size();
	}

public:
	// TODO - This should be serialized, so there's no need to be able to set this...
	String name;
	TextureFilter filter = TextureFilter::Bilinear;
	TextureAddress addrMode = TextureAddress::Repeat;
	TextureMipMode mipMode = TextureMipMode::Linear;

	// TODO - Having access to this shouldn't be allowed at this level. The only reason this is 
	// required is because of the texture converter. Eventually, there needs to be a data structure
	// that stores this information as well as some other info.
	DynamicArray<MipmapLevel> mipLevels;
	// TODO - This information should be in the data structure with mipmap levels
	ImageFormat format = ImageFormat::Invalid;
private:
	friend TEX_API void Serialize(SerializeBase& ser, const Texture& tex);
	friend TEX_API void Deserialize(DeserializeBase& ser, Texture& tex);

private:
	UniquePtr<TextureResource> gpuResource;
};

TEX_API Texture* WhiteTexture();
TEX_API Texture* BlackTexture();
