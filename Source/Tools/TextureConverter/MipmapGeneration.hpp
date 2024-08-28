// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

class Texture;

enum class MipMapFilter
{
	Box,
	Kaiser,
	FFT
};

u32 GetMaxMipMapLevels(
	u32 width, 
	u32 height
);

bool GenerateMipMapLevels(
	Texture& texture,
	u32 levelsToGen,
	MipMapFilter generationFilter
);