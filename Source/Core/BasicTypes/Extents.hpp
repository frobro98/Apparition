// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

struct Extents2D
{
	f32 width;
	f32 height;

	friend bool operator==(const Extents2D& lhs, const Extents2D& rhs)
	{
		return lhs.width == rhs.width && lhs.height == rhs.height;
	}

	friend bool operator!=(const Extents2D& lhs, const Extents2D& rhs)
	{
		return lhs.width != rhs.width && lhs.height != rhs.height;
	}
};

struct Extents3D
{
	f32 width;
	f32 height;
	f32 depth;
};

