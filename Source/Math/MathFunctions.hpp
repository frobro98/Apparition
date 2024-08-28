// Copyright 2020, Nathan Blane

#pragma once

#include <cmath>

#include "CoreFlags.hpp"
#include "Math/MathConstants.hpp"
#include "Math/BitManipulation.hpp"
#include "Math/MathAPI.hpp"

namespace Math
{
template<typename T>
constexpr forceinline T Max(T lhs, T rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template<typename T>
constexpr forceinline T Min(T lhs, T rhs)
{
	return lhs < rhs ? lhs : rhs;
}

template<typename T>
constexpr forceinline T Clamp(T val, T minVal, T maxVal)
{
	return Min(Max(val, minVal), maxVal);
}

// Saturate
constexpr forceinline f32 Saturate(f32 val)
{
	return Clamp(val, 0.f, 1.f);
}

constexpr forceinline f64 Saturate(f64 val)
{
	return Clamp(val, 0., 1.);
}

// Select
constexpr forceinline i32 Select(i32 iFalse, i32 iTrue, bool expr)
{
	return expr ? iTrue : iFalse;
}

constexpr forceinline i64 Select(i64 iFalse, i64 iTrue, bool expr)
{
	return expr ? iTrue : iFalse;
}

constexpr forceinline u32 Select(u32 uFalse, u32 uTrue, bool expr)
{
	return expr ? uTrue : uFalse;
}

constexpr forceinline u64 Select(u64 fFalse, u64 fTrue, bool expr)
{
	return expr ? fTrue : fFalse;
}

constexpr forceinline f32 Select(f32 fFalse, f32 fTrue, bool expr)
{
	return expr ? fTrue : fFalse;
}

constexpr forceinline f64 Select(f64 fFalse, f64 fTrue, bool expr)
{
	return expr ? fTrue : fFalse;
}

constexpr forceinline bool IsEqual(const f32 a, const f32 b, const f32 epsilon = InternalTolerence)
{
	f32 tmp = a - b;
	return ((tmp >= -epsilon) && (tmp <= epsilon));
}
	
constexpr forceinline bool IsNotEqual(const f32 a, const f32 b, const f32 epsilon = InternalTolerence)
{
	return !IsEqual(a, b, epsilon);
}
	
constexpr forceinline bool IsOne(const f32 a, const f32 epsilon = InternalTolerence)
{
	f32 tmp = a - 1.0f;
	return ((tmp >= -epsilon) && (tmp <= epsilon));
}
	
constexpr forceinline bool IsNonZero(const f32 a, const f32 epsilon = InternalTolerence)
{
	return ((a < -epsilon) || (a > epsilon));
}
	
constexpr forceinline bool IsZero(const f32 a, const f32 epsilon = InternalTolerence)
{
	return ((a >= -epsilon) && (a <= epsilon));
}

constexpr forceinline f32 RadiansToDegrees(f32 rad)
{
	return rad * InvPiOver180;
}

constexpr forceinline f64 RadiansToDegrees(f64 rad)
{
	return rad * InvPiOver180_dbl;
}

constexpr forceinline f32 DegreesToRadians(f32 deg)
{
	return deg * PiOver180;
}

constexpr forceinline f64 DegreesToRadians(f64 deg)
{
	return deg * PiOver180_dbl;
}

// Absolute Value
forceinline i32 Abs(i32 value)
{
	return Max(-value, value);
}

forceinline i64 Abs(i64 value)
{
	return Max(-value, value);
}

// TODO - Make Abs constexpr
forceinline f32 Abs(f32 value)
{
	constexpr u32 floatMask = 0x7fffffff;
	return AsFloat32(AsUint32(value) & floatMask);
}

forceinline f64 Abs(f64 value)
{
	constexpr u64 doubleMask = 0x7fffffffffffffff;
	return AsFloat64(AsUint64(value) & doubleMask);
}

forceinline f32 Floor(f32 val)
{
	return floorf(val);
}

forceinline f64 Floor(f64 val)
{
	return floor(val);
}

forceinline f32 Ceil(f32 val)
{
	return ceilf(val);
}

forceinline f64 Ceil(f64 val)
{
	return ceil(val);
}

forceinline f32 Round(f32 val)
{
	return roundf(val);
}

forceinline f64 Round(f64 val)
{
	return round(val);
}

forceinline f32 Truncate(f32 val)
{
	return truncf(val);
}

forceinline f64 Truncate(f64 val)
{
	return trunc(val);
}

forceinline f32 Frac(f32 val)
{
	return val - Floor(val);
}

forceinline f64 Frac(f64 val)
{
	return val - Floor(val);
}

forceinline f32 Modf(f32 val, f32& i)
{
	i = Truncate(val);
	return val - i;
}

forceinline f64 Modf(f64 val, f64& i)
{
	i = Truncate(val);
	return val - i;
}

MATH_API f32 Fmod(f32 x, f32 y);

template <typename LerpType>
forceinline LerpType Lerp(const LerpType& a, const LerpType& b, f32 t)
{
	return LerpType(a + t * (b - a));
}

template <typename StepType>
forceinline StepType Step(const StepType& a, const StepType& b)
{
	return Select(0.f, 1.f, a >= b);
}

template <typename StepType>
forceinline StepType SmoothStep(const StepType& a, const StepType& b, const StepType& t)
{
	StepType d = Saturate((t - a) / (b - a));
	return d * d * (3.f - (2.f * d));
}

forceinline f32 Sin(f32 radAngle)
{
	return sinf(radAngle);
}

forceinline f32 Asin(f32 radAngle)
{
	return asinf(radAngle);
}

forceinline f32 Cos(f32 radAngle)
{
	return cosf(radAngle);
}

forceinline f32 Acos(f32 radAngle)
{
	return acosf(radAngle);
}

forceinline f32 Tan(f32 radAngle)
{
	return tanf(radAngle);
}

forceinline f32 Atan(f32 radAngle)
{
	return atanf(radAngle);
}

forceinline f32 Atan2(f32 y, f32 x)
{
	return atan2f(y, x);
}

// Normalized sinc function
forceinline f32 Sinc(f32 x)
{
	f32 ans = 1;
	if (!IsZero(x))
	{
		ans = Sin(Math::Pi * x) / (Math::Pi * x);
	}
	return ans;
}
	
forceinline f32 Sqrt(f32 num)
{
	return sqrtf(num);
}

forceinline f64 Sqrt(f64 num)
{
	return sqrt(num);
}

forceinline f32 InvSqrt(f32 num)
{
	return 1.f / Sqrt(num);
}

forceinline f64 InvSqrt(f64 num)
{
	return 1.f / Sqrt(num);
}

forceinline f32 Pow(f32 num, f32 pow)
{
	return powf(num, pow);
}

forceinline f32 Log2(f32 x)
{
	return log2f(x);
}

forceinline f32 Log10(f32 x)
{
	return log10f(x);
}

forceinline f32 Ln(f32 x)
{
	return logf(x);
}

forceinline f32 LogBase(f32 base, f32 x)
{
	return logf(x) / logf(base);
}

forceinline f32 Exp(f32 x)
{
	return expf(x);
}

forceinline f32 Exp2(f32 x)
{
	return Exp(x * NatLogOf2);
}

forceinline f32 Exp10(f32 x)
{
	return Exp(x * NatLogOf10);
}

forceinline f64 Exp(f64 x)
{
	return exp(x);
}

forceinline f64 Exp2(f64 x)
{
	return Exp(x * NatLogOf2);
}

forceinline f64 Exp10(f64 x)
{
	return Exp(x * NatLogOf10);
}

}
