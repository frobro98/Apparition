// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Shader/ShaderConstantNameMap.hpp"
#include "Logging/LogFunctions.hpp"
#include "Debugging/Assertion.hpp"
#include "Shader/ShaderAPI.hpp"

DEFINE_LOG_CHANNEL(ShaderResources);

struct NativeUniformBuffer;
struct NativeTexture;
struct NativeSampler;

struct ShaderBindInfo
{
	String name;
	u16 bindIndex;
	u16 size;
};

// Header for the pointer data?
struct ShaderConstantTable
{
	DynamicArray<ShaderBindInfo> uniformBufferResourceInfo;
	DynamicArray<ShaderBindInfo> textureResourceInfo;
};

SHADER_API ShaderConstantTable ConstructShaderConstantTable(ShaderConstantNameMap& nameMap);

//////////////////////////////////////////////////////////////////////////
//Serialization
//////////////////////////////////////////////////////////////////////////

inline void Serialize(SerializeBase& ser, const ShaderBindInfo& shaderInfo)
{
	Serialize(ser, shaderInfo.name);
	Serialize(ser, shaderInfo.bindIndex);
	Serialize(ser, shaderInfo.size);
}

inline void Deserialize(DeserializeBase& ser, ShaderBindInfo& shaderInfo)
{
	Deserialize(ser, shaderInfo.name);
	Deserialize(ser, shaderInfo.bindIndex);
	Deserialize(ser, shaderInfo.size);
}

inline void Serialize(SerializeBase& ser, const ShaderConstantTable& shaderHeader)
{
	Serialize(ser, shaderHeader.uniformBufferResourceInfo);
	Serialize(ser, shaderHeader.textureResourceInfo);
}

inline void Deserialize(DeserializeBase& ser, ShaderConstantTable& shaderHeader)
{
	Deserialize(ser, shaderHeader.uniformBufferResourceInfo);
	Deserialize(ser, shaderHeader.textureResourceInfo);
}

// So I need to essentially have a collection of shader resources (texture samplers, buffers, etc)
// and I need to be able to have a way to set a resource with validation. If a material has a
// normal map/extra texture, I need to be able to set it in an intelligent way, not necessarily
// knowing what every binding in the shader is.

// This could be done by name of the parameter -> parameter structure transformation. This has the
// benefit of not requiring intimate knowledge of the shader being used. It does, however, require
// some knowledge of it. Because Material creation is happening within the scripting, the user would
// be able to know what textures they'd need to set up

// Currently, because this is dealing with materials, it doesn't necessarily allow for uniform buffers 
// or other resources to be in this. I don't think this is an issue just yet, so I won't worry about it.
// If it eventually comes up, and materials being similar in mechanisms to Unity materials, I will just
// have them behave in a similar way too, i.e. setting the data through the material.

// With this kind of assumption in mind, how does one validate the shader parameters? For uniform buffers,
// the data structure already lives within the engine, so it could be as easy as having static meta data
// for all of the uniform buffer structures and looping through that to find out if the resource exists
// within a shader. This method doesn't necessarily remove the need for some internal book keeping. The
// internal book keeping, in this case, would have to come from the shader itself. There isn't really
// anything other than the name of the resource within the shader. 

// To prevent this sort of thing from costing a bunch due to string hashing, the user is given a handle
// to the portion of the material that they're asking for. For all of the data that is set in the background,
// setting up that data should use the backing behavior that the Material would essentially be using. In the
// end, every portion of the shader should exist for the actual pipeline to use.

// Ok so the user is setting aspects of the Material up now. So where does the actual data get stored? There
// needs to be some backing storage that receives the shader resources. This backing data should be big enough
// for all of the resources that live within the shader. With this backing store, the theory is that there
// would be the data for each kind of buffer, texture, etc. living alongside this blob of pointers to native
// resources. With this data living alongside the pointers, we can get the bind point of each resource and 
// push that along to the graphics api.


