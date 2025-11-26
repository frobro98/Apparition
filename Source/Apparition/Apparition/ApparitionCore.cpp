
#include "ApparitionCore.h"

#include "Internal/ApparitionInternals.h"
#include "Internal/DeviceManager.h"

WALL_WRN_PUSH
#define VMA_VULKAN_VERSION 1003000
#define VMA_IMPLEMENTATION
#include "vma/vk_mem_alloc.h"
WALL_WRN_POP

namespace Apparition
{

void SetAllocationCallbacks(const AllocationCallbacks& memoryCallbacks)
{
	apparition.allocCallbacks = memoryCallbacks;
}

void SetErrorLogCallback(ValidationDelegate&& validationDelegate, void* userData)
{
	apparition.userValidationDelegate = MOVE(validationDelegate);
	apparition.validationUserData = userData;
}

void InitializeApparition(const InitializeParams& initParams)
{
	DeviceManager* deviceManager = new DeviceManager;
	deviceManager->SetDebugCallback(MOVE(apparition.userValidationDelegate), apparition.validationUserData);
	deviceManager->Initialize(initParams);

	apparition.deviceManager = deviceManager;
}

}
