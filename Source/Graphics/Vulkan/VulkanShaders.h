// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/MemoryBuffer.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Vulkan/VulkanDefinitions.h"
#include "Vulkan/VulkanShaderHeader.hpp"

// TODO - Loading shaders will be more than just loading the file. It will be compilation at runtime, reflection over source, etc.
MemoryBuffer LoadSPVShader(const tchar* shaderFile);

class VulkanDevice;

struct ShaderData
{
	ShaderData(const VulkanDevice& device, VulkanShaderHeader& shaderHeader, const DynamicArray<u32>& shaderCode);
	~ShaderData();

	VulkanShaderHeader shaderHeader;
	const VulkanDevice& logicalDevice;
	VkShaderModule module;
	VkShaderStageFlags shaderStage;
};

struct VulkanVertexShader : NativeVertexShader
{
	VulkanVertexShader(const VulkanDevice& device, VulkanShaderHeader& shaderHeader, const DynamicArray<u32>& shaderCode);
	const ShaderData shaderData;
};

struct VulkanFragmentShader : NativeFragmentShader
{
	VulkanFragmentShader(const VulkanDevice& device, VulkanShaderHeader& shaderHeader, const DynamicArray<u32>& shaderCode);
	const ShaderData shaderData;
};

struct VulkanGeometryShader : NativeGeometryShader
{
	VulkanGeometryShader(const VulkanDevice& device, VulkanShaderHeader& shaderHeader, const DynamicArray<u32>& shaderCode);
	const ShaderData shaderData;
};

struct VulkanTessEvaluationShader : NativeTessEvaluationShader
{
	VulkanTessEvaluationShader(const VulkanDevice& device, VulkanShaderHeader& shaderHeader, const DynamicArray<u32>& shaderCode);
	const ShaderData shaderData;
};

struct VulkanTessControlShader : NativeTessControlShader
{
	VulkanTessControlShader(const VulkanDevice& device, VulkanShaderHeader& shaderHeader, const DynamicArray<u32>& shaderCode);
	const ShaderData shaderData;
};

struct VulkanComputeShader : NativeComputeShader
{
	VulkanComputeShader(const VulkanDevice& device, VulkanShaderHeader& shaderHeader, const DynamicArray<u32>& shaderCode);
	const ShaderData shaderData;
};
