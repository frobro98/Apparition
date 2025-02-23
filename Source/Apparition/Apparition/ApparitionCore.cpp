
#include "ApparitionCore.h"

#include "Internal/ApparitionInternals.h"

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
