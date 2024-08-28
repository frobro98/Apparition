// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

struct InputContextID
{
	u32 hash;
	// TODO - Eventually will either have this hash represent the file contents or will add another hash to represent that
};

static InputContextID InvalidID = InputContextID{ 0x00000000 };

forceinline bool operator==(InputContextID lhs, InputContextID rhs)
{
	return lhs.hash == rhs.hash;
}
