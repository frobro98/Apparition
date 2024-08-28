// Copyright 2020, Nathan Blane

#include "Importers/TGAImporter.h"
#include "Texture2D/Texture.h"

#include "Utilities/MemoryUtilities.hpp"

TGAImporter::TGAImporter()
	: TextureImporter()
{
}

void TGAImporter::SetImportData(MemoryBuffer&& data)
{
	ProcessHeader(data);
	importData = MOVE(data);
}

void TGAImporter::ProcessHeader(const MemoryBuffer& data)
{
	Memcpy(&tgaFile, sizeof(TGAFileHeader), data.GetData(), sizeof(TGAFileHeader));
	validData = tgaFile.bits == 24 || tgaFile.bits == 32;

	width = tgaFile.width;
	height = tgaFile.height;
	bitDepth = tgaFile.bits / 8u;
	if (validData)
	{
		format = bitDepth == 3 ? ImageFormat::BGR_8u : ImageFormat::BGRA_8norm;
	}
	else if (tgaFile.bits == 8)
	{
		//  *eFormat = GL_LUMINANCE;
		//  *iComponents = GL_LUMINANCE;
		Assert(false);
	}
	else
	{
		// If on the iPhone, TGA's are BGR, and the iPhone does not 
		// support BGR without alpha, but it does support RGB,
		// so a simple swizzle of the red and blue bytes will suffice.
		// For faster iPhone loads however, save your TGA's with an Alpha!
	}
}

void TGAImporter::ModifyBGRImage()
{
	u32 imageSize = width * height * bitDepth;
	u8* texDataPtr = importedImageData.GetData();
	// Could be uint32 to allow always having an alpha
	constexpr size_t tgaHeaderSize = sizeof(TGAFileHeader);
	for (u32 i = 0; i < imageSize; ++i)
	{
		// Blue
		*texDataPtr = *importData.Offset(tgaHeaderSize + i);
		++i, ++texDataPtr;

		// Green
		*texDataPtr = *importData.Offset(tgaHeaderSize + i);
		++i, ++texDataPtr;

		// Red
		*texDataPtr = *importData.Offset(tgaHeaderSize + i);
		++texDataPtr;

		// Alpha
		*texDataPtr = 0xff;
		++texDataPtr;
	}
	format = ImageFormat::BGRA_8norm;
}

void TGAImporter::ProcessImport()
{
	constexpr u32 correctBitDepth = 4;
	const u32 textureSize = width * height * correctBitDepth;
	importedImageData.IncreaseSize(textureSize);
	// Vulkan doesn't seem to support 24 bit textures, so here we are
	if (bitDepth == 3 && tgaFile.imageType < 8) // BGR no alpha
	{
		ModifyBGRImage();
	}
	else if (tgaFile.imageType < 8)
	{
		Memcpy(importedImageData.GetData(), textureSize, importData.GetData() + sizeof(TGAFileHeader), textureSize);
	}
	else // RLE compression not currently supported!!
	{
		// TODO - implement RLE compression for tgas
		Assert(false);
	}
}
