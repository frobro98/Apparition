// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"


// CPU version of a generic UniformBuffer type.
// Corresponds with the shader definition
struct UniformBufferType
{
	UniformBufferType(
		const tchar* type,
		const tchar* varName
	)
		: bufferType(type),
		variableName(varName)
	{
	}

	const tchar* bufferType;
	const tchar* variableName;
};


// GPU version of a UniformBuffer type.
// Holds the bindIndex of the actual buffer shader side
struct ShaderParameterType
{
	ShaderParameterType(u32 index)
		: bindIndex(index)
	{
	}

	const u32 bindIndex;
	bool isBound = false;
};

template <typename UniBuffType>
struct ShaderParameter : public ShaderParameterType
{
	ShaderParameter(u32 index)
		: ShaderParameterType(index)
	{
	}
};