// Copyright 2020, Nathan Blane

#pragma once

WALL_WRN_PUSH
#include "libPNG/png.h"
#include "libPNG/pnginfo.h"
WALL_WRN_POP

#include "Importers/TextureImporter.hpp"

class PNGImporter : public TextureImporter
{
public:
	virtual void SetImportData(MemoryBuffer&& compressedData) override;

private:
	static void PNGReadFunc(png_structp png_ptr, png_bytep data, png_size_t length);

private:
	void ProcessPNGHeader();
	virtual void ProcessImport() override;
	bool CheckPNGSignature() const;

private:
	u32 bufferReadLocation = 0;
	u32 bitDepth = 0;
	u32 colorType = 0;
	bool tRNSAlphaExists;
};