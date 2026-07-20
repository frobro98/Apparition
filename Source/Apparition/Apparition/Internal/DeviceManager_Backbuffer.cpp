
#include "DeviceManager.h"

#include "ApparitionInternals.h"
#include "Containers/DynamicArray.hpp"
#include "ImageFormatConversion.h"
#include "HandleDefinitions.h"
#include "VulkanInfos.h"


void DeviceManager::SetupBackbuffer(Apparition::Device device, const Apparition::BackbufferSetupParams& params)
{
    using namespace Apparition;

    DeviceInternal& deviceInternal = DeviceInternalFrom(device);

    if (deviceInternal.backbuffer.swapchainHandle != VK_NULL_HANDLE)
    {
		// TODO - Log that backbuffer is already set up
		return;
    }

	Backbuffer& backbuffer = deviceInternal.backbuffer;
    // Create surface
    
    // NOTE - Only supports Windows surfaces right now. No need for anything else atm
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    Vk::ZeroInfoStruct(surfaceCreateInfo, VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR);
    surfaceCreateInfo.hinstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(nullptr));
    surfaceCreateInfo.hwnd = reinterpret_cast<HWND>(params.wndHandle);
    VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &backbuffer.surfaceHandle);
    CHECK_VK(result);

    VkBool32 presentationSupported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(deviceInternal.physicalDevice, deviceInternal.graphicsFamilyIndex, backbuffer.surfaceHandle, &presentationSupported);
    if (presentationSupported != VK_TRUE)
    {
        // Error and return
		return;
    }

    // Needs surface information for swapchain setup
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(deviceInternal.physicalDevice, backbuffer.surfaceHandle, &surfaceCapabilities);
    CHECK_VK(result);

    DynamicArray<VkSurfaceFormatKHR> surfaceFormats;
    u32 formatCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(deviceInternal.physicalDevice, backbuffer.surfaceHandle, &formatCount, nullptr);
    CHECK_VK(result);
    surfaceFormats.Resize(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(deviceInternal.physicalDevice, backbuffer.surfaceHandle, &formatCount, surfaceFormats.GetData());
    CHECK_VK(result);

    DynamicArray<VkPresentModeKHR> presentModes;
    u32 presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(deviceInternal.physicalDevice, backbuffer.surfaceHandle, &presentModeCount, nullptr);
    CHECK_VK(result);
    presentModes.Resize(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(deviceInternal.physicalDevice, backbuffer.surfaceHandle, &presentModeCount, presentModes.GetData());
    CHECK_VK(result);

    // Create swapchain
	u32 swapchainImageCount = surfaceCapabilities.minImageCount + 1;
	if (surfaceCapabilities.maxImageCount > 0 &&
		swapchainImageCount > surfaceCapabilities.maxImageCount)
	{
		swapchainImageCount = surfaceCapabilities.maxImageCount;
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
	backbuffer.format = VkFormatToApparition(surfaceFormat.format);

	// Set up swapchain extents
	backbuffer.extents.width = surfaceCapabilities.currentExtent.width == 0xffffffff ? params.wndWidth : surfaceCapabilities.currentExtent.width;
	backbuffer.extents.height = surfaceCapabilities.currentExtent.height == 0xffffffff ? params.wndHeight : surfaceCapabilities.currentExtent.height;

	// Set up swapchain usage
	VkImageUsageFlags usageFlags;
	if (surfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	{
		usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	}
	else
	{
		usageFlags = static_cast<VkImageUsageFlags>(-1);
	}

	// Setting up the surface transform. Mostly going to be using current transform
	VkSurfaceTransformFlagBitsKHR transformBits = surfaceCapabilities.currentTransform;

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
		if(presentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
		{
			presentMode = presentModes[i];
			break;
		}
		//if (presentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
		//{
		//	presentMode = presentModes[i];
		//}
		else if (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			presentMode = presentModes[i];
			break;
		}
	}

	VkSwapchainCreateInfoKHR swapchainInfo = {};
	swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainInfo.surface = backbuffer.surfaceHandle;
	swapchainInfo.minImageCount = swapchainImageCount;
	swapchainInfo.imageFormat = surfaceFormat.format;
	swapchainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapchainInfo.imageExtent = backbuffer.extents;
	swapchainInfo.imageArrayLayers = 1;
	swapchainInfo.imageUsage = usageFlags;
	swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainInfo.queueFamilyIndexCount = 0;
	swapchainInfo.pQueueFamilyIndices = nullptr;
	swapchainInfo.preTransform = transformBits;
	swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapchainInfo.presentMode = presentMode;
	swapchainInfo.clipped = VK_TRUE;
	swapchainInfo.oldSwapchain = VK_NULL_HANDLE; // No need for this sandbox atm

	result = vkCreateSwapchainKHR(deviceInternal.device, &swapchainInfo, nullptr, &backbuffer.swapchainHandle);
	CHECK_VK(result);

	// Creating image views for consistent use
	u32 imageCount;
	vkGetSwapchainImagesKHR(deviceInternal.device, backbuffer.swapchainHandle, &imageCount, nullptr);
	DynamicArray<VkImage> backbufferImages(imageCount);
	vkGetSwapchainImagesKHR(deviceInternal.device, backbuffer.swapchainHandle, &imageCount, backbufferImages.GetData());

	backbuffer.images.Reserve(imageCount);
	backbuffer.views.Reserve(imageCount);
	// Hook these swapchain views into device internal views
	for (u32 i = 0; i < imageCount; ++i)
	{
		// Image set up
		u32 imageHandleIndex = PopFreeHandleIndex(deviceInternal.imageResourceHandlePool);
		Assert(imageHandleIndex != InvalidHandleIndex);
		ImageInternal& imgInternal = GetImageInternalFromIndex(deviceInternal, imageHandleIndex);
		imgInternal.image = backbufferImages[i];
		// image allocation is backed by the VkSwapchain, no need for this to be valid
		imgInternal.allocation = VK_NULL_HANDLE;
		// Swapchain doesn't have mips
		imgInternal.access.Resize(1);
		// The current access pattern of the image is not known, undefined is ok
		imgInternal.access[0] = Apparition::ImageAccess::Undefined;
		imgInternal.format = backbuffer.format;
		backbuffer.images.Add(imageHandleIndex);

		// ImageView set up
		u32 imageViewHandleIndex = PopFreeHandleIndex(deviceInternal.imageViewResourceHandlePool);
		Assert(imageViewHandleIndex != InvalidHandleIndex);
		// No need to get the index gen because we're kind of backdooring the system. We only care about index

		VkImageViewCreateInfo viewInfo;
		Vk::ZeroInfoStruct(viewInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.image = backbufferImages[i];
		viewInfo.format = ApparitionFormatToVk(backbuffer.format);
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.a = VK_COMPONENT_SWIZZLE_IDENTITY
		};
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.levelCount = 1;
		VkImageView backbufferView = VK_NULL_HANDLE;
		result = vkCreateImageView(deviceInternal.device, &viewInfo, nullptr, &backbufferView);
		CHECK_VK(result);

		ImageViewInternal& viewInternal = GetImageViewInternalFromIndex(deviceInternal, imageViewHandleIndex);
		viewInternal.imageView = backbufferView;
		viewInternal.imageIndex = imageHandleIndex;

		backbuffer.views.Add(imageViewHandleIndex);
	}

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	Vk::ZeroInfoStruct(semaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO);
	for (u32 i = 0; i < Backbuffer::numSwapchainImages; ++i)
	{
		result = vkCreateSemaphore(deviceInternal.device, &semaphoreCreateInfo, nullptr, &backbuffer.acquireImageSemaphores[i]);
		CHECK_VK(result);
		result = vkCreateSemaphore(deviceInternal.device, &semaphoreCreateInfo, nullptr, &backbuffer.submitRenderSemaphores[i]);
		CHECK_VK(result);

		result = vkCreateSemaphore(deviceInternal.device, &semaphoreCreateInfo, nullptr, &backbuffer.isImageAvailableSem);
		CHECK_VK(result);
		result = vkCreateSemaphore(deviceInternal.device, &semaphoreCreateInfo, nullptr, &backbuffer.hasRenderingFinishedSem);
		CHECK_VK(result);
	}

	// TODO - Log that backbuffer data has been created
}

void DeviceManager::TeardownBackbuffer(Apparition::Device device)
{
	using namespace Apparition;

	DeviceInternal& deviceInternal = DeviceInternalFrom(device);

	Backbuffer& backbuffer = deviceInternal.backbuffer;

	vkDestroySemaphore(deviceInternal.device, backbuffer.isImageAvailableSem, nullptr);
	vkDestroySemaphore(deviceInternal.device, backbuffer.hasRenderingFinishedSem, nullptr);

	for (u32 viewIndex : backbuffer.views)
	{
		VkImageView imageView = deviceInternal.imageViewResources[viewIndex].imageView;
		vkDestroyImageView(deviceInternal.device, imageView, nullptr);
		PushFreedHandleIndex(deviceInternal.imageViewResourceHandlePool, viewIndex);
	}
	backbuffer.views.Clear();
	backbuffer.images.Clear();

	vkDestroySwapchainKHR(deviceInternal.device, backbuffer.swapchainHandle, nullptr);
	vkDestroySurfaceKHR(instance, backbuffer.surfaceHandle, nullptr);
}

BackbufferStatus DeviceManager::AcquireNextBackbufferImage(Device device)
{
	DeviceInternal& deviceInternal = DeviceInternalFrom(device);
	Backbuffer& backbuffer = deviceInternal.backbuffer;

	u32 imageIndex;
	VkResult result = vkAcquireNextImageKHR(deviceInternal.device, backbuffer.swapchainHandle, UINT64_MAX, backbuffer.isImageAvailableSem, VK_NULL_HANDLE, &imageIndex);
	backbuffer.currentImageIndex = imageIndex;

	// Set image index so that we can use it later on in the render

	BackbufferStatus status;
	if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
	{
		status = BackbufferStatus::Ready;
	}
	else if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		status = BackbufferStatus::Recreate;
	}
	else
	{
		status = BackbufferStatus::Unavailable;
	}

	return status;
}

ImageView DeviceManager::GetBackbufferImageView(Device device)
{
	DeviceInternal& deviceInternal = DeviceInternalFrom(device);
	Backbuffer& backbuffer = deviceInternal.backbuffer;
	u32 viewIndex = backbuffer.currentImageIndex;
	Assert(backbuffer.views.IsIndexValid(viewIndex));
	u64 viewHandle = ((u64)device.handle << DEVICE_INDEX_SHIFT)
		| ((u64)BackBufferHandleDecorator << POOL_INDEX_SHIFT)
		| (backbuffer.views[viewIndex] & RESOURCE_INDEX_MASK);
	return ImageView{viewHandle};
}

Image DeviceManager::GetAcquiredBackbufferImage(Device device)
{
	DeviceInternal& deviceInternal = DeviceInternalFrom(device);
	Backbuffer& backbuffer = deviceInternal.backbuffer;
	u32 imgIndex = backbuffer.currentImageIndex;
	Assert(backbuffer.images.IsIndexValid(imgIndex));
	u64 imgHandle = ((u64)device.handle << DEVICE_INDEX_SHIFT)
		| ((u64)BackBufferHandleDecorator << POOL_INDEX_SHIFT)
		| (backbuffer.images[imgIndex] & RESOURCE_INDEX_MASK);
	return Image{ imgHandle };
}


