// Copyright 2020, Nathan Blane

#include "Debugging/Assertion.hpp"
#include "ImageFormats.h"
#include "TextureCompression.h"
#include "Utilities/MemoryUtilities.hpp"
#include "Texture2D/TextureUtilities.hpp"
WALL_WRN_PUSH
#include "ispc/ispc_texcomp.h"
WALL_WRN_POP

namespace Compression
{

namespace
{
constexpr u32 CompressonDenom = 4;
constexpr u32 ChannelsPerPixel = 4;

void ForceMipDimensionForCompression(MipmapLevel& level)
{
	u32 origWidth = level.width;
	u32 origHeight = level.height;
	u32 adjustedWidth = origWidth % CompressonDenom == 0 ? origWidth : ((origWidth + 3) / CompressonDenom);
	u32 adjustedHeight = origHeight % CompressonDenom == 0 ? origHeight : ((origHeight + 3) / CompressonDenom);

	if (adjustedWidth > origWidth || adjustedHeight > origHeight)
	{
		u32 dataSize = adjustedWidth * adjustedHeight * ChannelsPerPixel;
		u8* adjustedData = new u8[dataSize];
		const u8* src = level.mipData.GetData();
		u8* dst = adjustedData;
		const u32 origRowWidth = origWidth * ChannelsPerPixel;
		const u32 adjustedRowWidth = adjustedWidth * ChannelsPerPixel;
		for (u32 i = 0; i < origHeight; ++i)
		{
			// Copy row of pixels then move to next row
			Memcpy(dst, adjustedRowWidth, src, origRowWidth);
			src += origRowWidth;
			dst += adjustedRowWidth;
		}

		level.mipData = ResourceBlob(adjustedData, dataSize);
	}
}

void ForceBufferDimensionForCompression(Texture& outDescription)
{
	for (auto& level : outDescription.mipLevels)
	{
		ForceMipDimensionForCompression(level);
	}

}

rgba_surface FormatMipForISPC(MipmapLevel& level)
{
	rgba_surface surface = {};
	surface.ptr = level.mipData.GetData();
	surface.width = static_cast<i32>(level.width);
	surface.height = static_cast<i32>(level.height);
	surface.stride = static_cast<i32>(level.width * ChannelsPerPixel);
	return surface;
}

void PrepareMipsForCompression(ImageFormat currentFormat, DynamicArray<MipmapLevel>& mipLevels)
{
	if (currentFormat == ImageFormat::BGRA_8norm || currentFormat == ImageFormat::BGR_8u)
	{
		for (auto& level : mipLevels)
		{
			const u32 bytesPerPixel = FormatInBytes(currentFormat);
			u8* imageData = level.mipData.GetData();
			for (u32 i = 0; i < level.mipData.GetSize(); i += bytesPerPixel)
			{
				u8 tmp = imageData[i + 0];
				imageData[i + 0] = imageData[i + 2];
				imageData[i + 2] = tmp;
			}
		}
	}
}

void PerformCompression(Texture& texture, CompressionFormat compressionFormat)
{
	for (auto& level : texture.mipLevels)
	{
		rgba_surface surface = FormatMipForISPC(level);
		u8* compressedData;
		u32 compressedSize;
		u32 numBlocksX = level.width > 4 ? level.width / 4 : 1;
		u32 numBlocksY = level.height > 4 ? level.height / 4 : 1;
		switch (compressionFormat)
		{
			case CompressionFormat::BC1:
			{
				texture.format = ImageFormat::BC1;
				compressedSize = numBlocksX * numBlocksY * 8;
				compressedData = new u8[compressedSize];
				CompressBlocksBC1(&surface, compressedData);
			}break;
			case CompressionFormat::BC3:
			{
				texture.format = ImageFormat::BC3;
				compressedSize = numBlocksX * numBlocksY * 16;
				compressedData = new u8[compressedSize];
				CompressBlocksBC3(&surface, compressedData);
			}break;
			case CompressionFormat::BC7:
			{
				texture.format = ImageFormat::BC7;
				compressedSize = numBlocksX * numBlocksY * 16;
				compressedData = new u8[compressedSize];
				bc7_enc_settings settings;
				GetProfile_slow(&settings);
				CompressBlocksBC7(&surface, compressedData, &settings);
			}break;
			case CompressionFormat::Invalid:
			default:
			{
				compressedData = nullptr;
				compressedSize = 0;
				Assert(false);
			}
		}

		level.mipData = ResourceBlob(compressedData, compressedSize);
	}
}
}

void CompressForGPU(const Texture& inTexture, Texture& outTexture, CompressionFormat compressionFormat)
{
	outTexture.mipLevels = inTexture.mipLevels;

	ForceBufferDimensionForCompression(outTexture);
	PrepareMipsForCompression(inTexture.format, outTexture.mipLevels);
	PerformCompression(outTexture, compressionFormat);
}

}