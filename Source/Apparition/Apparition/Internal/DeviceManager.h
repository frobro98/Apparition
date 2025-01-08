#pragma once

#include "Apparition/ApparitionCore.h"
#include "Apparition/Device.h"
#include "BasicTypes/Function.hpp"
#include "Containers/Map.h"
#include "VulkanDefinitions.h"

namespace Apparition
{
struct InitializeParams;
}

void InitializeDeviceManager(const Apparition::InitializeParams& params);

class DeviceManager
{
public:
	static DeviceManager& Get();

	DeviceManager(const Apparition::InitializeParams& params);

#pragma region Device Management
	Apparition::DeviceHandle CreateDevice(const Apparition::DeviceCreationParams& params);
	void DestroyDevice(Apparition::DeviceHandle deviceHandle);
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

private:
	struct DeviceInternals
	{
		VkDevice device = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		u32 graphicsFamilyIndex = 0;
		u32 transferFamilyIndex = 0;
		u32 computeFamilyIndex = 0;
	};

	struct UserAllocationCallbacks;
	struct UserValidationCallbackData
	{
		Apparition::ValidationDelegate delegate;
		void* userData;
	};

private:
	UserValidationCallbackData userValidation;
	Map<u32, DeviceInternals> vulkanDeviceDataMap;
	
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;

	static const inline u32 InvalidDeviceHandle = 0;
	static inline u32 NextDeviceHandle = 1;
};