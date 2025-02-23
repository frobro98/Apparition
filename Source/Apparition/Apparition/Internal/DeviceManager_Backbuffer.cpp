
#include "DeviceManager.h"

#include "Apparition/Internal/VulkanInfos.h"

void DeviceManager::SetupBackbuffer(Apparition::DeviceHandle device, const Apparition::BackbufferSetupParams& params)
{
    using namespace Apparition;

    DeviceInternals* internals = GetDeviceInternals(device);
    Assert(internals);

    if (internals->backbuffer.swapchainHandle != VK_NULL_HANDLE)
    {
		// TODO - Log that backbuffer is already set up
		return;
    }

	Backbuffer& backbuffer = internals->backbuffer;
    // Create surface
    
    // NOTE - Only supports Windows surfaces right now. No need for anything else atm
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    Vk::ZeroInfoStruct(surfaceCreateInfo, VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR);
    surfaceCreateInfo.hinstance = reinterpret_cast<HINSTANCE>(GetModuleHandle(nullptr));
    surfaceCreateInfo.hwnd = reinterpret_cast<HWND>(params.wndHandle);
    VkResult result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &backbuffer.surfaceHandle);
    CHECK_VK(result);

    VkBool32 presentationSupported = VK_FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(internals->physicalDevice, internals->graphicsFamilyIndex, backbuffer.surfaceHandle, &presentationSupported);
    if (presentationSupported != VK_TRUE)
    {
        // Error and return
		return;
    }

    // Needs surface information for swapchain setup
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(internals->physicalDevice, backbuffer.surfaceHandle, &surfaceCapabilities);
    CHECK_VK(result);

    DynamicArray<VkSurfaceFormatKHR> surfaceFormats;
    u32 formatCount = 0;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(internals->physicalDevice, backbuffer.surfaceHandle, &formatCount, nullptr);
    CHECK_VK(result);
    surfaceFormats.Resize(formatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(internals->physicalDevice, backbuffer.surfaceHandle, &formatCount, surfaceFormats.GetData());
    CHECK_VK(result);

    DynamicArray<VkPresentModeKHR> presentModes;
    u32 presentModeCount;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(internals->physicalDevice, backbuffer.surfaceHandle, &presentModeCount, nullptr);
    CHECK_VK(result);
    presentModes.Resize(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(internals->physicalDevice, backbuffer.surfaceHandle, &presentModeCount, presentModes.GetData());
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
	backbuffer.format = surfaceFormat.format;

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

	result = vkCreateSwapchainKHR(internals->device, &swapchainInfo, nullptr, &backbuffer.swapchainHandle);
	CHECK_VK(result);

	// Creating image views for consistent use
	u32 imageCount;
	vkGetSwapchainImagesKHR(internals->device, backbuffer.swapchainHandle, &imageCount, nullptr);
	DynamicArray<VkImage> backbufferImages(imageCount);
	vkGetSwapchainImagesKHR(internals->device, backbuffer.swapchainHandle, &imageCount, backbufferImages.GetData());

	backbuffer.views.Reserve(imageCount);
	for (VkImage image : backbufferImages)
	{
		VkImageViewCreateInfo viewInfo;
		Vk::ZeroInfoStruct(viewInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.image = image;
		viewInfo.format = backbuffer.format;
		viewInfo.components = {
			.r = VK_COMPONENT_SWIZZLE_R,
			.g = VK_COMPONENT_SWIZZLE_G,
			.b = VK_COMPONENT_SWIZZLE_B,
			.a = VK_COMPONENT_SWIZZLE_A
		};
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.layerCount = 1;
		viewInfo.subresourceRange.levelCount = 1;
		
		VkImageView backbufferView = VK_NULL_HANDLE;
		result = vkCreateImageView(internals->device, &viewInfo, nullptr, &backbufferView);
		CHECK_VK(result);

		backbuffer.views.Add(backbufferView);
	}

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	Vk::ZeroInfoStruct(semaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO);
	result = vkCreateSemaphore(internals->device, &semaphoreCreateInfo, nullptr, &backbuffer.isImageAvailableSem);
	CHECK_VK(result);
	result = vkCreateSemaphore(internals->device, &semaphoreCreateInfo, nullptr, &backbuffer.hasRenderingFinishedSem);
	CHECK_VK(result);

	// TODO - Log that backbuffer data has been created
}

void DeviceManager::TeardownBackbuffer(Apparition::DeviceHandle device)
{
	using namespace Apparition;

	DeviceInternals* internals = GetDeviceInternals(device);
	Assert(internals);

	Backbuffer& backbuffer = internals->backbuffer;

	vkDestroySemaphore(internals->device, backbuffer.isImageAvailableSem, nullptr);
	vkDestroySemaphore(internals->device, backbuffer.hasRenderingFinishedSem, nullptr);

	for (VkImageView backbufferView : backbuffer.views)
	{
		vkDestroyImageView(internals->device, backbufferView, nullptr);
	}
	backbuffer.views.Clear();

	vkDestroySwapchainKHR(internals->device, backbuffer.swapchainHandle, nullptr);
	vkDestroySurfaceKHR(instance, backbuffer.surfaceHandle, nullptr);
}