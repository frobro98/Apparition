// Copyright 2020, Nathan Blane

#pragma once

#include "CoreAPI.hpp"

namespace Memory
{
// Defaulted to 16 because of vector intrinsics
constexpr size_t DefaultAlignment = 16;

CORE_API void* Malloc(size_t size, size_t alignment = DefaultAlignment);

CORE_API void* Realloc(void* ptr, size_t size, size_t alignment = DefaultAlignment);

CORE_API void Free(void* p);
}
