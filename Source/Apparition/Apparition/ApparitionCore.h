#pragma once

#include "BasicTypes/Delegate.h"
#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/EnumDefinitions.h"

struct VkDebugUtilsMessengerCallbackDataEXT;

// Using similar version definition as Vulkan
#define APPARITION_MAKE_VERSION(major, minor, patch) \
	((((u32)(major)) << 22) | (((u32)(minor)) << 12) | ((u32)(patch)))

static constexpr u32 AptnInvalidHandle = 0;


using AptnAllocationDelegate = Delegate<void* (void* /* userData */, size_t /* allocSize */, size_t /* alignment */, AllocationScope /* scope */)>;
using AptnReallocationDelegate = Delegate<void* (void*, void*, size_t, size_t, AllocationScope)>;
using AptnFreeDelegate = Delegate<void(void* /* userData */, void* /* memory */)>;
using AptnAllocationNotificationDelegate = Delegate<void(void* /* userData */, size_t /* allocSize */, AllocationScope /* scope */)>;
using AptnFreeNotificationDelegate = Delegate<void(void* /* userData */, size_t /* size */, AllocationScope /* scope */)>;

using AptnDebugMessageTypeFlags = u32;
using AptnValidationDelegate = Delegate<bool(ValidationSeverity, AptnDebugMessageTypeFlags /* messageTypeFlags*/, const VkDebugUtilsMessengerCallbackDataEXT* /* callbackData */, void* /* userData */)>;

struct AptnAllocationCallbacks
{
	// Memory management optional functions
	AptnAllocationDelegate allocFunctionCallback;
	AptnReallocationDelegate reallocFunctionCallback;
	AptnFreeDelegate freeFunctionCallback;
	AptnAllocationNotificationDelegate allocNotificationCallback;
	AptnFreeNotificationDelegate freeNotificationCallback;
	void* userData;
};

// TODO - Handle validation layers
struct AptnInitializeParams
{
	const tchar* applicationName = nullptr;
	const tchar* engineName = nullptr;
	const u32 applicationVersion = 0;
	const u32 engineVersion = 0;
	const u32 vulkanAPIVersion = 0;
};

namespace Apparition
{
APPARITION_API void SetAllocationCallbacks(const AptnAllocationCallbacks& memoryCallbacks);
APPARITION_API void SetErrorLogCallback(AptnValidationDelegate&& validationDelegate, void* userData);

APPARITION_API void InitializeApparition(const AptnInitializeParams& initParams);
}
// TODO - The below could be done with templates and concepts

// NOTE - This is partially correct, but it does not determine validity through the actual handle pools
#define HANDLE_TYPE_ISVALID(HandleType)				\
	constexpr bool IsValid(HandleType handle)		\
	{												\
		return handle.handle != AptnInvalidHandle;	\
	}

// Compare ops for InvalidHandle
#define HANDLE_TYPE_OPERATORS(HandleType)									\
	inline bool operator==(HandleType handle, u32 internalHandleValue)		\
	{																		\
		return handle.handle == internalHandleValue;						\
	}																		\
																			\
	inline bool operator!=(HandleType handle, u32 internalHandleValue)		\
	{																		\
		return handle.handle != internalHandleValue;						\
	}

// Defines a handle type
#define HANDLE_TYPE(HandleType)		\
struct HandleType					\
{									\
	u64 handle;						\
};									\
HANDLE_TYPE_OPERATORS(HandleType)	\
HANDLE_TYPE_ISVALID(HandleType)