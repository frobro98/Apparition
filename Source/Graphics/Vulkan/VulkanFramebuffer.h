// Copyright 2020, Nathan Blane

#pragma once

#include "VulkanDefinitions.h"
#include "BasicTypes/Uncopyable.hpp"
#include "Containers/DynamicArray.hpp"
#include "BasicTypes/Extents.hpp"
#include "Graphics/RenderPassAttachments.hpp"

class VulkanDevice;
struct VulkanRenderingLayout;
class VulkanRenderPass;
struct VulkanTexture;
struct NativeRenderTargets;
struct VulkanTexture;

class VulkanFramebuffer : private Uncopyable
{
public:
	VulkanFramebuffer(const VulkanDevice& device);
	~VulkanFramebuffer();

	void Initialize(const VulkanRenderingLayout& targetDesc, const NativeRenderTargets& renderTextures, VulkanRenderPass* renderPass_);

	bool ContainsRT(const VulkanTexture& texture);
	bool ContainsRTs(const NativeRenderTargets& renderTextures);

	u32 GetAttachmentCount() const { return viewAttachments.Size(); }
	bool HasDepthAttachment() const { return nativeTargets.depthTarget != nullptr; }

	inline VkFramebuffer GetNativeHandle() const { return frameBuffer; }
	inline VulkanRenderPass* GetRenderPass() const { return renderPass; }
	inline u32 GetWidth() const { return extents.width; }
	inline u32 GetHeight() const { return extents.height; }

private:
	NativeRenderTargets nativeTargets{};
	DynamicArray<VkImageView> viewAttachments;
	VkFramebuffer frameBuffer = VK_NULL_HANDLE;
	VkExtent2D extents;
	const VulkanDevice* logicalDevice;
	VulkanRenderPass* renderPass;
};