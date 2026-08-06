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

namespace Apparition
{

struct Device
{
	u64 handle;
};
HANDLE_TYPE_OPERATORS(Device);

enum class QueueType
{
	Graphics,
	Compute,
	Transfer
};

struct QueueCreationParams
{
	QueueType queueType = QueueType::Graphics;
	f32 priority = 1.f;
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
	DynamicArray<QueueCreationParams> queueCreationParams;
};

// ---- Device Functionality ----

// No need to expose Vulkan Instance creation, will check upon creation of a device

NODISCARD APPARITION_API Device CreateDevice(const DeviceCreationParams& params);
APPARITION_API void DestroyDevice(Device deviceHandle);
}