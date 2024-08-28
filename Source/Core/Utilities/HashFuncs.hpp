// Copyright 2020, Nathan Blane

#pragma once

#include <type_traits>

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/ThirdParty/xxhash.h"
#include "Utilities/ThirdParty/farmhash.h"
#include "Utilities/ThirdParty/city.h"

template <typename T>
forceinline void HashCombine(u32& hash, const T& obj)
{
	u32 objHash = GetHash(obj);
	HashCombine(hash, objHash);
}

forceinline void HashCombine(u32& hash, u32 val)
{
	hash ^= val + 0x9e3779b9 + (hash << 6) + (hash >> 2);
}

//////////////////////////////////////////////////////////////////////////
// FNV-1a hash functions
//////////////////////////////////////////////////////////////////////////

constexpr u32 fnv32(const void* hashData, u32 dataSize)
{
	u8* byteData = (u8 *)hashData;

	constexpr u32 offsetBasis = 0x811c9dc5;
	constexpr u32 hashPrime = 0x1000193;

	u32 hash = offsetBasis;

	for (u32 i = 0; i < dataSize; ++i)
	{
		hash ^= *byteData++;
		hash *= hashPrime;
	}

	return hash;
}

constexpr u32 fnv32(const tchar* strData)
{
	constexpr u32 offsetBasis = 0x811c9dc5;
	constexpr u32 hashPrime = 0x1000193;

	u32 hash = offsetBasis;

	u32 i = 0;
	while (strData[i])
	{
		hash ^= strData[i++];
		hash *= hashPrime;
	}

	return hash;
}

constexpr u64 fnv64(const void* hashData, u64 dataSize)
{
	u8* byteData = (u8 *)hashData;

	constexpr u64 offsetBasis = 0xcbf29ce484222325;
	constexpr u64 hashPrime = 0x100000001b3;

	u64 hash = offsetBasis;

	for (u64 i = 0; i < dataSize; ++i)
	{
		hash ^= *byteData++;
		hash *= hashPrime;
	}

	return hash;
}

constexpr u64 fnv64(const tchar* strData)
{
	constexpr u64 offsetBasis = 0xcbf29ce484222325;
	constexpr u64 hashPrime = 0x100000001b3;

	u64 hash = offsetBasis;

	u64 i = 0;
	while (strData[i])
	{
		hash ^= strData[i++];
		hash *= static_cast<u64>(hashPrime);
	}

	return hash;
}

template <class T, typename = std::enable_if_t<!std::is_pointer_v<T>>>
constexpr u64 fnvHash(const T& objToHash)
{
	return fnv64(&objToHash, sizeof(T));
}

// TODO - Move this rotation to a place that can be reused. Might be useful to have...
// constexpr forceinline u32 xxhashRotl(u32 val, u32 bits)
// {
// 	return (val << bits) | (val >> (32 - bits));
// }
// 
// // TODO - Move this rotation to a place that can be reused. Might be useful to have...
// constexpr forceinline u64 xxhashRotl(u64 val, u64 bits)
// {
// 	return (val << bits) | (val >> (64 - bits));
// }
// 
// constexpr forceinline u32 xxhashRound32(u32 input, u32 accum)
// {
// 	u32 res = accum + input * XXHash32Prime2;
// 	res = xxhashRotl(res, 13);
// 	res *= XXHash32Prime1;
// 	return res;
// }
// 
// constexpr forceinline u64 xxhashRound64(u64 input, u64 accum)
// {
// 	u64 res = accum + input * XXHash64Prime2;
// 	res = xxhashRotl(res, 31);
// 	res *= XXHash64Prime1;
// 	return res;
// }
// 
// constexpr forceinline u64 xxhashCombineRound(u64 input, u64 accum)
// {
// 	u64 newVal = xxhashRound64(input, 0);
// 	u64 res = accum ^ newVal;
// 	res = res * XXHash64Prime1 + XXHash64Prime4;
// 	return res;
// }
// 
// u32 xxhash32(const void* buffer, u32 byteCount, u32 seed = 0);


//////////////////////////////////////////////////////////////////////////
// Farm64 hash function
//////////////////////////////////////////////////////////////////////////



