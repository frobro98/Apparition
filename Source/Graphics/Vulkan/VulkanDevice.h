// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"
#include "Containers/DynamicArray.hpp"
#include "Graphics/ResourceLockPtr.hpp"

class RenderContext;
class Window;
class VulkanQueue;
class VulkanCommandBufferManager;
class VulkanStagingBufferManager;
class VulkanMemoryManager;
class VulkanFenceManager;
class VulkanDescriptorPool;
class VulkanRenderingCloset;

class VulkanDevice
{
public:
	VulkanDevice() = default;
	~VulkanDevice();

	void Initialize(VkInstance inst);
	VkFormat QuerySupportedFormat(DynamicArray<VkFormat> possibleFormats, VkImageTiling tiling, VkFormatFeatureFlags features);
	bool IsFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) const;

	ResourceLockPtr<VulkanRenderingCloset*> GetRenderingStorage() const;

	inline const VkPhysicalDeviceLimits& GetDeviceLimits() const { return gpuProperties.limits; }
	inline const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() const { return memoryProperties; }
	// TODO - Move this and probably descriptor pools elsewhere
	inline VulkanDescriptorPool* GetActivePool() const { return descriptorPools[0]; }

	inline VkDevice GetNativeHandle() const { return vulkanDevice; }
	inline VkPhysicalDevice GetPhysicalDevice() const { return gpu; }
	inline VulkanCommandBufferManager& GetCmdBufferManager() const { return *commandBufferManager; }
	inline VulkanFenceManager& GetFenceManager() const { return *fenceManager; }
	inline VulkanStagingBufferManager& GetStagingBufferManager() const { return *stagingBufferManager; }
	inline VulkanMemoryManager& GetMemoryManager() const { return *memoryManager; }
	
	inline VulkanQueue* GetGraphicsQueue() const
	{
		Assert(vulkanDevice != VK_NULL_HANDLE);
		Assert(graphicsQueue != nullptr);
		return graphicsQueue;
	}

	inline VulkanQueue* GetTransferQueue() const
	{
		Assert(vulkanDevice != VK_NULL_HANDLE);
		Assert(transferQueue != nullptr);
		return transferQueue;
	}

private:
	bool IsSuitableGPU(VkPhysicalDevice physicalDevice, u32& graphicsIndex, u32& transferIndex);

	VkPhysicalDeviceProperties gpuProperties;
	VkPhysicalDeviceMemoryProperties memoryProperties;
	VkPhysicalDeviceFeatures enabledGPUFeatures{};
	VkPhysicalDevice gpu = VK_NULL_HANDLE;
	VkDevice vulkanDevice = VK_NULL_HANDLE;

	DynamicArray<class VulkanDescriptorPool*> descriptorPools;

	VulkanFenceManager* fenceManager = nullptr;
	VulkanCommandBufferManager* commandBufferManager = nullptr;
	VulkanStagingBufferManager* stagingBufferManager = nullptr;
	VulkanMemoryManager* memoryManager = nullptr;
	VulkanRenderingCloset* storage = nullptr;
	VulkanQueue* graphicsQueue = nullptr;
	VulkanQueue* transferQueue = nullptr;
};
