#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/Backbuffer.h"
#include "Apparition/Buffer.h"
#include "Apparition/CommandBuffer.h"
#include "Apparition/Device.h"
#include "Apparition/Queue.h"
#include "BasicTypes/Function.hpp"
#include "Containers/DynamicArray.hpp"
#include "VulkanDefinitions.h"

using namespace Apparition;

class CommandBufferManager;
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

	DeviceInternal& GetDeviceInternals(Device deviceHandle);
	DeviceInternal& GetDeviceInternals(u32 deviceIndex);
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
	const DynamicArray<QueueInternal>& GetQueueArray(DeviceInternal& deviceInternal, u32 queueFamilyIndex);
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
#pragma endregion

#pragma region Command Buffer
	CommandPool CreateCommandPool(Device deviceHandle, const CommandPoolCreationParams& params);
	void DestroyCommandPool(CommandPool commandPoolHandle);

	CommandBuffer AllocateCommandBuffer(CommandPool commandPoolHandle, const CommandBufferAllocParams& params);
	void FreeCommandBuffer(CommandBuffer commandBufferHandle);

	// TEMP
	VkCommandBuffer GetCommandBufferHandle(CommandBuffer cbHandle);
#pragma endregion

#pragma region Resources
	Buffer CreateBuffer(Device device, const BufferCreationParams& params);
	void DestroyBuffer(Buffer buffer);

	//RetVal GetBufferDescription(Buffer buffer) const;
#pragma endregion
private:
	UserValidationCallbackData userValidation;
	DynamicArray<DeviceInternal> deviceInternals;
	
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;

	CommandBufferManager* cmdBufferManager = nullptr;

	static inline u64 NextDeviceHandle = 1;
};