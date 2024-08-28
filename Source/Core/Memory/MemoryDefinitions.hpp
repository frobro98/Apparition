// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/MemoryUtilities.hpp"

constexpr u32 AllocationDefaultAlignment = 16;
constexpr u32 BitsForDefaultAlignment = 4;

constexpr size_t PageAllocationSize = KilobytesAsBytes(64);
constexpr size_t OSAllocationAlignment = KilobytesAsBytes(64);

