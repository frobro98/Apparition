// Copyright 2020, Nathan Blane

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <mutex>

#include "Utilities/Array.hpp"
#include "VulkanDevice.h"
#include "VulkanCreateInfos.h"
#include "VulkanQueue.h"
#include "VulkanDescriptorPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFence.hpp"
#include "VulkanStagingBufferManager.hpp"
#include "VulkanMemoryManager.hpp"
#include "VulkanDescriptorLayoutManager.h"
#include "VulkanRenderingCloset.hpp"
#include "VulkanUtilities.h"

VulkanDevice::~VulkanDevice()
{
	delete graphicsQueue;
	delete transferQueue;
	delete fenceManager;
	delete commandBufferManager;
	delete stagingBufferManager;
	delete memoryManager;
	delete storage;

	for (auto pool : descriptorPools)
	{
		delete pool;
	}

	vkDestroyDevice(vulkanDevice, nullptr);
}

void VulkanDevice::Initialize(VkInstance inst)
{
	u32 physicalDeviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, nullptr);
	CHECK_VK(result);
	DynamicArray<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = vkEnumeratePhysicalDevices(inst, &physicalDeviceCount, physicalDevices.GetData());
	CHECK_VK(result);

	u32 graphicsFamilyIndex = std::numeric_limits<u32>::max();
	u32 transferFamilyIndex = std::numeric_limits<u32>::max();
	for(const auto& physicalDevice : physicalDevices)
	{
		if (IsSuitableGPU(physicalDevice, graphicsFamilyIndex, transferFamilyIndex))
		{
			gpu = physicalDevice;
			break;
		}
	}

	Assert(graphicsFamilyIndex != std::numeric_limits<u32>::max());
	Assert(transferFamilyIndex != std::numeric_limits<u32>::max());

	f32 priorities[] = { 1.0f };
	VkDeviceQueueCreateInfo queueInfos[] =
	{
		Vk::DeviceQueueInfo(graphicsFamilyIndex, 1, priorities),
		Vk::DeviceQueueInfo(transferFamilyIndex, 1, priorities)
	};

	const tchar* deviceExtensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	// Enabling features
	VkPhysicalDeviceFeatures gpuFeatures;
	VkFormatProperties properties;
	vkGetPhysicalDeviceFormatProperties(gpu, VK_FORMAT_R8G8B8_UNORM, &properties);
	vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);
	vkGetPhysicalDeviceProperties(gpu, &gpuProperties);
	vkGetPhysicalDeviceMemoryProperties(gpu, &memoryProperties);

	if (gpuFeatures.geometryShader)
	{
		enabledGPUFeatures.geometryShader = VK_TRUE;
	}
	if (gpuFeatures.tessellationShader)
	{
		enabledGPUFeatures.tessellationShader = VK_TRUE;
	}
	if (gpuFeatures.fillModeNonSolid)
	{
		enabledGPUFeatures.fillModeNonSolid = VK_TRUE;
	}
	if (gpuFeatures.textureCompressionBC)
	{
		enabledGPUFeatures.textureCompressionBC = VK_TRUE;
	}
	if (gpuFeatures.textureCompressionETC2)
	{
		enabledGPUFeatures.textureCompressionETC2 = VK_TRUE;
	}
	if (gpuFeatures.textureCompressionASTC_LDR)
	{
		enabledGPUFeatures.textureCompressionASTC_LDR = VK_TRUE;
	}

	Assert(enabledGPUFeatures.fillModeNonSolid);
	Assert(enabledGPUFeatures.tessellationShader);
	Assert(enabledGPUFeatures.geometryShader);
	Assert(enabledGPUFeatures.textureCompressionBC);

	VkDeviceCreateInfo deviceInfo = Vk::DeviceInfo(queueInfos, (u32)ArraySize(queueInfos), deviceExtensions, (u32)ArraySize(deviceExtensions), enabledGPUFeatures);
	result = vkCreateDevice(gpu, &deviceInfo, nullptr, &vulkanDevice);
	CHECK_VK(result);

	graphicsQueue = new VulkanQueue(this);
 	graphicsQueue->Initialize(graphicsFamilyIndex, VulkanQueue::Type::Graphics);

	transferQueue = new VulkanQueue(this);
	transferQueue->Initialize(transferFamilyIndex, VulkanQueue::Type::Copy);

	commandBufferManager = new VulkanCommandBufferManager(*this);
	commandBufferManager->Initialize();

	fenceManager = new VulkanFenceManager(*this);
	stagingBufferManager = new VulkanStagingBufferManager(*this);
	memoryManager = new VulkanMemoryManager(*this);

	descriptorPools.Add(new VulkanDescriptorPool(*this));

	storage = new VulkanRenderingCloset(*this);

	// TODO - Remove this because this class shouldn't exist and will be replaced after this gets removed
	GetDescriptorLayoutManager().logicalDevice = this;
}

VkFormat VulkanDevice::QuerySupportedFormat(DynamicArray<VkFormat> possibleFormats, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	Assert(vulkanDevice);

	VkFormat supportedFormat = VK_FORMAT_UNDEFINED;
	for (u32 i = 0; i < possibleFormats.Size(); ++i)
	{
		VkFormat format = possibleFormats[i];
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(gpu, format, &formatProperties);
		if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & features) == features)
		{
			supportedFormat = format;
			break;
		}
		else if (tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & features) == features)
		{
			supportedFormat = format;
			break;
		}
	}

	return supportedFormat;
}

bool VulkanDevice::IsFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	bool result = false;
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(gpu, format, &formatProperties);
	if (tiling == VK_IMAGE_TILING_OPTIMAL && (formatProperties.optimalTilingFeatures & features) == features)
	{
		result = true;
	}
	else if (tiling == VK_IMAGE_TILING_LINEAR && (formatProperties.linearTilingFeatures & features) == features)
	{
		result = true;
	}

	return result;
}

static std::mutex renderStorageMut;

ResourceLockPtr<VulkanRenderingCloset*> VulkanDevice::GetRenderingStorage() const
{
	return ResourceLockPtr<VulkanRenderingCloset*>(storage, renderStorageMut);
}

bool VulkanDevice::IsSuitableGPU(VkPhysicalDevice physicalDevice, u32& graphicsIndex, u32& transferIndex)
{
	VkPhysicalDeviceProperties deviceProperties;

	vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		// TODO - Check if checking version of GPU api is necessary

		// TODO - Check what limits are important for a gpu
		Assert(deviceProperties.limits.maxImageDimension2D >= 4096);

		u32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
		Assert(queueFamilyCount > 0);
		DynamicArray<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.GetData());

		for (u32 i = 0; i < queueFamilyCount; ++i)
		{
			VkBool32 presentationSupported = PresentationSupported(physicalDevice, i);

			if (queueFamilyProperties[i].queueCount > 0 &&
				queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
				queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				if (graphicsIndex == std::numeric_limits<u32>::max())
				{
					graphicsIndex = i;
				}

				if (presentationSupported)
				{
					graphicsIndex = i;
				}
			}
			else if (queueFamilyProperties[i].queueCount > 0 &&
				queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				if (transferIndex == std::numeric_limits<u32>::max())
				{
					transferIndex = i;
				}
			}
		}

		return true;
	}
	else
	{
		// TODO - Log that there isn't support for any other type of GPU

		return false;
	}
}
