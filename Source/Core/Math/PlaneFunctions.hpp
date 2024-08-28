// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Vector3.hpp"
#include "Math/Plane.hpp"

namespace Math
{

// n . p = 0 needs to be plugged into d = ax + by + cz
// so we need to subtract d from each side to make ax + by + cz - d = 0.
forceinline f32 DistanceFromPointAndPlane(const Vector3& point, const Plane& plane)
{
	return point.x * plane.normal.x + point.y * plane.normal.y + point.z * plane.normal.z - plane.d;
}
}
