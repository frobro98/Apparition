// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Memory/MemoryFunctions.hpp"
#include "VulkanDefinitions.h"

class VulkanDevice;
struct SamplerDescription;

namespace Vk
{
template<typename T>
void ZeroInfoStruct(T& createInfo, u32 structType)
{
	static_assert(offsetof(T, sType) == 0, "sType should be first memeber of Vk create info structs");
	static_assert(sizeof(T::sType) == sizeof(u32), "sType should be compatible with u32");
	Memory::Memzero(&createInfo, sizeof(T));
	// Don't want to include vulkan header here, so let's make it an intrinsi
	(u32&)createInfo.sType = structType;
}
}
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

	//VkSamplerCreateInfo SamplerInfo(const SamplerDescription& params);

	namespace create
	{
		VkImageView ImageView(
			const VulkanDevice* device, 
			VkImage image, 
			VkFormat format, 
			VkImageAspectFlags aspectFlags
		);
	}

