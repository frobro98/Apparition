// Copyright 2020, Nathan Blane

#pragma once

#include "Texture2D/Texture.h"

namespace Compression
{
void CompressForGPU(const Texture& inDescription, Texture& outDescription, CompressionFormat compressionFormat);
}