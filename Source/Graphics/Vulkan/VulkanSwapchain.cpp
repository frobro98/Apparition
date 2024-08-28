// Copyright 2020, Nathan Blane

#include "Memory/MemoryCore.hpp"
#include "Containers/ArrayView.hpp"
#include "VulkanSwapchain.h"
#include "VulkanDevice.h"
#include "VulkanFramebuffer.h"
#include "VulkanSurface.h"
#include "VulkanCreateInfos.h"
#include "VulkanRenderPass.h"
#include "VulkanUtilities.h"
#include "VulkanCommandBuffer.h"
#include "VulkanQueue.h"
#include "VulkanRenderingCloset.hpp"
#include "VulkanMemoryManager.hpp"
#include "VulkanTexture.h"

#include "Debugging/MetricInterface.hpp"

DECLARE_METRIC_GROUP(WindowGraphics);
METRIC_STAT(SubmitFrame, WindowGraphics);
METRIC_STAT(Present, WindowGraphics);

namespace
{
static void DestroySwapchainImageViews(VkDevice vkDevice, const ArrayView<VulkanTexture*> imageViews)
{
	for (const auto view : imageViews)
	{
		vkDestroyImageView(vkDevice, view->imageView, nullptr);
	}
}
}

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, VulkanSurface* renderSurface)
	:logicalDevice(device), surface(renderSurface)
{
}

void VulkanSwapchain::Initialize()
{
	CreateSwapchain();
	InitializeSwapchainSyncronization();
	CacheSwapchainImages();
}

void VulkanSwapchain::Deinitialize()
{
	DestroySwapchainImageViews(logicalDevice.GetNativeHandle(), swapchainImageTargets);

	vkDestroySemaphore(logicalDevice.GetNativeHandle(), imageAvailable, nullptr);
	vkDestroySemaphore(logicalDevice.GetNativeHandle(), renderingFinished, nullptr);
	Assert(swapchainHandle != VK_NULL_HANDLE);
	vkDestroySwapchainKHR(logicalDevice.GetNativeHandle(), swapchainHandle, nullptr);
}

void VulkanSwapchain::Recreate(u32 newWidth, u32 newHeight)
{
	// TODO - Seems not ideal, find different way
	vkDeviceWaitIdle(logicalDevice.GetNativeHandle());

	DestroySwapchainImageViews(logicalDevice.GetNativeHandle(), swapchainImageTargets);
	Assert(swapchainHandle != VK_NULL_HANDLE);
	//vkDestroySwapchainKHR(logicalDevice.GetNativeHandle(), swapchainHandle, nullptr);
	//surface->Deinitialize();

	surface->Initialize(newWidth, newHeight);
	CreateSwapchain();
	CacheSwapchainImages();

	// TODO - Seems not ideal, find different way
	vkDeviceWaitIdle(logicalDevice.GetNativeHandle());
}

VkResult VulkanSwapchain::GetNextImage()
{
	return vkAcquireNextImageKHR(logicalDevice.GetNativeHandle(), swapchainHandle, UINT64_MAX, imageAvailable, VK_NULL_HANDLE, &imageIndex);
}

void VulkanSwapchain::SubmitFrame()
{
	BEGIN_TIMED_BLOCK(SubmitFrame);

	VulkanCommandBufferManager& cmdBufferManager = logicalDevice.GetCmdBufferManager();
	cmdBufferManager.GetActiveGraphicsBuffer()->EndRenderPass();

	VkImageSubresourceRange range = {};
	range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	range.baseArrayLayer = 0;
	range.layerCount = 1;
	range.baseMipLevel = 0;
	range.levelCount = 1;
	ImageLayoutTransition(
		*cmdBufferManager.GetActiveGraphicsBuffer(), range,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		{ swapchainImageTargets[imageIndex]->image.Get() }
	);

	cmdBufferManager.SubmitGraphicsBuffer(false, imageAvailable, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, renderingFinished);

	END_TIMED_BLOCK(SubmitFrame);
}

void VulkanSwapchain::Present()
{
	BEGIN_TIMED_BLOCK(Present);

	VulkanQueue* presentQueue = logicalDevice.GetGraphicsQueue();

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderingFinished;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchainHandle;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	// NOTE - For some reason, Vulkan, on Desktop, is not getting the correct handles to the swapchain images.
	// It still renders, but throws errors and warnings. Calling vkGetSwapchainImagesKHR also gives climbing
	// image handle values (e.g. 1st: 0x74, Next: 0x77, etc.). Don't know why this happens... nor how to stop
	// it

// 	u32 imageCount;
// 	vkGetSwapchainImagesKHR(logicalDevice.GetNativeHandle(), swapchainHandle, &imageCount, nullptr);
// 	swapchainImages.Resize(imageCount);
// 	vkGetSwapchainImagesKHR(logicalDevice.GetNativeHandle(), swapchainHandle, &imageCount, swapchainImages.GetData());

	VkResult result = vkQueuePresentKHR(presentQueue->GetNativeHandle(), &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		//Recreate()
	}
	else if (result != VK_SUCCESS)
	{
		// TODO - Log
		Assert(false);
	}

	END_TIMED_BLOCK(Present);
}

