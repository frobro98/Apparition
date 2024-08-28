// Copyright 2020, Nathan Blane

// TODO - Remove this somehow
#include "Platform/PlatformDefinitions.h"
#include "PNGImporter.hpp"

#pragma warning(push)
#pragma warning(disable:4611) // setjmp usage
namespace
{
jmp_buf jmpBuf;

constexpr u32 PNGSignatureByteSize = sizeof(png_size_t);

static void PNGErrorFunction(png_structp png_ptr, png_const_charp error_msg)
{
	PNGImporter* importer = reinterpret_cast<PNGImporter*>(png_get_io_ptr(png_ptr));
	UNUSED(importer, error_msg);
	printf("%s\n", error_msg);
}

static void PNGWarningFunction(png_structp png_ptr, png_const_charp warn_msg)
{
	PNGImporter* importer = reinterpret_cast<PNGImporter*>(png_get_io_ptr(png_ptr));
	UNUSED(importer, warn_msg);
	printf("%s\n", warn_msg);
}
}


void PNGImporter::PNGReadFunc(png_structp png_ptr, png_bytep data, png_size_t size)
{
	PNGImporter* importer = reinterpret_cast<PNGImporter*>(png_get_io_ptr(png_ptr));
	Assert(importer->bufferReadLocation + size <= importer->importData.Size());
	MemoryBuffer& compData = importer->importData;
	Memcpy(data, size, compData.Offset(importer->bufferReadLocation), size);
	// TODO - x64: Figure out how to handle size_t
	importer->bufferReadLocation += (u32)size;
}

void PNGImporter::SetImportData(MemoryBuffer&& data)
{
	importData = MOVE(data);
	ProcessPNGHeader();
}

void PNGImporter::ProcessPNGHeader()
{
	validData = CheckPNGSignature();

	if (validData)
	{
		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, this, PNGErrorFunction, PNGWarningFunction);
		png_infop pngInfo = png_create_info_struct(png);

		png_set_read_fn(png, this, &PNGImporter::PNGReadFunc);
		png_read_info(png, pngInfo);

		width = static_cast<i32>(pngInfo->width);
		height = static_cast<i32>(pngInfo->height);
		bitDepth = pngInfo->bit_depth;
		colorType = pngInfo->color_type;
		format = (colorType & PNG_COLOR_MASK_COLOR) != 0 ? ImageFormat::RGBA_8u : ImageFormat::Gray_8u;
		tRNSAlphaExists = png_get_valid(png, pngInfo, PNG_INFO_tRNS) != 0;

		png_destroy_read_struct(&png, &pngInfo, nullptr);
	}
}

void PNGImporter::ProcessImport()
{
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, this, PNGErrorFunction, PNGWarningFunction);
	png_infop pngInfo = png_create_info_struct(png);

	bufferReadLocation = 0; // Reading the entire file
	png_set_read_fn(png, this, &PNGImporter::PNGReadFunc);

	Assert(bitDepth >= 8);
	// TODO - Gray not supported
	// Set up
	if ((colorType & PNG_COLOR_MASK_ALPHA) == 0)
	{
		if (tRNSAlphaExists)
		{
			png_set_tRNS_to_alpha(png);
		}
		else
		{
			// Assuming bit depth is 8 at this point...
			png_set_add_alpha(png, 0xff, PNG_FILLER_AFTER);
		}
	}

	// Configure Transform
	i32 pngTransform = PNG_TRANSFORM_IDENTITY;
	if (bitDepth == 16)
	{
#ifdef PNG_READ_SCALE_16_TO_8_SUPPORTED
		pngTransform |= PNG_TRANSFORM_SCALE_16;
#else
		pngTransform |= PNG_TRANSFORM_STRIP_16;
#endif
	}

	// Read
	const u32 channels = 4;
	const u32 pixelBytes = bitDepth * channels / 8;
	const u32 bytesPerRow = pixelBytes * width;
	importedImageData.IncreaseSize(width * height * pixelBytes);

	png_bytep* pngRows = reinterpret_cast<png_bytep*>(png_malloc(png, sizeof(png_bytep) * height));
	for (i32 i = 0; i < height; ++i)
	{
		pngRows[i] = importedImageData.Offset(i * bytesPerRow);
	}
	png_set_rows(png, pngInfo, pngRows);

	if (!setjmp(jmpBuf))
	{
		png_read_png(png, pngInfo, pngTransform, nullptr);
	}

	png_free(png, pngRows);
	png_destroy_read_struct(&png, &pngInfo, nullptr);
}

bool PNGImporter::CheckPNGSignature() const
{
	Assert(importData.Size() > PNGSignatureByteSize);

	const png_size_t* pngSignature = reinterpret_cast<const png_size_t*>(importData.GetData());
	return png_sig_cmp(reinterpret_cast<png_const_bytep>(pngSignature), 0, PNGSignatureByteSize) == 0;
}
#pragma warning(pop) // setjmp usage