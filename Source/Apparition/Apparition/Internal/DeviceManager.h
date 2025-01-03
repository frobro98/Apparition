#pragma once

#include "Apparition/Device.h"
#include "BasicTypes/Function.hpp"
#include "Containers/Map.h"
#include "VulkanDefinitions.h"

struct DeviceInternals
{
	VkDevice device = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	u32 graphicsFamilyIndex = 0;
	u32 transferFamilyIndex = 0;
	u32 computeFamilyIndex = 0;
};

class DeviceManager
{
public:
	static DeviceManager& Get();

	DeviceManager();

#pragma region Device Management
	Apparition::DeviceHandle CreateDevice(const Apparition::DeviceCreationParams& params);
	void DestroyDevice(Apparition::DeviceHandle deviceHandle);
#pragma endregion

#pragma region Debug Callback
	template <typename Func>
	void SetDebugCallback(Func&& func)
	{
		DebugCallback = FORWARD(Func, func);
	}

	bool IsDebugFunctionSet() const { return DebugCallback; }

	bool BroadcastDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData);
#pragma endregion

private:
	Function<bool(VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT*,
		void*)> DebugCallback;
	Map<u32, DeviceInternals> vulkanDeviceDataMap;
	
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerHandle = VK_NULL_HANDLE;
};