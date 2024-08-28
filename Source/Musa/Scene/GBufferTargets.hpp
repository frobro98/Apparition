// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
#include "Graphics/RenderTarget.hpp"

struct GBuffer
{
	RenderTarget* positionTexture = nullptr;
	RenderTarget* normalTexture = nullptr;
	RenderTarget* diffuseTexture = nullptr;
};


