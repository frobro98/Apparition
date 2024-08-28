// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/MemoryBuffer.hpp"
#include "Path/Path.hpp"
#include "ImageFormats.h"

// Consider making this a suite of functions that load specific kinds of image files
class TextureImporter
{
public:
	TextureImporter() = default;
	virtual ~TextureImporter() = default;

	u32 GetWidth() const;
	u32 GetHeight() const;
	ImageFormat GetFormat() const;

	virtual void SetImportData(MemoryBuffer&& compressedData) = 0;

	bool IsValid() const;
	void Import();
	const MemoryBuffer& GetImportedPixelData() const;

protected:
	virtual void ProcessImport() = 0;

protected:
	MemoryBuffer importData;
	MemoryBuffer importedImageData;
	ImageFormat format = ImageFormat::Invalid;
	i32 width = 0;
	i32 height = 0;
	bool validData = false;
};