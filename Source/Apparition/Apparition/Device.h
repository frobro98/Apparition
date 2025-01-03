#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/FunctionRef.hpp"
#include "Containers/DynamicArray.hpp"

struct VkPhysicalDeviceFeatures;
struct VkDeviceQueueCreateInfo;
struct VkQueueFamilyProperties;

namespace Apparition
{

// TODO - Move this
static constexpr u32 InvalidHandle = 0;

struct DeviceHandle
{
	u32 Handle;
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
	u32 computeSupport : 1;
	u32 transferSupport : 1;
};

// ---- Device Functionality ----

// No need to expose Vulkan Instance creation, will check upon creation of a device
NODISCARD DeviceHandle CreateDevice(const DeviceCreationParams& Params);
void DestroyDevice(DeviceHandle deviceHandle);
void GetPhysicalDevice(DeviceHandle deviceHandle);
void GetDeviceFormatProperties(DeviceHandle deviceHandle);

}