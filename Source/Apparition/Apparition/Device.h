#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/FunctionRef.hpp"
#include "Containers/DynamicArray.hpp"
#include "Apparition/ApparitionAPI.hpp"

struct VkPhysicalDeviceFeatures;
struct VkDeviceQueueCreateInfo;
struct VkQueueFamilyProperties;
struct VkDevice_T;
typedef struct VkDevice_T* VkDevice;

namespace Apparition
{

struct Device
{
	u64 handle;
};

// ONLY SUPPORTS DISCRETE GPUS CURRENTLY. WILL CHANGE TO PRIORITIZING DISCRETE
// ONLY SUPPORTS DEVICE THAT WILL PRESENT
// DEVICE CREATION CALLBACK ALLOWS CUSTOM DEVICE OPTIONS
struct DeviceCreationParams
{
	// Optional callback to set custom features on your device
	FunctionRef<void
		(const VkPhysicalDeviceFeatures& /*supportedFeatures*/, 
		 VkPhysicalDeviceFeatures& /*enabledDeviceFeatures*/)> featureSetupCallback;
	// Optional callback to allow for custom queue setup by user
	FunctionRef<
		DynamicArray<VkDeviceQueueCreateInfo>
			(const DynamicArray<VkQueueFamilyProperties>&, 
			 u32 /*gfxQueueIdx*/, u32 /*tfrQueueIdx*/, u32 /*compQueueIdx*/)> queueCreationCallback;
	u32 graphicsSupport : 1;
	u32 computeSupport : 1;
	u32 transferSupport : 1;
};

// ---- Device Functionality ----

// No need to expose Vulkan Instance creation, will check upon creation of a device

// TODO - 
NODISCARD APPARITION_API Device CreateDevice(const DeviceCreationParams& params);
APPARITION_API void DestroyDevice(Device deviceHandle);
APPARITION_API u32 GetGraphicsQueueIndex(Device deviceHandle);
APPARITION_API u32 GetComputeQueueIndex(Device deviceHandle);
APPARITION_API u32 GetTransferQueueIndex(Device deviceHandle);
// TODO - Expose native functionality of Vulkan in a separate file
APPARITION_API VkDevice GetVulkanDevice(Device deviceHandle);
}