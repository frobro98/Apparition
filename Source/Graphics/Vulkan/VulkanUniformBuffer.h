// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanAbstractions.h"
#include "BasicTypes/Uncopyable.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"

class VulkanUniformBuffer : public NativeUniformBuffer , private Uncopyable
{
public:
	VulkanUniformBuffer(const VulkanDevice& device, u32 bufferSize);
	~VulkanUniformBuffer();

	void UpdateUniforms(const void* bufferData);

	inline VulkanBuffer& GetBuffer() const { return *uniformBuffer; }

private:
	const VulkanDevice& logicalDevice;
	VulkanBuffer* uniformBuffer;
	
};

class VulkanGlobalUniformBuffer
{
public:
	VulkanGlobalUniformBuffer(const VulkanDevice& device, u32 totalMemory);
	~VulkanGlobalUniformBuffer();

	void AddDataToBuffer(const void* bufferData, u32 dataSize);
	inline VulkanBuffer& GetBuffer() const { return *globalBuffer; }

private:
	const VulkanDevice& logicalDevice;
	VulkanBuffer* globalBuffer;
	u32 bufferPosition;
	u32 alignment;
};

