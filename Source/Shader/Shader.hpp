// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Shader/ShaderResourceManager.hpp"
#include "Shader/ShaderID.hpp"
#include "Shader/ShaderAPI.hpp"

// In the process of refactoring the material system, shaders need to obviously be considered in this endeavor.
// One of the issues with shaders is that there isn't just a type of shader. From a high level, you can't just
// get a single shader from the file system and expect that to be all you need. A material, currently, needs
// at least a vertex shader and a fragment shader. If I want to keep things simple and not have multiple 
// high level shader types, that's all well and good. It sort of seems bad and could easily be unraveled if
// there aren't the proper checks down on the lower levels of the shader system

// This is a high level shader type. It just holds data, doesn't actually contain any behavior. The behavior
// comes from the MaterialShader class, which wraps around this shader type
namespace Shader
{
SHADER_API ShaderID FindOrLoadShaderFile(const tchar* shaderFile);
//void UnloadShader(ShaderID id);
//void IsShaderLoaded(ShaderID id);
}

// Material Shader
//	- Internal to Material class
//	- Holds resource storage for a specific shader
//	- Holds references to specific ShaderResource objects
//
// ShaderResource
//	- Representation of shader code
//	- Can represent any shader that is part of the programmable pipeline
//	- Contains the shader resource names and information, and an initialized shader resource
