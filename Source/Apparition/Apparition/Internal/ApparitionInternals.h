#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/ApparitionDefinitions.h"
#include "DeviceManager.h"
#include "HandleDefinitions.h"
#include "HandlePool.h"
#include "VulkanDefinitions.h"

class DeviceManager;
class CommandBufferManager;

struct ApparitionInternals
{
	AptnAllocationCallbacks allocCallbacks;
	DeviceManager* deviceManager = nullptr;
	AptnValidationDelegate userValidationDelegate;
	void* validationUserData = nullptr;
};

extern ApparitionInternals apparition;

// TODO - Move these to separate header
struct Backbuffer
{
	static constexpr inline u32 numSwapchainImages = 2;
	DynamicArray<u32> views;
	DynamicArray<u32> images;
	VkSwapchainKHR vkHandle = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	VkExtent2D extents = {};
	AptnImageFormat format = AptnImageFormat::Invalid;
	VkSemaphore acquireImageSemaphores[numSwapchainImages];
	VkSemaphore submitRenderSemaphores[numSwapchainImages];
	VkSemaphore isImageAvailableSem = VK_NULL_HANDLE;
	VkSemaphore hasRenderingFinishedSem = VK_NULL_HANDLE;
	u32 currentImageIndex = 0;
};

struct QueueInternal
{
	VkQueue vkHandle = VK_NULL_HANDLE;
	u32 queueFamilyIndex = 0;
	bool canPresent = false;
	// Maybe have the type of queue this is?
};

struct CommandBufferInternal
{
	VkCommandBuffer vkHandle = VK_NULL_HANDLE;
	// General state information.
	u32 queueFamilyIndex = 0;
	// TODO: Will need more specific state info about where we are in the CB's lifetime
	bool hasBegun = false;
	bool renderBegun = false;
};

struct CommandPoolInternal
{
	VkCommandPool vkHandle = VK_NULL_HANDLE;
	u32 queueFamilyIndex = 0;
};

struct BufferInternal
{
	VkBuffer vkHandle = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VkDeviceAddress bufferDeviceAddress = 0;
	bool isMappable = false;
};

struct ImageInternal
{
	VkImage vkHandle = VK_NULL_HANDLE;
	VmaAllocation allocation = VK_NULL_HANDLE;

	// Image formatting and access
	// Access per mip level of the image. Defaults all to Undefined until access occurs
	DynamicArray<AptnImageAccess > access;
	AptnImageFormat format;
};

struct ImageViewInternal
{
	VkImageView vkHandle = VK_NULL_HANDLE;
	u32 imageIndex = UINT32_MAX;

	// View information
};

struct SamplerInternal
{
	VkSampler vkHandle = VK_NULL_HANDLE;
};

struct VertexInputPipelineStateInternal
{
	VkPipeline vkHandle = VK_NULL_HANDLE;
};

struct PrerasterShadersPipelineStateInternal
{
	VkPipeline vkHandle = VK_NULL_HANDLE;
};

struct FragmentShaderPipelineStateInternal
{
	VkPipeline vkHandle = VK_NULL_HANDLE;
};

struct FragmentOutputPipelineStateInternal
{
	VkPipeline vkHandle = VK_NULL_HANDLE;
};

struct PipelineInternal
{
	VkPipeline vkHandle = VK_NULL_HANDLE;

	// States used in pipeline construction
	u32 vertexInputIndex = 0;
	u32 prerasterShadersIndex = 0;
	u32 fragmentShaderIndex = 0;
	u32 fragmentOutputIndex = 0;
};

struct SamplerHeapInternal
{
	VkBuffer vkHandle = VK_NULL_HANDLE;
	VmaAllocation vmaAllocation = VK_NULL_HANDLE;
	VkDeviceSize heapSize = 0;
	void* pHeapStart = nullptr;
	void* pHeapEnd = nullptr;
	void* pSamplerCurrent = nullptr;
	VkDeviceAddress heapDeviceAddress = 0;
	VkDeviceSize samplerDescriptorSize = 0;
	VkDeviceSize heapReservedRange = 0;
	DynamicArray<AptnSamplerDescriptor> pendingSamplerDescriptors;
};

