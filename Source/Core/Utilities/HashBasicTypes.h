// Copyright 2020, Nathan Blane

#pragma once

#include "Utilities/HashFuncs.hpp"
#include "String/CStringUtilities.hpp"
#include "GUID/Guid.hpp"

enum class KeyInput;

constexpr forceinline u32 GetHash(u8 b)
{
	return static_cast<u32>(b);
}

constexpr forceinline u32 GetHash(i8 b)
{
	return static_cast<u32>(b);
}

constexpr forceinline u32 GetHash(u16 s)
{
	return static_cast<u32>(s);
}

constexpr forceinline u32 GetHash(i16 s)
{
	return static_cast<u32>(s);
}

constexpr forceinline u32 GetHash(u32 i)
{
	return i;
}

constexpr forceinline u32 GetHash(i32 i)
{
	return static_cast<u32>(i);
}

constexpr forceinline u32 GetHash(u64 i)
{
	return fnv32(&i, sizeof(u64));
}

constexpr forceinline u32 GetHash(i64 i)
{
	return fnv32(&i, sizeof(i64));
}

forceinline u32 GetHash(float f)
{
	return *reinterpret_cast<u32*>(&f);
}

inline u32 GetHash(double d)
{
	return fnv32(&d, sizeof(double));
}

constexpr forceinline u32 GetHash(const void* p)
{
	return fnv32(p, sizeof(p));
}

constexpr forceinline u32 GetHash(void* p)
{
	return fnv32(p, sizeof(p));
}

constexpr forceinline u32 GetHash(const tchar* cStr)
{
	return fnv32(cStr);
}

constexpr forceinline u32 GetHash(KeyInput key)
{
	return static_cast<u32>(key);
}

forceinline u32 GetHash(const Guid& guid)
{
	return guid.GetHash();
}