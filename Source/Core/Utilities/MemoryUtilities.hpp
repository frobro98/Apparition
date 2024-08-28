// Copyright 2020, Nathan Blane

#pragma once

#include "CoreFlags.hpp"
#include "Debugging/Assertion.hpp"

forceinline constexpr size_t KilobytesAsBytes(size_t kilobytes)
{
	return kilobytes * 1024;
}

forceinline constexpr size_t MegabytesAsBytes(size_t megabytes)
{
	return KilobytesAsBytes(megabytes) * 1024;
}

forceinline constexpr size_t GigabytesAsBytes(size_t gigabytes)
{
	return MegabytesAsBytes(gigabytes) * 1024;
}

forceinline constexpr size_t Kilobytes(size_t bytes)
{
	return bytes / 1024;
}

forceinline constexpr size_t Megabytes(size_t bytes)
{
	return bytes / Kilobytes(bytes);
}

forceinline constexpr size_t Gigabytes(size_t bytes)
{
	return bytes / Megabytes(bytes);
}

forceinline void Memcpy(void* dst, const void* src, size_t size)
{
	memcpy(dst, src, size);
}

forceinline void Memcpy(void* dest, size_t destSize,  const void* src, size_t srcSize)
{
	memcpy_s(dest, destSize, src, srcSize);
}

forceinline void Memmove(void* dest, size_t destSize, const void* src, size_t srcSize)
{
	memmove_s(dest, destSize, src, srcSize);
}

forceinline void Memset(void* mem, i8 value, size_t memSize)
{
	memset(mem, value, memSize);
}

forceinline void ZeroMem(void* mem, size_t memSize)
{
	memset(mem, 0, memSize);
}

forceinline i32 Memcmp(const void* src1, const void* src2, size_t count)
{
	return memcmp(src1, src2, count);
}

forceinline const void* Memchr(const void* ptr, tchar ch, size_t count)
{
	return memchr(ptr, ch, count);
}

