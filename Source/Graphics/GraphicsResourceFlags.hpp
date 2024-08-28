// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

struct  TextureUsage
{
	enum Type
	{
		SampledResource = 0x00000000,
		RenderTarget = 0x00000001,
		DepthStencilTarget = 0x00000002,
		Presentation = 0x00000004,

		UsageMax = 0x70000000
	};
};

struct BufferUsage
{
	enum Type
	{

		UsageMax = 0x70000000
	};
};
