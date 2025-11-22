// Copyright 2022, Nathan Blane

#include "Platform.hpp"
#include "PlatformDefinitions.h"

WALL_WRN_PUSH
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
WALL_WRN_POP

namespace Platform
{
void DebugBreak()
{
	::DebugBreak();
}
}
