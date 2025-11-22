#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "Apparition/Backbuffer.h"
#include "BasicTypes/Function.hpp"
#include "Containers/Map.h"
#include "VulkanDefinitions.h"

// VMA
#include "vma/vk_mem_alloc.h"

namespace Apparition
{
struct InitializeParams;
}

// TODO - Move these to separate header
struct Backbuffer
{
	DynamicArray<VkImageView> views;
	VkSwapchainKHR swapchainHandle = VK_NULL_HANDLE;
	VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
	VkExtent2D extents = {};
	VkFormat format = VK_FORMAT_UNDEFINED;
	VkSemaphore isImageAvailableSem = VK_NULL_HANDLE;
	VkSemaphore hasRenderingFinishedSem = VK_NULL_HANDLE;
};

struct DeviceInternals
{
	Backbuffer backbuffer{};
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VmaAllocator allocator = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
	VkCommandPool graphicsCmdPool = VK_NULL_HANDLE;
	VkCommandPool transferCmdPool = VK_NULL_HANDLE;
	VkCommandPool computeCmdPool = VK_NULL_HANDLE;
	u32 graphicsFamilyIndex = 0;
	u32 transferFamilyIndex = 0;
	u32 computeFamilyIndex = 0;
};

class DeviceManager final
{
public:
	struct UserAllocationCallbacks;
	struct UserValidationCallbackData
	{
		Apparition::ValidationDelegate delegate;
		void* userData;
	};

public:
	~DeviceManager();

	void Initialize(const Apparition::InitializeParams& params);
	void Deinitialize();

#pragma region Device Management
	Apparition::DeviceHandle CreateDevice(const Apparition::DeviceCreationParams& params);
	void DestroyDevice(Apparition::DeviceHandle deviceHandle);

	DeviceInternals* GetDeviceInternals(Apparition::DeviceHandle deviceHandle);
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

#pragma region Swapchain
	void SetupBackbuffer(Apparition::DeviceHandle device, const Apparition::BackbufferSetupParams& params);
	void TeardownBackbuffer(Apparition::DeviceHandle device);
#pragma endregion

private:
	UserValidationCallbackData userValidation;
	Map<u32, DeviceInternals> vulkanDeviceDataMap;
	
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;

	static const inline u32 InvalidDeviceHandle = 0;
	static inline u64 NextDeviceHandle = 1;
};