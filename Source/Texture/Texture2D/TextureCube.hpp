#pragma once

#include "PlatformDefinitions.h"
#include "Containers/DynamicArray.hpp"
#include "String/String.h"
#include "ImageFormats.h"
#include "Texture2D/TextureUtilities.hpp"

// NOTE - Layers, for vulkan, must be in this order to be interpreted correctly:
//    ---->>>  +X, -X, +Y, -Y, +Z, -Z
struct TextureLayer
{
	DynamicArray<uint8> layerData;
	uint32 width = 0; 
	uint32 height = 0;
};

// TODO - Probably need some sort of inheritance-like behavior for 2D, 3D, Cube, etc. texture types
struct TextureCube
{
	static constexpr uint32 CubeMapSideCount = 6;
	String name;
	TextureLayer layers[CubeMapSideCount];
	ImageFormat format = ImageFormat::Invalid;

	uint32 GetImageSize() const
	{
		const uint32 formatInBytes = FormatInBytes(format);
		uint32 imageSize = 0;
		for (uint32 i = 0; i < CubeMapSideCount; ++i)
		{
			imageSize += layers[i].width * layers[i].height * formatInBytes;
		}
		return imageSize;
	}

	// TODO - This belongs more on the gpu side of things. Deals with sampler info
	int32 wrapS;
	int32 wrapT;
	int32 minFilter;
	int32 magFilter;
};