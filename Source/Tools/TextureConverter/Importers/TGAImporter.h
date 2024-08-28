// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Texture/ImageFormats.h"
#include "Importers/TextureImporter.hpp"

#pragma pack(push, 1)
struct TGAFileHeader
{
	i8 idSize;			// Size of ID field that folows header
	i8 colorMapType;		// 0 = none, 1 = paletted
	i8 imageType;			// 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8 = rle
	u16 colorMapStart;	// First color map entry
	u16 colorMapLength;	// Number of colors
	u8 colorMapBits;		// bits per palette entry
	u16 xStart;			// image x origin
	u16 yStart;			// image y origin
	u16 width;			// width in pixels
	u16 height;			// height in pixels
	i8 bits;				// bits per pixel (8, 16, 24, 32)
	i8 descriptor;		// image descriptor
};
#pragma pack(pop)

class TGAImporter : public TextureImporter
{
public:
	TGAImporter();

	virtual void SetImportData(MemoryBuffer&& compressedData) override;

private:
	virtual void ProcessImport() override;
	void ProcessHeader(const MemoryBuffer& compressedData);
	void ModifyBGRImage();

private:
	TGAFileHeader tgaFile{};
	u32 bitDepth;
};