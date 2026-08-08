#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/FunctionRef.hpp"
#include "Containers/DynamicArray.hpp"
#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/ApparitionCore.h"

struct VkPhysicalDeviceFeatures;
struct VkDeviceQueueCreateInfo;
struct VkQueueFamilyProperties;
struct VkDevice_T;
typedef struct VkDevice_T* VkDevice;

HANDLE_TYPE(AptnDevice)

enum class AptnQueueType
{
	Graphics,
	Compute,
	Transfer
};

struct AptnQueueCreationParams
{
	AptnQueueType queueType = AptnQueueType::Graphics;
	f32 priority = 1.f;
};

// ONLY SUPPORTS DISCRETE GPUS CURRENTLY. WILL CHANGE TO PRIORITIZING DISCRETE
// ONLY SUPPORTS DEVICE THAT WILL PRESENT
// DEVICE CREATION CALLBACK ALLOWS CUSTOM DEVICE OPTIONS
struct AptnDeviceCreationParams
{
	// Optional callback to set custom features on your device
	FunctionRef<void
		(const VkPhysicalDeviceFeatures& /*supportedFeatures*/, 
		 VkPhysicalDeviceFeatures& /*enabledDeviceFeatures*/)> featureSetupCallback;
	// Optional callback to allow for custom queue setup by user
	DynamicArray<AptnQueueCreationParams> queueCreationParams;
};

// ---- Device Functionality ----

// No need to expose Vulkan Instance creation, will check upon creation of a device
namespace Apparition
{
NODISCARD APPARITION_API AptnDevice CreateDevice(const AptnDeviceCreationParams& params);
APPARITION_API void DestroyDevice(AptnDevice deviceHandle);
}