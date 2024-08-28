// Copyright 2020, Nathan Blane

#include "Plane.hpp"
#include "Matrix4.hpp"
#include "MatrixFunctions.hpp"

Plane::Plane()
	: normal(),
	d(0)
{
}

Plane::Plane(f32 a, f32 b, f32 c, f32 d_)
	: normal(a, b, c),
	d(d_)
{
}

Plane::Plane(Vector3 normal_, f32 d_)
	: normal(normal_),
	d(d_)
{
}

Plane::Plane(Vector3 p0, Vector3 p1, Vector3 p2)
	: normal(Vector3(p1 - p0).Cross(p2 - p0).GetNormalized()),
	d(normal.Dot(p0))
{
}

Plane& Plane::operator*=(const Matrix4& m)
{
	Matrix4 transAdjoint = Math::GetTransposeAdjointWithoutTranslation(m);
	f32 det = m.Determinant();
	// TODO - Have utility functions for transforming vec3s against mat4s
	normal = Vector3(Vector4(normal, 0) * transAdjoint);
	if (det < 0)
	{
		normal *= -1.f;
	}

	Vector3 newPoint = Vector3(Vector4(normal * d, 1) * m);
	d = newPoint.Dot(normal);

	return *this;
}


