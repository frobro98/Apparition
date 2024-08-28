// Copyright 2020, Nathan Blane

#pragma once

#include "Graphics/GraphicsResourceDefinitions.hpp"
#include "VulkanDefinitions.h"

class VulkanSwapchain;
class VulkanSurface;
class VulkanDevice;
struct VulkanTexture;

struct VulkanBackBuffer
{
	VulkanTexture* colorTexture;
};

// TODO - Figure out how to represent native resources. 
// Currently, there can should either be a table that has handles to resources or derive from set resources essentially for type safety

// Represents the viewport of the entire window
class VulkanViewport final : public NativeViewport
{
public:
	VulkanViewport(VulkanDevice& device, VkInstance instance, void* windowHandle, u32 viewWidth, u32 viewHeight);
	~VulkanViewport();

	void Resize(u32 newViewWidth, u32 newViewHeight);

	void AcquireBackBuffer();
	void SubmitFrame();
	void PresentFrame();

	inline VulkanTexture& GetBackBuffer() const { return *backBufferTexture; }
	inline u32 GetWidth() const { return viewportWidth; }
	inline u32 GetHeight() const { return viewportHeight; }

private:
	VulkanTexture* backBufferTexture = nullptr;
	VulkanDevice& logicalDevice;
	VulkanSurface* viewSurface;
	VulkanSwapchain* swapchain;
	u32 viewportWidth, viewportHeight;
};