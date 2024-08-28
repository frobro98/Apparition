// Copyright 2020, Nathan Blane

#pragma once

#include "String/String.h"
#include "ImageFormats.h"
#include "Texture2D/Texture.h"

class Path;

bool IsSupportedTexture(
	const Path& texturePath
);

Texture ProcessImageFile(
	const Path& filePath, 
	CompressionFormat format
);