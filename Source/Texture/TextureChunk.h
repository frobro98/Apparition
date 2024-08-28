// Copyright 2020, Nathan Blane

#pragma once

#include "Texture2D/Texture.h"
#include "ImageFormats.h"

struct TextureChunk
{
	u32 width;
	u32 height;
	u32 size;
	ImageFormat format;
};