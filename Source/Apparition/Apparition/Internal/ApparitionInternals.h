#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/CommandBuffer.h"
#include "Containers/DynamicArray.hpp"
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
	DynamicArray<VkImageView> views;
	VkSwapchainKHR swapchainHandle = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	VkExtent2D extents = {};
	VkFormat format = VK_FORMAT_UNDEFINED;
	VkSemaphore isImageAvailableSem = VK_NULL_HANDLE;
	VkSemaphore hasRenderingFinishedSem = VK_NULL_HANDLE;
};

struct CommandBufferInternals
{
	VkDevice owningDevice = VK_NULL_HANDLE;
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	Apparition::CommandBufferHandle handle;
};

struct CommandPoolInternals
{
	VkCommandPool cmdPool = VK_NULL_HANDLE;
	VkDevice owningDevice = VK_NULL_HANDLE;
	DynamicArray<CommandBufferInternals> allocatedCommandBuffers;
	Apparition::CommandPoolHandle handle;
};

struct DeviceInternal
{
	Backbuffer backbuffer{};
	DynamicArray<CommandPoolInternals> CommandPools;
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VmaAllocator allocator = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkCommandPool graphicsCmdPool = VK_NULL_HANDLE;
	VkCommandPool transferCmdPool = VK_NULL_HANDLE;
	VkCommandPool computeCmdPool = VK_NULL_HANDLE;
	u32 graphicsFamilyIndex = 0;
	u32 transferFamilyIndex = 0;
	u32 computeFamilyIndex = 0;
};