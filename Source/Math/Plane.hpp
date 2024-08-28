// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Math/Vector3.hpp"
#include "Math/Matrix4.hpp"

// Plane as defined by the vector equation of a plane
//	ax + by + cz = d, where n = <a, b, c> and d = dot(n, p)
// This is different from the linear equation of a plane
//	ax + by + cz + d = 0
//
// The math used here was learned by looking online. An example page would be:
//   http://www.it.hiof.no/~borres/j3d/math/plan/p-plan.html
//
struct Plane
{
	Plane();
	Plane(f32 a, f32 b, f32 c, f32 d);
	Plane(Vector3 normal, f32 d);
	Plane(Vector3 p0, Vector3 p1, Vector3 p2);

	// Learn more from the "On the Transformation of Surface Normals" paper
	//	https://www.glassner.com/wp-content/uploads/2014/04/On-the-Transformation-of-Surface-Normals.pdf
	Plane& operator*=(const Matrix4& m);

	Vector3 normal;
	f32 d;
};

Plane operator*(const Plane& p, const Matrix4& m)
{
	Plane plane(p);
	plane *= m;
	return plane;
}