VulkanTexture& VulkanSwapchain::GetSwapchainTarget() const
{
	return *swapchainImageTargets[imageIndex];
}

void VulkanSwapchain::CreateSwapchain()
{
	VkSurfaceCapabilitiesKHR surfaceCapabilites = surface->GetSurfaceCapabilities();
	DynamicArray<VkSurfaceFormatKHR> surfaceFormats = surface->GetSurfaceFormats();
	DynamicArray<VkPresentModeKHR> presentModes = surface->GetPresentModes();

	u32 swapchainImageCount = surfaceCapabilites.minImageCount + 1;
	if (surfaceCapabilites.maxImageCount > 0 &&
		swapchainImageCount > surfaceCapabilites.maxImageCount)
	{
		swapchainImageCount = surfaceCapabilites.maxImageCount;
	}

	// Finding optimal supported surface format
	VkSurfaceFormatKHR surfaceFormat = {};
	if (surfaceFormats.Size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		surfaceFormat = {
			VK_FORMAT_R8G8B8A8_UNORM,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		};
	}
	else
	{
		// TODO - Add for-each support to my arrays
		for (u32 i = 0; i < surfaceFormats.Size(); ++i)
		{
			if (surfaceFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
			{
				surfaceFormat = surfaceFormats[i];
				break;
			}
		}
	}

	if (surfaceFormat.format != VK_FORMAT_R8G8B8A8_UNORM)
	{
		surfaceFormat = surfaceFormats[0];
	}
	swapchainFormat = surfaceFormat.format;

	// Set up swapchain extents
	swapchainExtent.width = surfaceCapabilites.currentExtent.width == 0xffffffff ? surface->GetSurfaceWidth() : surfaceCapabilites.currentExtent.width;
	swapchainExtent.height = surfaceCapabilites.currentExtent.height == 0xffffffff ? surface->GetSurfaceHeight() : surfaceCapabilites.currentExtent.height;

	// Set up swapchain usage
	VkImageUsageFlags usageFlags;
	if (surfaceCapabilites.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	else
	{
		usageFlags = static_cast<VkImageUsageFlags>(-1);
	}

	// Setting up the surface transform. Mostly going to be using current transform
	VkSurfaceTransformFlagBitsKHR transformBits = surfaceCapabilites.currentTransform;

	// Set up presentation mode
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
	for (u32 i = 0; i < presentModes.Size(); ++i)
	{
		// Only checking against the mode with lowest latency and still has V-Sync
		//if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		//{
		//	presentMode = presentModes[i];
		//	break;
		//}
		//if(presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
		//{
		//	presentMode = presentModes[i];
		//}
		if (presentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		{
			presentMode = presentModes[i];
		}
		else if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			presentMode = presentModes[i];
			break;
		}
	}

	VkSwapchainCreateInfoKHR swapchainInfo = {};
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.surface = surface->GetNativeHandle();
	swapchainInfo.minImageCount = swapchainImageCount;
	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainInfo.imageExtent = swapchainExtent;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = usageFlags;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount = 0;
	swapchainInfo.pQueueFamilyIndices = nullptr;
	swapchainInfo.preTransform = transformBits;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.presentMode = presentMode;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = swapchainHandle;

	NOT_USED VkResult result = vkCreateSwapchainKHR(logicalDevice.GetNativeHandle(), &swapchainInfo, nullptr, &swapchainHandle);
	CHECK_VK(result);
}

void VulkanSwapchain::CacheSwapchainImages()
{
	u32 imageCount;
	vkGetSwapchainImagesKHR(logicalDevice.GetNativeHandle(), swapchainHandle, &imageCount, nullptr);
	swapchainImages.Resize(imageCount);
	vkGetSwapchainImagesKHR(logicalDevice.GetNativeHandle(), swapchainHandle, &imageCount, swapchainImages.GetData());

	swapchainImageTargets.Resize(imageCount);
	for (u32 i = 0; i < imageCount; ++i)
	{
		VulkanImage* img = new VulkanImage;
		img->device = &logicalDevice;
		img->handle = swapchainImages[i]; //(VkImage)(0xc+(u64)i);
		img->width = swapchainExtent.width;
		img->height = swapchainExtent.height;
		img->format = swapchainFormat;
		img->layout = VK_IMAGE_LAYOUT_UNDEFINED;
		img->aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
		swapchainImageTargets[i] = new VulkanTexture(logicalDevice, *img);
	}
}

void VulkanSwapchain::InitializeSwapchainSyncronization()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkResult result = vkCreateSemaphore(logicalDevice.GetNativeHandle(), &semaphoreInfo, nullptr, &imageAvailable);
	CHECK_VK(result);
	result = vkCreateSemaphore(logicalDevice.GetNativeHandle(), &semaphoreInfo, nullptr, &renderingFinished);
	CHECK_VK(result);
}
