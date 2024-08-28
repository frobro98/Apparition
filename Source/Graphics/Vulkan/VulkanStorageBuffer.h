// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanAbstractions.h"
#include "BasicTypes/Uncopyable.hpp"

class VulkanStorageBuffer : private Uncopyable
{
public:
	VulkanStorageBuffer() = default;

	void Initialize(const VulkanDevice& device, u32 bufferSize);
	void UpdateStorage(const void* bufferData);

	inline VulkanBuffer& GetBuffer() { return *storageBuffer; }

private:
	VulkanBuffer* storageBuffer;
};

using StorageBuffer = VulkanStorageBuffer;