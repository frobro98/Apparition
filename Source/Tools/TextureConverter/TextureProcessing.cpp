// Copyright 2020, Nathan Blane

#include "TextureProcessing.hpp"
#include "BasicTypes/Color.hpp"
#include "Containers/Map.h"
#include "Path/Path.hpp"
#include "File/FileUtilities.hpp"
#include "Importers/JPEGImporter.hpp"
#include "Importers/TGAImporter.h"
#include "Importers/PNGImporter.hpp"
#include "Importers/BMPImporter.hpp"
#include "Importers/TextureCompression.h"
#include "Math/MathFunctions.hpp"

namespace
{
constexpr const char* supportedImageTypes[] =
{
	"jpg",
	"tga",
	"png",
	"bmp"
};

Map<String, TextureImporter*> importerMap = {
	{"jpg", new JPEGImporter},
	{"tga", new TGAImporter},
	{"png", new PNGImporter},
	{"bmp", new BMPImporter}
};

Texture ConstructTexture(TextureImporter& importer)
{
	Texture tex;
	MipmapLevel baseLevel = {};
	baseLevel.width = importer.GetWidth();
	baseLevel.height = importer.GetHeight();
	const MemoryBuffer& pixels = importer.GetImportedPixelData();
	u8* data = new u8[pixels.Size()];
	Memcpy(data, pixels.Size(), pixels.GetData(), pixels.Size());
	baseLevel.mipData = ResourceBlob(data, pixels.Size());
	tex.mipLevels.Add(MOVE(baseLevel));
	tex.format = importer.GetFormat();
	return tex;
}

}

bool IsSupportedTexture(const Path& texturePath)
{
	if (texturePath.DoesFileExist())
	{
		String extension = texturePath.GetFileExtension();
		for (auto* imageType : supportedImageTypes)
		{
			if (extension == imageType)
			{
				return true;
			}
		}
	}

	return false;
}

Texture ProcessImageFile(const Path& filePath, CompressionFormat format)
{
	Assert(filePath.DoesFileExist());

	MemoryBuffer textureData = LoadFileToMemory(filePath.GetString());
	String extension = filePath.GetFileExtension();
	
	TextureImporter* importer = importerMap[*extension];
	importer->SetImportData(MOVE(textureData));
	if (importer->IsValid())
	{
		importer->Import();
	}

	String textureName = filePath.GetFileNameWithoutExtension();
	Texture originalTexture = ConstructTexture(*importer);
	Texture compressedTexture;
	if (format != CompressionFormat::Invalid)
	{
		Compression::CompressForGPU(originalTexture, compressedTexture, format);
		compressedTexture.name = MOVE(textureName);
		return compressedTexture;
	}

	originalTexture.name = MOVE(textureName);
	return originalTexture;
}
