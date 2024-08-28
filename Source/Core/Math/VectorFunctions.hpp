// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Vector4.hpp"
#include "Math/MathFunctions.hpp"
#include "CoreFlags.hpp"

namespace Math
{
forceinline float GetRadiansBetween(const Vector4 vec0, const Vector4& vec1)
{
	Vector4 v0 = vec0.GetNormalized();
	Vector4 v1 = vec1.GetNormalized();
	float cosAngle = v0.Dot(v1);
	return Math::Acos(cosAngle);
}
// Gets angle between 2 vectors
forceinline float GetDegreesBetween(const Vector4 vec0, const Vector4& vec1)
{
	return Math::RadiansToDegrees(GetRadiansBetween(vec0, vec1));
}

forceinline Vector4 Reflect(const Vector4& v, const Vector4& n)
{
	return v - 2 * v.Dot(n) * n;
}

// 	forceinline Vector4 Refract(const Vector4& incident, const Vector4& normal, float32 indexEta)
// 	{
// 		float32 ni = normal.Dot(incident);
// 		float32 k = 1.f - indexEta * indexEta * (1.f - ni * ni);
// 		return Select(Vector4::Zero, indexEta * incident - (indexEta * ni + Sqrt(k)) * normal, k >= 0);
// 	}

// 	inline void Lerp(Vector4 &out, const Vector4& a, const Vector4& b, const float t)
// 	{
// 		// out = a + t * (b - a);
// 		out = a + t * (b - a);
// 	}

inline void LerpArray(Vector4 *out, const Vector4* a, const Vector4* b, const float t, const int numVects)
{
	for (int i = 0; i < numVects; ++i)
	{
		out[i] = a[i] + t * (b[i] - a[i]);
	}
}

}
