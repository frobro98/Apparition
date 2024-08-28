// Copyright 2020, Nathan Blane

#include "Quat.hpp"
#include "Vector4.hpp"
#include "Matrix4.hpp"
#include "MathFunctions.hpp"
#include "QuaternionImplementation.hpp"

// Includes to handle SIMD register types
WALL_WRN_PUSH
#include <xmmintrin.h>
#include <smmintrin.h> 
WALL_WRN_POP

namespace Internal
{
#ifdef VECTOR_ALGO_SSE
using VectorRegister = __m128;

#else

// TODO - Determine if more set functions need to live here as well
void  QuaternionSetMatrix(Quat& q, const Matrix4& mat)
{
	float tr = mat[m0] + mat[m5] + mat[m10];
	float s;

	if (tr > 0.f)
	{
		s = 2 * Math::Sqrt(tr + 1.f);
		q.w = .25f * s;
		q.x = -(mat[m9] - mat[m6]) / s;
		q.y = -(mat[m2] - mat[m8]) / s;
		q.z = -(mat[m4] - mat[m1]) / s;
	}
	else if (!!((mat[m0] > mat[m5]) & (mat[m0] > mat[m10])))
	{
		s = 2.f * Math::Sqrt(1.f + mat[m0] - mat[m5] - mat[m10]);
		q.w = -(mat[m9] - mat[m6]) / s;
		q.x = .25f * s;
		q.y = (mat[m1] + mat[m4]) / s;
		q.z = (mat[m2] + mat[m8]) / s;
	}
	else if (mat[m5] > mat[m10])
	{
		s = 2.f * Math::Sqrt(1.f + mat[m5] - mat[m0] - mat[m10]);
		q.w = -(mat[m2] - mat[m8]) / s;
		q.x = (mat[m1] + mat[m4]) / s;
		q.y = .25f * s;
		q.z = (mat[m6] + mat[m9]) / s;
	}
	else
	{
		s = 2.f * Math::Sqrt(1.f + mat[m10] - mat[m0] - mat[m5]);
		q.w = -(mat[m4] - mat[m1]) / s;
		q.x = (mat[m2] + mat[m8]) / s;
		q.y = (mat[m6] + mat[m9]) / s;
		q.z = .25f * s;
	}
}


float QuaternionDotQuaternion(const Quat& q0, const Quat& q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

void  QuaternionNormalize(Quat& q)
{
	float inverseMag = QuaternionMagnitudeInverse(q);
	q.x *= inverseMag;
	q.y *= inverseMag;
	q.z *= inverseMag;
	q.w *= inverseMag;
}

void  QuaternionInverse(Quat& q)
{
	float inverseMag = QuaternionMagnitudeInverse(q);
	q.Conjugate();
	QuaternionMultiTo(q, inverseMag*inverseMag);
}

float QuaternionMagnitude(const Quat& q)
{
	return Math::Sqrt(QuaternionMagnitudeSqr(q));
}

float QuaternionMagnitudeSqr(const Quat& q)
{
	return QuaternionDotQuaternion(q, q);
}

float QuaternionMagnitudeInverse(const Quat& q)
{
	return 1.f / QuaternionMagnitude(q);
}

void QuaternionLqcvq(const Quat& q, const Vector4& v, Vector4& out)
{
	Vector4 quatV(q.x, q.y, q.z);

	float qdq = quatV.Dot(quatV);
	float qdv = quatV.Dot(v);
	out = 2.f*q.w*(quatV.Cross(v)) + (q.w*q.w - qdq) * v + 2.f * (qdv)* quatV;
}

void QuaternionLqvqc(const Quat& q, const Vector4& v, Vector4& out)
{
	Vector4 quatV(q.x, q.y, q.z);
	out = 2.f*q.w*(v.Cross(quatV)) + (q.w*q.w - quatV.Dot(quatV)) * v + 2.f * (quatV.Dot(v)) * quatV;
}
void QuaternionMultiByElement(Quat& q0, const Quat& q1)
{
	q0 = Quat(q0.x*q1.x, q0.y*q1.y, q0.z*q1.z, q0.w*q1.w);
}


void QuaternionAddTo(Quat& q0, const Quat& q1)
{
	q0.x += q1.x;
	q0.y += q1.y;
	q0.z += q1.z;
	q0.w += q1.w;
}

void QuaternionSubTo(Quat& q0, const Quat& q1)
{
	q0.x -= q1.x;
	q0.y -= q1.y;
	q0.z -= q1.z;
	q0.w -= q1.w;
}
void QuaternionMultiTo(Quat& q0, const Quat& q1)
{
	Vector4 v0(q0.x, q0.y, q0.z);
	Vector4 v1(q1.x, q1.y, q1.z);
	q0.Set(v1.Cross(v0) + q1.w*v0 + q0.w * v1, q0.w*q1.w - v0.Dot(v1));
}
void QuaternionDivTo(Quat& q0, const Quat& q1)
{
	q0.x /= q1.x;
	q0.y /= q1.y;
	q0.z /= q1.z;
	q0.w /= q1.w;
}


void QuaternionAddTo(Quat& q, float val)
{
	q.x += val;
	q.y += val;
	q.z += val;
	q.w += val;
}
void QuaternionSubTo(Quat& q, float val)
{
	q.x -= val;
	q.y -= val;
	q.z -= val;
	q.w -= val;
}
void QuaternionSubTo(float val, Quat& q)
{
	q.x = val - q.x;
	q.y = val - q.y;
	q.z = val - q.z;
	q.w = val - q.w;
}
void QuaternionMultiTo(Quat& q, float val)
{
	q.x *= val;
	q.y *= val;
	q.z *= val;
	q.w *= val;
}
void QuaternionDivTo(float val, Quat& q)
{
	q.x = val / q.x;
	q.y = val / q.y;
	q.z = val / q.z;
	q.w = val / q.w;
}
void QuaternionDivTo(Quat& q, float val)
{
	q.x /= val;
	q.y /= val;
	q.z /= val;
	q.w /= val;
}

#endif

}