// Copyright 2020, Nathan Blane

#pragma once

#include "Shader/ShaderStructure.hpp"
#include "Shader/ShaderHeader.hpp"

using ShaderInputByLocation = Map<u32, ShaderVariable>;
using ShaderOutputByLocation = Map<u32, ShaderVariable>;
using ShaderConstantByBinding = Map<u32, ShaderConstant>;

struct ShaderCompiledOutput
{
	ShaderHeader header;
	ShaderInputByLocation locationToInputs;
	ShaderOutputByLocation locationToOutputs;

	ShaderConstantByBinding bindingToConstants;
	MemoryBuffer shaderCode;
	const tchar* shaderEntryPoint;

	ShaderStage::Type stage;
};

struct ShaderStructure
{
	ShaderCompiledOutput compiledOutput;
	ShaderDebugInfo readableShaderInfo;
};
