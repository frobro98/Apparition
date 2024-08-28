// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanAbstractions.h"
#include "BasicTypes/Uncopyable.hpp"
#include "Containers/DynamicArray.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "VulkanBufferAllocation.hpp"

struct Face;
class VulkanDevice;

class VulkanIndexBuffer : public NativeIndexBuffer, private Uncopyable
{
public:
	VulkanIndexBuffer(VulkanDevice& device, u64 sizeInBytes, u32 indexSize);
	~VulkanIndexBuffer();

	inline VulkanBuffer& GetBuffer() const { return *indexBuffer; }
	inline u64 GetSize() const { return indexBuffer->memory->size; }
	inline u32 GetIndexStride() const { return indexStride; }

private:
	VulkanDevice& logicalDevice;
	VulkanBuffer* indexBuffer;
	u32 indexStride;
};
