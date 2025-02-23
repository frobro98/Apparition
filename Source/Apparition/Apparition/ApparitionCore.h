#pragma once

#include "BasicTypes/Delegate.h"
#include "Apparition/ApparitionAPI.hpp"
#include "Apparition/EnumDefinitions.h"

struct VkDebugUtilsMessengerCallbackDataEXT;

// Using similar version definition as Vulkan
#define APPARITION_MAKE_VERSION(major, minor, patch) \
	((((u32)(major)) << 22) | (((u32)(minor)) << 12) | ((u32)(patch)))

namespace Apparition
{
static constexpr u32 InvalidHandle = 0;

using AllocationDelegate = Delegate<void* (void* /* userData */, size_t /* allocSize */, size_t /* alignment */, AllocationScope /* scope */)>;
using ReallocationDelegate = Delegate<void* (void*, void*, size_t, size_t, AllocationScope)>;
using FreeDelegate = Delegate<void(void* /* userData */, void* /* memory */)>;
using AllocationNotificationDelegate = Delegate<void(void* /* userData */, size_t /* allocSize */, AllocationScope /* scope */)>;
using FreeNotificationDelegate = Delegate<void(void* /* userData */, size_t /* size */, AllocationScope /* scope */)>;

using DebugMessageTypeFlags = u32;
using ValidationDelegate = Delegate<bool(ValidationSeverity, DebugMessageTypeFlags /* messageTypeFlags*/, const VkDebugUtilsMessengerCallbackDataEXT* /* callbackData */, void* /* userData */)>;

struct AllocationCallbacks
{
	// Memory management optional functions
	AllocationDelegate allocFunctionCallback;
	ReallocationDelegate reallocFunctionCallback;
	FreeDelegate freeFunctionCallback;
	AllocationNotificationDelegate allocNotificationCallback;
	FreeNotificationDelegate freeNotificationCallback;
	void* userData;
};

// TODO - Handle validation layers
struct InitializeParams
{
	const tchar* applicationName = nullptr;
	const tchar* engineName = nullptr;
	const u32 applicationVersion = 0;
	const u32 engineVersion = 0;
	const u32 vulkanAPIVersion = 0;
};

APPARITION_API void SetAllocationCallbacks(const AllocationCallbacks& memoryCallbacks);
APPARITION_API void SetErrorLogCallback(ValidationDelegate&& validationDelegate, void* userData);

APPARITION_API void InitializeApparition(const InitializeParams& initParams);
}
