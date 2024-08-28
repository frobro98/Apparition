#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "CoreAPI.hpp"

namespace PlatformMemory
{
enum class PlatformProtectionKind
{
	NoAccess,
	Read,
	Write,
	ReadWrite
};

struct PlatformMemoryInfo
{
	u64 totalPhys;
	u64 totalVirtual;
	u32 pageSize; // (4K)
	u32 allocationGranularity; // (64K) VirtualAlloc rounds up to this, which essentially means that addresses are essentially aligned
};

// Allocate Memory
NODISCARD CORE_API void* PlatformAlloc(size_t size);
// Free Memory
CORE_API void PlatformFree(void* p);
// Protect Page Memory
NODISCARD CORE_API bool PlatformProtect(void* p, size_t size, PlatformProtectionKind procKind);
// Get Platform Memory Constants
NODISCARD CORE_API PlatformMemoryInfo GetPlatformMemoryInfo();
}
