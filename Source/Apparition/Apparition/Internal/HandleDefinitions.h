#pragma once

#include "BasicTypes/Intrinsics.hpp"

/*
* *********************************
* Anatomy of a Handle in Apparition
* *********************************
* 
* -- Vulkan, and therefore Apparition, has multi-device support. Each device houses all of its resources internally
* 
* -- Each resource must know of what device it belongs to. 
*   -- Some resources, like CommandPools and DescriptorPools, manage their own resources, and these resources require knowledge of
*      these structures as well.
* 
* -- The relationship between resource and the owner of the resource is uneven. For example, in a system that Apparition would likely 
*    support, it would be unlikely to have more than 2 devices at once. Because of this uneven relationship, a handle of a resource 
*    can be split to give multiple kinds of data
* 
*                      ******************************
*                      64-bit Handle Data Description
*                      ******************************
*     
*       |       1 byte       |     3 bytes     |          4 bytes          |
*      /+++++++++++++++++++++|++++++++++++++++++\                           \
*     |                      |                   |                           \
*     | device and pool data | handle generation | index into resource array  |
*/

// Bits per section of data within a handle
#define DEVICE_DATA_TOTAL_BITS      2ull
#define POOL_DATA_TOTAL_BITS        6ull
#define RESOURCE_GEN_TOTAL_BITS     24ull
#define RESOURCE_INDEX_TOTAL_BITS   32ull

// 2 bits of device index, which supports 4 total devices, due to 0 being invalid
#define DEVICE_INDEX_SHIFT          62ull
// 6 bits of pool index, which support 63 pools a single resource type could belong to, due to 0 being invalid
#define POOL_INDEX_SHIFT            56ull
// 24 bits of resource handle generation, which supports 16,777,215 generations of a resource, due to 0 being invalid
#define RESOURCE_GEN_SHIFT          32ull

#define POOL_INDEX_MASK             ((1ull << POOL_DATA_TOTAL_BITS) - 1ull)
#define RESOURCE_GEN_MASK           ((1ull << RESOURCE_GEN_TOTAL_BITS) - 1ull)
// 32 bits of resource handles, which supprots 4,294,967,295, due to 0 being invalid
#define RESOURCE_INDEX_MASK         ((1ull << RESOURCE_GEN_SHIFT) - 1ull)

template <typename Handle>
inline u32 GetDeviceIndexFromHandle(Handle handle)
{
	Assert(handle.handle != AptnInvalidHandle);
	const u64 handleData = handle.handle;
	return (handleData >> DEVICE_INDEX_SHIFT);
}

template <typename Handle>
inline u32 GetResourcePoolIndexFromHandle(Handle handle)
{
	Assert(handle.handle != AptnInvalidHandle);
	const u64 poolIndexDataShifted = (handle.handle >> POOL_INDEX_SHIFT);
	return (poolIndexDataShifted & POOL_INDEX_MASK);
}

template <typename Handle>
inline u32 GetHandleIndex(Handle handle)
{
	Assert(handle.handle != AptnInvalidHandle);
	return (handle.handle & RESOURCE_INDEX_MASK);
}

#define _REGISTER_HANDLE_TYPE_DEVICE(HandleType)								\
inline DeviceInternal& GetDeviceInternal(Aptn##HandleType handleType)			\
{																				\
	Assert(apparition.deviceManager);											\
	DeviceManager& deviceManager = *apparition.deviceManager;					\
	const u32 deviceIndex = GetDeviceIndexFromHandle(handleType);				\
	return deviceManager.DeviceInternalFrom(deviceIndex);						\
}

#define _REGISTER_HANDLE_TYPE_GET_INTERNALS(HandleType, InternalName)															\
inline HandleType##Internal& Get##HandleType##InternalFromIndex(DeviceInternal& deviceInternal, u32 handleIndex)				\
{																																\
	return deviceInternal.InternalName[handleIndex - 1];																		\
}																																\
inline const HandleType##Internal& Get##HandleType##InternalFromIndex(const DeviceInternal& deviceInternal, u32 handleIndex)	\
{																																\
	return deviceInternal.InternalName[handleIndex - 1];																		\
}																																\
																																\
inline HandleType##Internal& Get##HandleType##Internal(Aptn##HandleType handle)													\
{																																\
	Assert(apparition.deviceManager);																							\
	DeviceManager& deviceManager = *apparition.deviceManager;																	\
	const u32 deviceIndex = GetDeviceIndexFromHandle(handle);																	\
	DeviceInternal& deviceInternal = deviceManager.DeviceInternalFrom(deviceIndex);												\
	return Get##HandleType##InternalFromIndex(deviceInternal, GetHandleIndex(handle));											\
}

#define REGISTER_HANDLE_TYPE(HandleType, InternalName)				\
	_REGISTER_HANDLE_TYPE_DEVICE(HandleType)						\
	_REGISTER_HANDLE_TYPE_GET_INTERNALS(HandleType, InternalName)
