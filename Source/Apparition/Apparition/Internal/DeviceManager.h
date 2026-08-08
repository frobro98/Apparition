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
		AptnValidationDelegate delegate;
		void* userData;
	};

public:
	~DeviceManager();

	void Initialize(const AptnInitializeParams& params);
	void Deinitialize();

#pragma region Device Management
	AptnDevice CreateDevice(const AptnDeviceCreationParams& params);
	void DestroyDevice(AptnDevice deviceHandle);

	DeviceInternal& DeviceInternalFrom(AptnDevice deviceHandle);
	DeviceInternal& DeviceInternalFrom(u32 deviceIndex);
private:
	void InitializeDeviceHandlePools(DeviceInternal& deviceInternal);
public:
#pragma endregion

#pragma region Queue
	AptnQueue AllocateGraphicsQueue(AptnDevice device);
	AptnQueue AllocateTransferQueue(AptnDevice device);
	//Queue AllocateComputeQueue(Device device);
	void FreeQueue(AptnQueue queue);

	bool CanAllocateQueue(const DeviceInternal& deviceInternal, u32 queueFamilyIndex) const;
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
	void SetupBackbuffer(AptnDevice device, const AptnBackbufferSetupParams& params);
	void TeardownBackbuffer(AptnDevice device);

	AptnBackbufferStatus AcquireNextBackbufferImage(AptnDevice device);
	AptnImageView GetBackbufferImageView(AptnDevice device);
	AptnImage GetAcquiredBackbufferImage(AptnDevice device);

public:
#pragma endregion

#pragma region Command Buffer
	AptnCommandPool CreateCommandPool(AptnDevice deviceHandle, const AptnCommandPoolCreationParams& params);
	void DestroyCommandPool(AptnCommandPool commandPoolHandle);

	AptnCommandBuffer AllocateCommandBuffer(AptnCommandPool commandPoolHandle, const AptnCommandBufferAllocParams& params);
	void FreeCommandBuffer(AptnCommandBuffer commandBufferHandle);
	void ResetCommandBuffer(AptnCommandBuffer commandBuffer);

	// TEMP
	VkCommandBuffer GetCommandBufferHandle(AptnCommandBuffer cbHandle);
#pragma endregion

#pragma region Resources
	AptnBuffer CreateBuffer(AptnDevice device, const AptnBufferCreationParams& params);
	void DestroyBuffer(AptnBuffer buffer);

	AptnImage CreateImage(AptnDevice device, const AptnImageCreationParams& params);
	void DestroyImage(AptnImage image);

	AptnImageView CreateImageView(AptnImage image, const AptnImageViewCreationParams& params);
	void DestroyImageView(AptnImageView imageView);

	AptnSampler CreateSampler(AptnDevice device, const AptnSamplerCreationParams& params);
	void DestroySampler(AptnSampler sampler);

#pragma endregion

#pragma region Pipeline State
	AptnVertexInputPipelineState CreateVertexInputPipelineState(AptnDevice device, const AptnVertexInputPipelineStateCreationParams& params);
	AptnPrerasterShadersPipelineState CreatePrerasterShadersPipelineState(AptnDevice device, const AptnPreRasterShadersPipelineStateCreationParams& params);
	AptnFragmentShaderPipelineState CreateFragmentShaderPipelineState(AptnDevice device, const AptnFragmentShaderPipelineStateCreationParams& params);
	AptnFragmentOutputPipelineState CreateFragmentOutputPipelineState(AptnDevice device, const AptnFragmentOutputPipelineStateCreationParams& params);

	void DestroyVertexInputPipelineState(AptnVertexInputPipelineState state);
	void DestroyPrerasterShadersPipelineState(AptnPrerasterShadersPipelineState state);
	void DestroyFragmentShaderPipelineState(AptnFragmentShaderPipelineState state);
	void DestroyFragmentOutputPipelineState(AptnFragmentOutputPipelineState state);

	AptnPipeline CreatePipeline(AptnDevice device, const AptnPipelineCreationParams& params);
	void DestroyPipeline(AptnPipeline pipeline);
#pragma endregion

#pragma region Descriptor Set
	AptnDescriptorSetLayout CreateDescriptorSetLayout(AptnDevice device, const AptnDescriptorSetLayoutCreationParams& params);
	void DestroyDescriptorSetLayout(AptnDescriptorSetLayout descriptorSetLayout);

	AptnDescriptorPool CreateDescriptorPool(AptnDevice device, const AptnDescriptorPoolCreationParams& params);
	void DestroyDescriptorPool(AptnDescriptorPool descriptorPool);

	AptnDescriptorSet AllocateDescriptorSet(AptnDescriptorPool descriptorPool, const AptnDescriptorSetAllocParams& allocParams);
	void FreeDescriptorSet(AptnDescriptorSet descriptorSet);
	void AllocateDescriptorSets(AptnDescriptorPool descriptorPool, const DynamicArray<AptnDescriptorSetAllocParams>& allocParams);
	void FreeDescriptorSets(const DynamicArray<AptnDescriptorSet> descriptorSets);

	void UpdateDescriptorSets(const DynamicArray<AptnUpdateDescriptorSetDesc>& descriptorSetUpdates);
#pragma endregion
private:
	UserValidationCallbackData userValidation;
	DynamicArray<DeviceInternal> deviceInternals;
	
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;

	static inline u64 NextDeviceHandle = 1;
};