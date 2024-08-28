// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

struct ShaderID
{
	u32 shaderNameHash;
	u32 bytecodeHash;
};

forceinline bool operator==(const ShaderID& lhs, const ShaderID& rhs)
{
	return lhs.bytecodeHash == rhs.bytecodeHash &&
		lhs.shaderNameHash == rhs.shaderNameHash;
}

forceinline bool operator!=(const ShaderID& lhs, const ShaderID& rhs)
{
	return lhs.bytecodeHash != rhs.bytecodeHash &&
		lhs.shaderNameHash != rhs.shaderNameHash;
}

forceinline u32 GetHash(const ShaderID& id)
{
	u32 hash = id.bytecodeHash;
	HashCombine(hash, id.shaderNameHash);
	return hash;
}
