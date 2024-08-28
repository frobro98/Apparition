// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"

class VulkanDevice;
struct SamplerDescription;

namespace Vk
{
	VkInstanceCreateInfo InstanceInfo(
		const tchar* const* instanceLayers, u32 numLayers,
		const tchar* const* instanceExtensions, u32 numExtensions,
		const void* additionalData = nullptr
	);

	VkDeviceQueueCreateInfo DeviceQueueInfo(
		u32 queueFamilyIndex, 
		u32 numQueues, 
		const f32* queuePriorities
	);

	VkDeviceCreateInfo DeviceInfo(
		const VkDeviceQueueCreateInfo* queueInfo, u32 numQueueInfos,
		const tchar** deviceExtensions, u32 numExtensions,
		const VkPhysicalDeviceFeatures& deviceFeatures
	);

//#if M_DEBUG
	VkDebugReportCallbackCreateInfoEXT DebugReportCallbackInfo(
		PFN_vkDebugReportCallbackEXT debugFunc,
		VkDebugReportFlagsEXT debugFlags,
		void* userData = 0
	);
//#endif // M_DEBUG

#ifdef _WIN32
	VkWin32SurfaceCreateInfoKHR SurfaceInfo(HINSTANCE hInstance, HWND hWnd);
#endif

	VkImageViewCreateInfo ImageViewInfo(
		VkImage image, u32 mipLevels, VkFormat format, VkImageAspectFlags aspectFlags
	);

	VkRenderPassCreateInfo RenderPassInfo(
		const VkAttachmentDescription* attachments, u32 numAttachments,
		const VkSubpassDescription* subpasses, u32 numSubpasses,
		const VkSubpassDependency* dependencies = nullptr, u32 numDependencies = 0
	);

	VkSamplerCreateInfo SamplerInfo(const SamplerDescription& params);

	namespace create
	{
		VkImageView ImageView(
			const VulkanDevice* device, 
			VkImage image, 
			VkFormat format, 
			VkImageAspectFlags aspectFlags
		);
	}
}
