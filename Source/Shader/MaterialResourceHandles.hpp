// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "String/String.h"
#include "Debugging/Assertion.hpp"
#include "Shader/ShaderTables.hpp"

struct ShaderConstantNameMap;
struct NativeUniformBuffer;
struct NativeTexture;

// Index into the resource table "header" for where the resource actually lives
struct UniformBufferDescriptor
{
	u16 resourceIndex;
};

// Index into the resource table "header" for where the resource actually lives
struct TextureSamplerDescriptor
{
	u16 resourceIndex;
};

