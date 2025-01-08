
#include "ApparitionCore.h"

#include "Internal/DeviceManager.h"

namespace Apparition
{

void InitializeApparition(const InitializeParams& initParams)
{
	InitializeDeviceManager(initParams);
}

}
