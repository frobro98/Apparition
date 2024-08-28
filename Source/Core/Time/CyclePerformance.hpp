// Copyright 2020, Nathan Blane

#pragma once

// TODO - These functions will be different per platform, so it makes sense to have some sort of distiguishing
// location that designates them as platform functionality as well as containing multiple platform implementations

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

using Cycles = u64;

CORE_API Cycles GetCycleCount();
CORE_API f64 GetMicrosecondsFrom(Cycles cycles);
CORE_API f64 GetMillisecondsFrom(Cycles cycles);
CORE_API f64 GetSecondsFrom(Cycles cycles);

CORE_API f64 GetMicroseconds();
CORE_API f64 GetMilliseconds();
CORE_API f64 GetSeconds();
