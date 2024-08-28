// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"
#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "Graphics/GraphicsResourceDefinitions.hpp"

class VulkanDevice;
class ImageMemory;
class BufferMemory;
class ImageMemory;

struct VulkanImage
{
	~VulkanImage();

	VkImage handle = VK_NULL_HANDLE;
	VkImageLayout layout;
	VkFormat format = VK_FORMAT_UNDEFINED;
	VkImageAspectFlags aspectFlags;
	u32 width = 0;
	u32 height = 0;
	u32 mipmapLevels = 1;
	ImageMemory* memory;
	VulkanDevice* device = nullptr;

	VkImageView CreateView() const;

	bool Initialized() const { return handle != VK_NULL_HANDLE && memory != nullptr; }
};

// TODO - This is getting big enough to make it a class
struct VulkanBuffer
{
	VkBuffer handle = VK_NULL_HANDLE;
	BufferMemory* memory;
	VulkanDevice& device;

	VulkanBuffer(VulkanDevice& dev, VkBuffer hdl, BufferMemory& mem)
		: handle(hdl),
		memory(&mem),
		device(dev)
	{
	}

	~VulkanBuffer();

	// TODO - Might consider implementing copying for these instead of leaving them default
	VulkanBuffer(const VulkanBuffer&) = default;
	VulkanBuffer& operator=(const VulkanBuffer&) = default;

	VulkanBuffer(VulkanBuffer&& other) noexcept
		: handle(other.handle),
		memory(other.memory),
		device(other.device)
	{
		other.handle = VK_NULL_HANDLE;
		other.memory = nullptr;
	}

	VulkanBuffer& operator=(VulkanBuffer&& other) noexcept
	{
		if (&other != this && other.Initialized())
		{
			handle = other.handle;
			memory = other.memory;

			other.handle = VK_NULL_HANDLE;
			other.memory = nullptr;
		}

		return *this;
	}

	bool Initialized() const { return handle != VK_NULL_HANDLE && memory != nullptr; }
};
