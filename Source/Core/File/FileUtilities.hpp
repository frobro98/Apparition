// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/MemoryBuffer.hpp"
#include "CoreAPI.hpp"

CORE_API MemoryBuffer LoadFileToMemory(const tchar* filePath);
