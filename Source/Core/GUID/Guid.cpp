// Copyright 2020, Nathan Blane

#include <objbase.h>

#include "Guid.hpp"
#include "Debugging/Assertion.hpp"
#include "Utilities/HashFuncs.hpp"

Guid::Guid()
{
	GUID guid;
	::CoCreateGuid(&guid);
	Memcpy(bytes, GuidSize, &guid, GuidSize);
	ComputeHash();
}

Guid::Guid(const u8* data)
{
	Assert(data != nullptr);
	Memcpy(bytes, GuidSize, data, GuidSize);
	ComputeHash();
}

Guid::Guid(const Guid& other)
	: hash(other.hash)
{
	Memcpy(bytes, GuidSize, other.bytes, GuidSize);
}

Guid& Guid::operator=(const Guid& other)
{
	if (this != &other)
	{
		Memcpy(bytes, GuidSize, other.bytes, GuidSize);
		hash = other.hash;
	}
	return *this;
}

String Guid::ToString()
{
	return String();
}

void Guid::ComputeHash()
{
	hash = fnv32(bytes, GuidSize);
}
