// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"

#include "Containers/DynamicArray.hpp"

class Window;
class VulkanDevice;
class VulkanSwapchain;

class VulkanSurface
{
public:
	VulkanSurface(VkInstance instance, VulkanDevice* device, void* windowHandle);

	void Initialize(u32 width, u32 height);
	void Deinitialize();

	void Recreate(u32 newWidth, u32 newHeight);

	inline VkSurfaceKHR GetNativeHandle() const { return surfaceHandle; }

	DynamicArray<VkPresentModeKHR> GetPresentModes() const;
	DynamicArray<VkSurfaceFormatKHR> GetSurfaceFormats() const;
	VkSurfaceCapabilitiesKHR GetSurfaceCapabilities() const;

	i32 GetSurfaceWidth() const;
	i32 GetSurfaceHeight() const;

private:
	VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
	// TODO - consider making this padding into a Vulkan struct
	VkInstance instance = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	DynamicArray<VkPresentModeKHR> presentModes;
	DynamicArray<VkSurfaceFormatKHR> surfaceFormats;
	VulkanDevice* logicalDevice;
	void* windowHandle;
	u32 width;
	u32 height;
};