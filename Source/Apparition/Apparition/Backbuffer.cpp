
#include "Backbuffer.h"

#include "Internal/ApparitionInternals.h"

namespace Apparition
{
void SetupBackbuffer(DeviceHandle device, const BackbufferSetupParams& params)
{
    Assert(apparition.deviceManager);

    return apparition.deviceManager->SetupBackbuffer(device, params);
}

void TeardownBackbuffer(DeviceHandle device)
{
    Assert(apparition.deviceManager);
    NOT_USED DeviceManager& deviceManager = *apparition.deviceManager;
    deviceManager.TeardownBackbuffer(device);
}
}