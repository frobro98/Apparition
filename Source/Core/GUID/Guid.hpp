// Copyright 2020, Nathan Blane

#pragma once

#include <type_traits>

#include "BasicTypes/Intrinsics.hpp"
#include "Utilities/MemoryUtilities.hpp"
#include "String/String.h"
#include "CoreAPI.hpp"

constexpr u32 GuidSize = 16;

class CORE_API Guid
{
public:
	Guid();
	Guid(const u8* data);
	Guid(const Guid& other);
	Guid& operator=(const Guid&);

	String ToString();
	inline u32 GetHash() const
	{
		return hash;
	}

	friend bool operator==(const Guid& lhs, const Guid& rhs)
	{
		return Memcmp(lhs.bytes, rhs.bytes, GuidSize) == 0;
	}

	friend bool operator!=(const Guid& lhs, const Guid& rhs)
	{
		return Memcmp(lhs.bytes, rhs.bytes, GuidSize) != 0;
	}

	friend bool operator<(const Guid& lhs, const Guid& rhs)
	{
		return Memcmp(lhs.bytes, rhs.bytes, GuidSize) < 0;
	}

	friend bool operator<=(const Guid& lhs, const Guid& rhs)
	{
		return Memcmp(lhs.bytes, rhs.bytes, GuidSize) <= 0;
	}

	friend bool operator>(const Guid& lhs, const Guid& rhs)
	{
		return Memcmp(lhs.bytes, rhs.bytes, GuidSize) > 0;
	}

	friend bool operator>=(const Guid& lhs, const Guid& rhs)
	{
		return Memcmp(lhs.bytes, rhs.bytes, GuidSize) >= 0;
	}

private:
	void ComputeHash();

private:
	u8 bytes[GuidSize];
	u32 hash = 0;
};
