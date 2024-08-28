// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Vector4.hpp"

struct BoxBounds
{
	Vector4 position;
	Vector4 dimensions;
};

struct SphereBounds
{
	Vector4 position;
	float radius = 0.f;
};

// Combines both bounded box and sphere together
struct BoxSphereBounds
{
	Vector4 position;
	Vector4 dimensions;
	float radius = 0.f;
};
