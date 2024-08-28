// Copyright 2020, Nathan Blane

#include <stdio.h>

#include "BasicTypes/Intrinsics.hpp"
#include "TextureProcessing.hpp"
#include "Archiver/FileSerializer.hpp"
#include "MipmapGeneration.hpp"
#include "BasicTypes/Color.hpp"
#include "Math/MathFunctions.hpp"

// TODO - Probably are useful utilities for color manipulation
Color TintColor(const Color& color, const Color& tint)
{
	Color tintedColor = color;
	tintedColor.r += static_cast<u8>((color.r - tint.r) * .5f);
	tintedColor.g += static_cast<u8>((color.g - tint.g) * .5f);
	tintedColor.b += static_cast<u8>((color.b - tint.b) * .5f);
	return Color(tintedColor);
}

u8 TintValue(u8 val, u8 tint)
{
	float tintedValue = Math::Clamp((tint - val) * .5f, 0.f, 255.f);
	val = val + tintedValue > 255 ? 255u : val + static_cast<u8>(tintedValue);
	return val;
}

void TintMipMapColor(MipmapLevel& texture, const Color& tint)
{

	constexpr u32 numChannels = 4;
	const u32 rowWidth = numChannels * texture.width;
	u8* imageData = texture.mipData.GetData();
	for (u32 i = 0; i < texture.height; ++i)
	{
		for (u32 j = 0; j < rowWidth; j += 4)
		{
			imageData[(j + 0) + rowWidth * i] = TintValue(imageData[(j + 0) + rowWidth * i], tint.r);
			imageData[(j + 1) + rowWidth * i] = TintValue(imageData[(j + 1) + rowWidth * i], tint.g);
			imageData[(j + 2) + rowWidth * i] = TintValue(imageData[(j + 2) + rowWidth * i], tint.b);
		}
	}
}

int main(int argc, char* argv[])
{
	// -f: file path
	// -c: compression format
	// -genmipmaps
	if (argc < 2)
	{
		printf("Error: No arguments passed in. File name and compression type must be provided");
		return -1;
	}

	Path filePath;
	Path outputFilePath;
	CompressionFormat compressionFormat = CompressionFormat::Invalid;
	MipMapFilter filter = MipMapFilter::Box;
	bool generateMipMaps = false;
	for (i32 i = 1; i < argc; i += 2)
	{
		char* argType = argv[i];
		if (i + 1 == argc)
		{
			printf("Error: An argument must be followed by the value it represents!");
			return -1;
		}
		char* arg = argv[i + 1];
		if (strcmp(argType, "-f") == 0)
		{
			filePath = arg;
		}
		else if (strcmp(argType, "-o") == 0)
		{
			outputFilePath = arg; 
		}
		else if(strcmp(argType, "-c") == 0)
		{
			// Determine which supported compression schemes is chosen
			if (strcmp(arg, "bc7") == 0)
			{
				compressionFormat = CompressionFormat::BC7;
			}
			else if (strcmp(arg, "bc1") == 0)
			{
				compressionFormat = CompressionFormat::BC1;
			}
			else if (strcmp(arg, "bc3") == 0)
			{
				compressionFormat = CompressionFormat::BC3;
			}
			else
			{
				return -1;
			}
		}
		else if (strcmp(argType, "-genmips") == 0)
		{
			generateMipMaps = true;
			if (strcmp(arg, "kaiser") == 0)
			{
				filter = MipMapFilter::Kaiser;
			}
			else if (strcmp(arg, "fft") == 0)
			{
				filter = MipMapFilter::FFT;
			}
			else if (strcmp(arg, "box") != 0)
			{
				printf("Couldn't recognize the parameter %s\n", arg);
				return -1;
			}
		}
		else
		{
			printf("Unknown argument passed in: %s\n", argType);
			return -1;
		}
	}

	Texture texture;
	if (IsSupportedTexture(filePath))
	{
		texture = ProcessImageFile(filePath, compressionFormat);
	}
	else
	{
		printf("Image format of %s isn't supported!", filePath.GetString());
		return - 1;
	}

	if (generateMipMaps)
	{
		u32 maxGeneratedLevels = GetMaxMipMapLevels(texture.GetWidth(), texture.GetHeight()) - 1;
		GenerateMipMapLevels(texture, maxGeneratedLevels, filter);

		Color tint = Color::Green();
		for (u32 i = 1; i < texture.mipLevels.Size(); ++i)
		{
			TintMipMapColor(texture.mipLevels[i], tint);
			if (i == 1)
			{
				tint = Color::Red();
			}
			else if (i == 2)
			{
				tint = Color::Magenta();
			}
			else if (i == 3)
			{
				tint = Color::Red();
			}
			else if (i == 4)
			{
				tint = Color::Black();
			}
		}
	}

	String textureFileName;
// 	if (compressionFormat != CompressionFormat::Invalid)
// 	{
// 		textureFileName = texture.name + "_c" + ".tex";
// 	}
// 	else
	if(outputFilePath.GetString() == nullptr)
	{
		textureFileName = texture.name + ".tex";
	}
	else
	{
		textureFileName = outputFilePath.GetString();
	}

	Path textureFile(MOVE(textureFileName));
	// TODO - Add some sort of formatting functionality into my string utilities
	{

		FileSerializer fileSerializer(textureFile);
		Serialize(fileSerializer, texture);
		printf("Serialized out \"%s\" to %s\n", *texture.name, textureFile.GetString());
	}

	return 0;
}