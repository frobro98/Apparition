// Copyright 2020, Nathan Blane

#include "TextureImporter.hpp"
#include "File/FileUtilities.hpp"

u32 TextureImporter::GetWidth() const
{
	return static_cast<u32>(width);
}

u32 TextureImporter::GetHeight() const
{
	return static_cast<u32>(height);
}

ImageFormat TextureImporter::GetFormat() const
{
	return format;
}

bool TextureImporter::IsValid() const
{
	return validData;
}

void TextureImporter::Import()
{
	ProcessImport();
}

const MemoryBuffer& TextureImporter::GetImportedPixelData() const
{
	return importedImageData;
}
