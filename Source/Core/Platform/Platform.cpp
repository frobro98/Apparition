// Copyright 2022, Nathan Blane

#include "Platform.hpp"
#include "PlatformDefinitions.h"

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Platform
{
void DebugBreak()
{
	::DebugBreak();
}
}
