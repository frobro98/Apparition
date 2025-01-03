#pragma once

#include "BasicTypes/Delegate.h"

enum VkSystemAllocationScope;
enum VkInternalAllocationType;

namespace Apparition
{
struct ApparitionInitializeParams
{
	// Memory management optional functions
	
	Delegate<void(void*, size_t, VkInternalAllocationType, VkSystemAllocationScope)> allocNotificationCallback;
	Delegate<void(void*, size_t, VkInternalAllocationType, VkSystemAllocationScope)> freeNotificationCallback;
	Delegate<void*(void*, size_t, size_t, VkSystemAllocationScope)> allocFunctionCallback;
	Delegate<void*(void*, void*, size_t, size_t, VkSystemAllocationScope)> reallocFunctionCallback;
	Delegate<void(void*, void*)> freeFunctionCallback;
	void* userData;
};

void InitializeApparition();
//void SetErrorLogCallback();
}
