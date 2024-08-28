#pragma once

#include "BasicTypes/Intrinsics.hpp"

// Floor of Log2. Not using a table implementation because I don't want to actually set that thing up

constexpr u32 CountBits32(u64 x)
{
	x -= (x >> 1) & 0x5555555555555555ull; //put count of each 2 bits into those 2 bits
	x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull); //put count of each 4 bits into those 4 bit
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full; //put count of each 8 bits into those 8 bits
	return (x * 0x0101010101010101) >> 56; //returns left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + etc.
}

constexpr u32 FloorLogTwo32(u32 x)
{
	u32 pos = 0;
	if (x >= 1ul << 16) { x >>= 16; pos += 16; }
	if (x >= 1ul << 8) { x >>= 8; pos += 8; }
	if (x >= 1ul << 4) { x >>= 4; pos += 4; }
	if (x >= 1ul << 2) { x >>= 2; pos += 2; }
	if (x >= 1ul << 1) { pos += 1; }
	return (x == 0) ? 0 : pos;
}

constexpr u64 FloorLogTwo64(u64 x)
{
	u64 pos = 0;
	if (x >= 1ull << 32) { x >>= 32; pos += 32; }
	if (x >= 1ull << 16) { x >>= 16; pos += 16; }
	if (x >= 1ull << 8) { x >>= 8; pos += 8; }
	if (x >= 1ull << 4) { x >>= 4; pos += 4; }
	if (x >= 1ull << 2) { x >>= 2; pos += 2; }
	if (x >= 1ull << 1) { pos += 1; }
	return (x == 0) ? 0 : pos;
}

forceinline constexpr u32 LeadingZeros32(u32 x)
{
	return x == 0 ? 32 : 31 - FloorLogTwo32(x);
}

forceinline constexpr u64 LeadingZeros64(u64 x)
{
	return x == 0 ? 64 : 63 - FloorLogTwo64(x);
}

constexpr u32 TrailingZeros32(u32 x)
{
	if (x != 0)
	{
		u32 n = 0;
		while ((x & 0x1) == 0)
		{
			x >>= 1;
			++n;
		}
		return n;
	}
	return 0;
}

constexpr u64 TrailingZeros64(u64 x)
{
	if (x != 0)
	{
		u64 n = 0;
		while ((x & 0x1) == 0)
		{
			x >>= 1;
			++n;
		}
		return n;
	}
	return 0;
}

forceinline constexpr u32 CeilLogTwo32(u32 x)
{
	i32 mask = ((i32)LeadingZeros32(x) << 26) >> 31;
	return (32 - LeadingZeros32(x - 1)) & (~mask);
}

forceinline constexpr u64 CeilLogTwo64(u64 x)
{
	i64 mask = ((i64)LeadingZeros64(x) << 57ull) >> 63ull;
	return (64ull - LeadingZeros64(x - 1)) & (~mask);
}

forceinline constexpr u32 FloorPowerOfTwo32(u32 x)
{
	return 1ul << FloorLogTwo32(x);
}

forceinline constexpr u64 FloorPowerOfTwo64(u64 x)
{
	return 1ull << FloorLogTwo64(x);
}

forceinline constexpr u32 CeilPowerOfTwo32(u32 x)
{
	return 1ul << CeilLogTwo32(x);
}

forceinline constexpr u64 CeilPowerOfTwo64(u64 x)
{
	return 1ull << CeilLogTwo64(x);
}

forceinline constexpr bool IsPowerOf2(u64 num)
{
	return (!(num == 0)) & ((num & (num - 1)) == 0);
}

