#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/ImageDescription.h"
#include "Containers/DynamicArray.hpp"
#include "DeviceManager.h"
#include "HandleDefinitions.h"
#include "HandlePool.h"
#include "VulkanDefinitions.h"

class DeviceManager;
class CommandBufferManager;

struct ApparitionInternals
{
	Apparition::AllocationCallbacks allocCallbacks;
	DeviceManager* deviceManager = nullptr;
	Apparition::ValidationDelegate userValidationDelegate;
	void* validationUserData = nullptr;
};

extern ApparitionInternals apparition;

// TODO - Move these to separate header
struct Backbuffer
{
	static constexpr inline u32 numSwapchainImages = 2;
	DynamicArray<u32> views;
	DynamicArray<u32> images;
	VkSwapchainKHR swapchainHandle = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	VkExtent2D extents = {};
	Apparition::ImageFormat::Type format = Apparition::ImageFormat::Invalid;
	VkSemaphore acquireImageSemaphores[numSwapchainImages];
	VkSemaphore submitRenderSemaphores[numSwapchainImages];
	VkSemaphore isImageAvailableSem = VK_NULL_HANDLE;
	VkSemaphore hasRenderingFinishedSem = VK_NULL_HANDLE;
	u32 currentImageIndex = 0;
};

struct QueueInternal
{
	VkQueue queue = VK_NULL_HANDLE;
	// Maybe have the type of queue this is?
};

struct CommandBufferInternal
{
	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	// General state information.
	// TODO: Will need more specific state info about where we are in the CB's lifetime
	bool hasBegun = false;
};

struct CommandPoolInternal
{
	VkCommandPool cmdPool = VK_NULL_HANDLE;
	u32 queueFamilyIndex = 0;
};

struct BufferInternal
{
	VkBuffer buffer = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;
	bool isMappable = false;
};

struct ImageInternal
{
	VkImage image = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;

	// Image formatting and access
	// Access per mip level of the image. Defaults all to Undefined until access occurs
	DynamicArray<Apparition::ImageAccess::Type> access;
	Apparition::ImageFormat::Type format;
};

struct ImageViewInternal
{
	VkImageView imageView = VK_NULL_HANDLE;
	u32 imageIndex = UINT32_MAX;

	// View information
};

struct SamplerInternal
{
	VkSampler sampler = VK_NULL_HANDLE;
};

struct VertexInputPipelineStateInternal
{
	VkPipeline state = VK_NULL_HANDLE;
};

struct PrerasterShadersPipelineStateInternal
{
	VkPipeline state = VK_NULL_HANDLE;
};

struct FragmentShaderPipelineStateInternal
{
	VkPipeline state = VK_NULL_HANDLE;
};

struct FragmentOutputPipelineStateInternal
{
	VkPipeline state = VK_NULL_HANDLE;
};

struct PipelineInternal
{
	VkPipeline pipeline = VK_NULL_HANDLE;

	// States used in pipeline construction
	u32 vertexInputIndex = 0;
	u32 prerasterShadersIndex = 0;
	u32 fragmentShaderIndex = 0;
	u32 fragmentOutputIndex = 0;
};

struct DescriptorSetLayoutInternal
{
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
};

struct DescriptorPoolInternal
{
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
};

struct DescriptorSetInternal
{
	VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

struct DeviceInternal
{
	Backbuffer backbuffer{};
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VmaAllocator allocator = VK_NULL_HANDLE;

	// The sizes of these pools relates to internal queue family properties
	HandlePool graphicsQueueHandlePool;
	HandlePool transferQueueHandlePool;
	HandlePool computeQueueHandlePool;
	// This contains queues that are live within the ecosystem, not all queues available
	DynamicArray<QueueInternal> graphicsQueues;
	DynamicArray<QueueInternal> transferQueues;
	DynamicArray<QueueInternal> computeQueues;

	HandlePool commandPoolsHandlePool;
	HandlePool commandBufferHandlePool;
	HandlePool bufferResourceHandlePool;
	HandlePool imageResourceHandlePool;
	HandlePool imageViewResourceHandlePool;
	HandlePool samplerResourceHandlePool;
	HandlePool vertexInputResourceHandlePool;
	HandlePool prerasterShadersResourceHandlePool;
	HandlePool fragmentShaderResourceHandlePool;
	HandlePool fragmentOutputResourceHandlePool;
	HandlePool pipelineResourceHandlePool;
	HandlePool descriptorSetLayoutHandlePools;
	HandlePool descriptorPoolHandlePools;
	HandlePool descriptorSetHandlePools;

	DynamicArray<CommandPoolInternal> commandPools;
	DynamicArray<CommandBufferInternal> commandBuffers;
	DynamicArray<BufferInternal> bufferResources;
	DynamicArray<ImageInternal> imageResources;
	DynamicArray<ImageViewInternal> imageViewResources;
	DynamicArray<SamplerInternal> samplerResources;
	DynamicArray<VertexInputPipelineStateInternal> vertexInputResources;
	DynamicArray<PrerasterShadersPipelineStateInternal> prerasterShadersResources;
	DynamicArray<FragmentShaderPipelineStateInternal> fragmentShaderResources;
	DynamicArray<FragmentOutputPipelineStateInternal> fragmentOutputResources;
	DynamicArray<PipelineInternal> pipelineResources;
	DynamicArray<DescriptorSetLayoutInternal> descriptorSetLayoutResources;
	DynamicArray<DescriptorPoolInternal> descriptorPoolResources;
	DynamicArray<DescriptorSetInternal> descriptorSetResources;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	u32 graphicsFamilyIndex = 0;
	u32 transferFamilyIndex = 0;
	u32 computeFamilyIndex = 0;
};

REGISTER_HANDLE_TYPE(CommandPool, commandPools);
REGISTER_HANDLE_TYPE(CommandBuffer, commandBuffers);
// Resource Handles
REGISTER_HANDLE_TYPE(Buffer, bufferResources);
REGISTER_HANDLE_TYPE(Image, imageResources);
REGISTER_HANDLE_TYPE(ImageView, imageViewResources);
REGISTER_HANDLE_TYPE(Sampler, samplerResources);
// Pipeline Handles
REGISTER_HANDLE_TYPE(VertexInputPipelineState, vertexInputResources);
REGISTER_HANDLE_TYPE(PrerasterShadersPipelineState, prerasterShadersResources);
REGISTER_HANDLE_TYPE(FragmentShaderPipelineState, fragmentShaderResources);
REGISTER_HANDLE_TYPE(FragmentOutputPipelineState, fragmentOutputResources);
REGISTER_HANDLE_TYPE(Pipeline, pipelineResources);
// DescriptorSet Handles
REGISTER_HANDLE_TYPE(DescriptorSetLayout, descriptorSetLayoutResources);
REGISTER_HANDLE_TYPE(DescriptorPool, descriptorPoolResources);
REGISTER_HANDLE_TYPE(DescriptorSet, descriptorSetResources);

