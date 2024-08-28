// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Memory/MemoryAllocation.hpp"
#include "Utilities/BitUtilities.hpp"
#include "Debugging/Assertion.hpp"

#define GLOBAL_OPERATOR_NEW_DELETE_OVERLOADS \
	NODISCARD void* operator new(size_t count) { return Memory::Malloc(count); } \
	NODISCARD void* operator new(size_t count, const std::nothrow_t&) noexcept { return Memory::Malloc(count); } \
	NODISCARD void* operator new(size_t count, std::align_val_t al) { return Memory::Malloc(count, (size_t)al); } \
	NODISCARD void* operator new(size_t count, std::align_val_t al, const std::nothrow_t &) noexcept { return Memory::Malloc(count, (size_t)al); } \
	NODISCARD void* operator new[](size_t count) { return Memory::Malloc(count); } \
	NODISCARD void* operator new[](size_t count, const std::nothrow_t&) noexcept { return Memory::Malloc(count); } \
	NODISCARD void* operator new[](size_t count, std::align_val_t al) { return Memory::Malloc(count, (size_t)al); } \
	NODISCARD void* operator new[](size_t count, std::align_val_t al, const std::nothrow_t &) noexcept { return Memory::Malloc(count, (size_t)al); } \
	void operator delete(void* p) throw() { return Memory::Free(p); } \
	void operator delete(void* p, const std::nothrow_t&) noexcept { return Memory::Free(p); } \
	void operator delete(void* p, size_t /*size*/) throw() { return Memory::Free(p); } \
	void operator delete(void* p, size_t /*size*/, const std::nothrow_t&) noexcept { return Memory::Free(p); } \
	void operator delete[](void* p) throw() { return Memory::Free(p); } \
	void operator delete[](void* p, const std::nothrow_t&) noexcept { return Memory::Free(p); } \
	void operator delete[](void* p, size_t /*size*/) throw() { return Memory::Free(p); } \
	void operator delete[](void* p, size_t /*size*/, const std::nothrow_t&) noexcept { return Memory::Free(p); }

template <typename AlignType>
forceinline constexpr AlignType Align(const AlignType ptr, size_t alignment)
{
	Assert(IsPowerOf2(alignment));
	return (AlignType)(((u64)ptr + (alignment - 1)) & ~(alignment - 1));
}

forceinline constexpr bool IsAligned(size_t ptrAddr, size_t alignment)
{
	return !(ptrAddr % alignment);
}

forceinline bool IsAligned(const void* ptrAddr, uintptr_t alignment)
{
	uintptr_t ptr = reinterpret_cast<uintptr_t>(ptrAddr);
	return !(ptr % alignment);
}