struct ResourceHeapInternal
{
	VkBuffer vkHandle = VK_NULL_HANDLE;
	VmaAllocation vmaAllocation = VK_NULL_HANDLE;
	VkDeviceSize heapSize = 0;
	void* heapData = nullptr;
	void* pHeapEnd = nullptr;
	void* pBDACurrent = nullptr;
	void* pImgCurrent = nullptr;
	VkDeviceAddress heapDeviceAddress = 0;
	VkDeviceSize bufferDescriptorSize = 0;
	VkDeviceSize bufferDescriptorBlockSize = 0;
	VkDeviceSize imageDescriptorSize = 0;
	VkDeviceSize imageDescriptorBlockSize = 0;
	VkDeviceSize heapReservedRange = 0;
	DynamicArray<AptnImageDescriptor> pendingImageDescriptors;
	DynamicArray<AptnBufferAddressDescriptor> pendingBufferDescriptors;
};

struct DescriptorSetLayoutInternal
{
	VkDescriptorSetLayout vkHandle = VK_NULL_HANDLE;
};

struct DescriptorPoolInternal
{
	VkDescriptorPool vkHandle = VK_NULL_HANDLE;
};

struct DescriptorSetInternal
{
	VkDescriptorSet vkHandle = VK_NULL_HANDLE;
};

struct DeviceInternal
{
	Backbuffer backbuffer{};
	VkPhysicalDeviceLimits physicalDeviceLimits;
	VkPhysicalDeviceDescriptorHeapPropertiesEXT descriptorHeapProperties;
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VmaAllocator allocator = VK_NULL_HANDLE;
	
	HandlePool queuesHandlePool;
	HandlePool commandPoolsHandlePool;
	HandlePool commandBuffersHandlePool;
	HandlePool bufferResourcesHandlePool;
	HandlePool imageResourcesHandlePool;
	HandlePool imageViewResourcesHandlePool;
	HandlePool samplerResourcesHandlePool;
	HandlePool vertexInputResourcesHandlePool;
	HandlePool prerasterShadersResourcesHandlePool;
	HandlePool fragmentShaderResourcesHandlePool;
	HandlePool fragmentOutputResourcesHandlePool;
	HandlePool pipelineResourcesHandlePool;
	HandlePool samplerHeapsHandlePool;
	HandlePool resourceHeapsHandlePool;
	HandlePool descriptorSetLayoutsHandlePool;
	HandlePool descriptorPoolsHandlePool;
	HandlePool descriptorSetsHandlePool;

	DynamicArray<QueueInternal> queues;
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
	DynamicArray<SamplerHeapInternal> samplerHeaps;
	DynamicArray<ResourceHeapInternal> resourceHeaps;
	DynamicArray<DescriptorSetLayoutInternal> descriptorSetLayouts;
	DynamicArray<DescriptorPoolInternal> descriptorPools;
	DynamicArray<DescriptorSetInternal> descriptorSets;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	u32 graphicsFamilyIndex = 0;
	u32 transferFamilyIndex = 0;
	u32 computeFamilyIndex = 0;
};

REGISTER_HANDLE_TYPE(Queue, queues);

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
// DescriptorHeap
REGISTER_HANDLE_TYPE(SamplerHeap, samplerHeaps);
REGISTER_HANDLE_TYPE(ResourceHeap, resourceHeaps);
// DescriptorSet Handles
REGISTER_HANDLE_TYPE(DescriptorSetLayout, descriptorSetLayouts);
REGISTER_HANDLE_TYPE(DescriptorPool, descriptorPools);
REGISTER_HANDLE_TYPE(DescriptorSet, descriptorSets);

