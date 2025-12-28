#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/ImageDescription.h"
#include "Containers/DynamicArray.hpp"
#include "HandlePool.h"
#include "VulkanDefinitions.h"

class DeviceManager;
class CommandBufferManager;

struct ApparitionInternals
{
	Apparition::AllocationCallbacks allocCallbacks;
	DeviceManager* deviceManager = nullptr;
	Apparition::ValidationDelegate userValidationDelegate;
	void* validationUserData = nullptr;
};

extern ApparitionInternals apparition;

// TODO - Move these to separate header
struct Backbuffer
{
	DynamicArray<u32> views;
	DynamicArray<u32> images;
	VkSwapchainKHR swapchainHandle = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	VkExtent2D extents = {};
	Apparition::ImageFormat::Type format = Apparition::ImageFormat::Invalid;
	VkSemaphore isImageAvailableSem = VK_NULL_HANDLE;
	VkSemaphore hasRenderingFinishedSem = VK_NULL_HANDLE;
	u32 currentImageIndex = 0;
};

struct QueueInternal
{
	VkQueue queue = VK_NULL_HANDLE;
	// Maybe have the type of queue this is?
};

struct CommandBufferInternal
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	// General state information.
	// TODO: Will need more specific state info about where we are in the CB's lifetime
	bool hasBegun = false;
};

struct CommandPoolInternal
{
	VkCommandPool cmdPool = VK_NULL_HANDLE;
	u32 queueFamilyIndex = 0;
};

struct BufferResourceInternal
{
	VkBuffer buffer = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;
	bool isMappable = false;
};

struct ImageResourceInternal
{
	VkImage image = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;

	// Image formatting and access
	Apparition::ImageFormat::Type format;
	Apparition::ImageAccess::Type access;
};

struct ImageViewResourceInternal
{
	VkImageView imageView = VK_NULL_HANDLE;
	u32 imageIndex = UINT32_MAX;

	// View information
};

struct DeviceInternal
{
	Backbuffer backbuffer{};
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VmaAllocator allocator = VK_NULL_HANDLE;

	// The sizes of these pools relates to internal queue family properties
	HandlePool graphicsQueueHandlePool;
	HandlePool transferQueueHandlePool;
	HandlePool computeQueueHandlePool;
	// This contains queues that are live within the ecosystem, not all queues available
	DynamicArray<QueueInternal> graphicsQueues;
	DynamicArray<QueueInternal> transferQueues;
	DynamicArray<QueueInternal> computeQueues;

	HandlePool commandPoolsHandlePool;
	HandlePool commandBufferHandlePool;
	HandlePool bufferResourceHandlePool;
	HandlePool imageResourceHandlePool;
	HandlePool imageViewResourceHandlePool;
	DynamicArray<CommandPoolInternal> commandPools;
	DynamicArray<CommandBufferInternal> commandBuffers;
	DynamicArray<BufferResourceInternal> bufferResources;
	DynamicArray<ImageResourceInternal> imageResources;
	DynamicArray<ImageViewResourceInternal> imageViewResources;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	u32 graphicsFamilyIndex = 0;
	u32 transferFamilyIndex = 0;
	u32 computeFamilyIndex = 0;
};