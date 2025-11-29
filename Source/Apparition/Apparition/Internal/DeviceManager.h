#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/Backbuffer.h"
#include "Apparition/CommandBuffer.h"
#include "BasicTypes/Function.hpp"
#include "Containers/Map.h"
#include "VulkanDefinitions.h"

using namespace Apparition;

class CommandBufferManager;
struct DeviceInternal;

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
	Apparition::DeviceHandle CreateDevice(const DeviceCreationParams& params);
	void DestroyDevice(DeviceHandle deviceHandle);

	DeviceInternal& GetDeviceInternals(DeviceHandle deviceHandle);
private:
	void InitializeDeviceHandlePools(DeviceInternal& deviceInternal);
#pragma endregion

#pragma region Debug Callback
public:
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
	void SetupBackbuffer(DeviceHandle device, const BackbufferSetupParams& params);
	void TeardownBackbuffer(DeviceHandle device);
#pragma endregion

#pragma region Command Buffer
	CommandPoolHandle CreateCommandPool(DeviceHandle deviceHandle, const CommandPoolCreationParams& params);
	void DestroyCommandPool(CommandPoolHandle commandPoolHandle);

	CommandBufferHandle AllocateCommandBuffer(CommandPoolHandle commandPoolHandle, const CommandBufferAllocParams& params);
	void FreeCommandBuffer(CommandBufferHandle commandBufferHandle);

	// TEMP
	VkCommandBuffer GetCommandBufferHandle(CommandBufferHandle cbHandle);
#pragma endregion

private:
	UserValidationCallbackData userValidation;
	DynamicArray<DeviceInternal> deviceInternals;
	
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;

	CommandBufferManager* cmdBufferManager = nullptr;

	static inline u64 NextDeviceHandle = 1;
};