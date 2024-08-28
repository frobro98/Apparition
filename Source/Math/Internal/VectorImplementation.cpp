// Copyright 2020, Nathan Blane

#include "VectorImplementation.h"
#include "Debugging/Assertion.hpp"
#include "Vector4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "MathFunctions.hpp"

// Includes to handle SIMD register types
WALL_WRN_PUSH
#include <xmmintrin.h>
#include <smmintrin.h> 
WALL_WRN_POP

namespace Internal
{
// TODO - Probably should be in a different file but not now...
#ifdef VECTOR_ALGO_SSE
using VectorRegister = __m128;

#else

float VectorDotVector(const Vector2& v0, const Vector2& v1)
{
	return v0.x * v1.x + v0.y * v1.y;
}

void VectorNormalize(Vector2& v)
{
	Assert(!v.IsZero());
	float invMag = VectorInverseMagnitude(v);
	v.x *= invMag;
	v.y *= invMag;
}

float VectorMagnitudeSqr(const Vector2& v)
{
	return v.x * v.x + v.y * v.y;
}

float VectorMagnitude(const Vector2& v)
{
	return Math::Sqrt(VectorMagnitudeSqr(v));
}

float VectorInverseMagnitude(const Vector2& v)
{
	return 1.f / VectorMagnitude(v);
}

float VectorAngleBetweenVectors(const Vector2& v0, const Vector2& v1)
{
	Vector2 v0Norm = v0;
	Vector2 v1Norm = v1;
	VectorNormalize(v0Norm);
	VectorNormalize(v1Norm);
	const float cosAngle = VectorDotVector(v0Norm, v1Norm);
	return Math::Acos(cosAngle);
}

float VectorDotVector(const Vector3& v0, const Vector3& v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

void VectorCrossVector(const Vector3& v0, const Vector3& v1, Vector3& perpVector)
{
	perpVector = Vector3(
		v0.y * v1.z - v1.y * v0.z,
		v0.z * v1.x - v1.z * v0.x,
		v0.x * v1.y - v1.x * v0.y
	);
}

void VectorNormalize(Vector3& v)
{
	Assert(!v.IsZero());
	float invMag = VectorInverseMagnitude(v);
	v.x *= invMag;
	v.y *= invMag;
	v.z *= invMag;
}

float VectorMagnitudeSqr(const Vector3& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float VectorMagnitude(const Vector3& v)
{
	return Math::Sqrt(VectorMagnitudeSqr(v));
}

float VectorInverseMagnitude(const Vector3& v)
{
	return 1.f / VectorMagnitude(v);
}

float VectorAngleBetweenVectors(const Vector3& v0, const Vector3& v1)
{
	Vector3 v0Norm = v0;
	Vector3 v1Norm = v1;
	VectorNormalize(v0Norm);
	VectorNormalize(v1Norm);
	const float cosAngle = VectorDotVector(v0Norm, v1Norm);
	return Math::Acos(cosAngle);
}

float VectorDotVector(const Vector4& v0, const Vector4& v1)
{
	return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
}

void VectorCrossVector(const Vector4& v0, const Vector4& v1, Vector4& perpVector)
{
	perpVector = Vector4(
		v0.y * v1.z - v1.y * v0.z,
		v0.z * v1.x - v1.z * v0.x,
		v0.x * v1.y - v1.x * v0.y
	);
}

void VectorNormalize(Vector4& v)
{
	Assert(!v.IsZero());
	float invMag = VectorInverseMagnitude(v);
	v.x *= invMag;
	v.y *= invMag;
	v.z *= invMag;
}

float VectorMagnitudeSqr(const Vector4& v)
{
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

float VectorMagnitude(const Vector4& v)
{
	return Math::Sqrt(VectorMagnitudeSqr(v));
}

float VectorInverseMagnitude(const Vector4& v)
{
	return 1.f / VectorMagnitude(v);
}

float VectorAngleBetweenVectors(const Vector4& v0, const Vector4& v1)
{
	Vector4 v0Norm = v0;
	Vector4 v1Norm = v1;
	VectorNormalize(v0Norm);
	VectorNormalize(v1Norm);
	const float cosAngle = VectorDotVector(v0Norm, v1Norm);
	return Math::Acos(cosAngle);
}

void VectorAddTo(Vector2& v0, const Vector2& v1)
{
	v0.x += v1.x;
	v0.y += v1.y;
}

void VectorSubTo(Vector2& v0, const Vector2& v1)
{
	v0.x -= v1.x;
	v0.y -= v1.y;
}

void VectorMultiTo(Vector2& v0, const Vector2& v1)
{
	v0.x *= v1.x;
	v0.y *= v1.y;
}

void VectorMultiTo(Vector2& v0, float s)
{
	v0.x *= s;
	v0.y *= s;
}

void VectorDivTo(Vector2& v0, float s)
{
	const float invS = 1.f / s;
	v0.x *= invS;
	v0.y *= invS;
}

void VectorAddTo(Vector3& v0, const Vector3& v1)
{
	v0.x += v1.x;
	v0.y += v1.y;
	v0.z += v1.z;
}

void VectorSubTo(Vector3& v0, const Vector3& v1)
{
	v0.x -= v1.x;
	v0.y -= v1.y;
	v0.z -= v1.z;
}

void VectorMultiTo(Vector3& v0, float s)
{
	v0.x *= s;
	v0.y *= s;
	v0.z *= s;
}

void VectorDivTo(Vector3& v0, float s)
{
	const float invS = 1.f / s;
	v0.x *= invS;
	v0.y *= invS;
	v0.z *= invS;
}

// Vector Math
void VectorAddTo(Vector4& v0, const Vector4& v1)
{
	v0.x += v1.x;
	v0.y += v1.y;
	v0.z += v1.z;
	v0.w = 1.f;
}

void VectorSubTo(Vector4& v0, const Vector4& v1)
{
	v0.x -= v1.x;
	v0.y -= v1.y;
	v0.z -= v1.z;
	v0.w = 1.f;
}

void VectorMultiTo(Vector4& v0, float s)
{
	v0.x *= s;
	v0.y *= s;
	v0.z *= s;
	v0.w = 1.f;
}

void VectorDivTo(Vector4& v0, float s)
{
	const float invS = 1.f / s;
	v0.x *= invS;
	v0.y *= invS;
	v0.z *= invS;
	v0.w = 1.f;
}

}

#endif // VECTOR_ALGO_SSE
