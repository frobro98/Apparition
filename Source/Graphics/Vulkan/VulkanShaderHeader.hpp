#pragma once

#include "Containers/DynamicArray.hpp"
#include "Graphics/Vulkan/VulkanAbstractions.h"
#include "String/String.h"

constexpr u32 ShaderConstantNameLen = 64;
struct SpirvBuffer
{
	String name;
	VkDescriptorType bufferType;
	u32 bindIndex;
};

struct SpirvSampler
{
	String name;
	VkDescriptorType samplerType;
	u32 bindIndex;
};

struct VulkanShaderHeader
{
	DynamicArray<SpirvBuffer> buffers;
	DynamicArray<SpirvSampler> samplers;
	String entryPoint;
	VkShaderStageFlags shaderStage;
	u32 bytecodeHash;
};

// Serializing header of a shader
forceinline void Serialize(SerializeBase& ser, const SpirvBuffer& buf)
{
	u32 type = (u32)buf.bufferType;
	Serialize(ser, type);
	Serialize(ser, buf.bindIndex);
}

forceinline void Serialize(SerializeBase& ser, const SpirvSampler& sampler)
{
	u32 type = (u32)sampler.samplerType;
	Serialize(ser, type);
	Serialize(ser, sampler.bindIndex);
}

forceinline void Serialize(SerializeBase& ser, const VulkanShaderHeader& shaderHeader)
{
	Serialize(ser, (u32)shaderHeader.shaderStage);
	Serialize(ser, shaderHeader.bytecodeHash);
	Serialize(ser, shaderHeader.buffers);
	Serialize(ser, shaderHeader.samplers);
	Serialize(ser, shaderHeader.entryPoint);
}

// Deserializing header of a shader
forceinline void Deserialize(DeserializeBase& ser, SpirvBuffer& buf)
{
	u32 type;
	Deserialize(ser, type);
	buf.bufferType = (VkDescriptorType)type;
	Deserialize(ser, buf.bindIndex);
}

forceinline void Deserialize(DeserializeBase& ser, SpirvSampler& sampler)
{
	u32 type;
	Deserialize(ser, type);
	sampler.samplerType = (VkDescriptorType)type;
	Deserialize(ser, sampler.bindIndex);
}

forceinline void Deserialize(DeserializeBase& ser, VulkanShaderHeader& shaderHeader)
{
	u32 stage;
	Deserialize(ser, stage);
	shaderHeader.shaderStage = (VkShaderStageFlags)stage;
	Deserialize(ser, shaderHeader.bytecodeHash);
	Deserialize(ser, shaderHeader.buffers);
	Deserialize(ser, shaderHeader.samplers);
	Deserialize(ser, shaderHeader.entryPoint);
}

