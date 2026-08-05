#pragma once

#include "BasicTypes/Function.hpp"
#include "Containers/DynamicArray.hpp"

#include "Apparition/ApparitionCore.h"
#include "Apparition/Backbuffer.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Device.h"
#include "Apparition/Pipeline.h"
#include "Apparition/Queue.h"

#include "Apparition/Internal/VulkanDefinitions.h"

using namespace Apparition;

struct DeviceInternal;
struct HandlePool;
struct QueueInternal;

class DeviceManager final
{
public:
	struct UserAllocationCallbacks;
	struct UserValidationCallbackData
	{
		ValidationDelegate delegate;
		void* userData;
	};

public:
	~DeviceManager();

	void Initialize(const InitializeParams& params);
	void Deinitialize();

#pragma region Device Management
	Apparition::Device CreateDevice(const DeviceCreationParams& params);
	void DestroyDevice(Device deviceHandle);

	DeviceInternal& DeviceInternalFrom(Device deviceHandle);
	DeviceInternal& DeviceInternalFrom(u32 deviceIndex);
private:
	void InitializeDeviceHandlePools(DeviceInternal& deviceInternal);
public:
#pragma endregion

#pragma region Queue
	Queue AllocateGraphicsQueue(Device device);
	Queue AllocateTransferQueue(Device device);
	//Queue AllocateComputeQueue(Device device);
	void FreeQueue(Queue queue);

	HandlePool& GetQueueHandlePool(DeviceInternal& deviceInternal, u32 queueFamilyIndex);
	const DynamicArray<QueueInternal>& GetQueueArray(const DeviceInternal& deviceInternal, u32 queueFamilyIndex) const;
#pragma endregion

#pragma region Debug Callback
	template <typename Func>
	void SetDebugCallback(Func&& func, void* userData)
	{
		userValidation.delegate = FORWARD(Func, func);
		userValidation.userData = userData;
	}

	bool IsDebugFunctionSet() const { return !!userValidation.delegate; }

	bool BroadcastDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
#pragma endregion

#pragma region Backbuffer
	void SetupBackbuffer(Device device, const BackbufferSetupParams& params);
	void TeardownBackbuffer(Device device);

	BackbufferStatus AcquireNextBackbufferImage(Device device);
	ImageView GetBackbufferImageView(Device device);
	Image GetAcquiredBackbufferImage(Device device);

public:
#pragma endregion

#pragma region Command Buffer
	CommandPool CreateCommandPool(Device deviceHandle, const CommandPoolCreationParams& params);
	void DestroyCommandPool(CommandPool commandPoolHandle);

	CommandBuffer AllocateCommandBuffer(CommandPool commandPoolHandle, const CommandBufferAllocParams& params);
	void FreeCommandBuffer(CommandBuffer commandBufferHandle);
	void ResetCommandBuffer(CommandBuffer commandBuffer);

	// TEMP
	VkCommandBuffer GetCommandBufferHandle(CommandBuffer cbHandle);
#pragma endregion

#pragma region Resources
	Buffer CreateBuffer(Device device, const BufferCreationParams& params);
	void DestroyBuffer(Buffer buffer);

	Image CreateImage(Device device, const ImageCreationParams& params);
	void DestroyImage(Image image);

	ImageView CreateImageView(Image image, const ImageViewCreationParams& params);
	void DestroyImageView(ImageView imageView);

	Sampler CreateSampler(Device device, const SamplerCreationParams& params);
	void DestroySampler(Sampler sampler);

	//RetVal GetBufferDescription(Buffer buffer) const;

	// TEMP
	VkBuffer GetBufferHandle(Buffer bufferHandle);
#pragma endregion

#pragma region Pipeline State
	VertexInputPipelineState CreateVertexInputPipelineState(Device device, const VertexInputPipelineStateCreationParams& params);
	PrerasterShadersPipelineState CreatePrerasterShadersPipelineState(Device device, const PreRasterShadersPipelineStateCreationParams& params);
	FragmentShaderPipelineState CreateFragmentShaderPipelineState(Device device, const FragmentShaderPipelineStateCreationParams& params);
	FragmentOutputPipelineState CreateFragmentOutputPipelineState(Device device, const FragmentOutputPipelineStateCreationParams& params);

	void DestroyVertexInputPipelineState(VertexInputPipelineState state);
	void DestroyPrerasterShadersPipelineState(PrerasterShadersPipelineState state);
	void DestroyFragmentShaderPipelineState(FragmentShaderPipelineState state);
	void DestroyFragmentOutputPipelineState(FragmentOutputPipelineState state);

	Pipeline CreatePipeline(Device device, const PipelineCreationParams& params);
	void DestroyPipeline(Pipeline pipeline);
#pragma endregion

#pragma region Descriptor Set
	DescriptorSetLayout CreateDescriptorSetLayout(Device device, const DescriptorSetLayoutCreationParams& params);
	void DestroyDescriptorSetLayout(DescriptorSetLayout descriptorSetLayout);

	DescriptorPool CreateDescriptorPool(Device device, const DescriptorPoolCreationParams& params);
	void DestroyDescriptorPool(DescriptorPool descriptorPool);

	DescriptorSet AllocateDescriptorSet(DescriptorPool descriptorPool, const DescriptorSetAllocParams& allocParams);
	void FreeDescriptorSet(DescriptorSet descriptorSet);
	void AllocateDescriptorSets(DescriptorPool descriptorPool, const DynamicArray<DescriptorSetAllocParams>& allocParams);
	void FreeDescriptorSets(const DynamicArray<DescriptorSet> descriptorSets);

	void UpdateDescriptorSets(const DynamicArray<UpdateDescriptorSetDesc>& descriptorSetUpdates);
#pragma endregion
private:
	UserValidationCallbackData userValidation;
	DynamicArray<DeviceInternal> deviceInternals;
	
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;

	static inline u64 NextDeviceHandle = 1;
};