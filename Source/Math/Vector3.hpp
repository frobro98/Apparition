// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Math/MathConstants.hpp"
#include "Math/MathAPI.hpp"

struct Vector2;
struct Vector4;

struct MATH_API Vector3
{
	static const Vector3 RightAxis;
	static const Vector3 UpAxis;
	static const Vector3 ForwardAxis;
	static const Vector3 Zero;
	static const Vector3 One;

public:
	f32 x = 0.f;
	f32 y = 0.f;
	f32 z = 0.f;

	Vector3() = default;
	explicit Vector3(f32 x, f32 y, f32 z);
	explicit Vector3(const Vector4& v4);
	explicit Vector3(const Vector2& v2, f32 inZ);

	float Dot(const Vector3& other) const;
	Vector3 Cross(const Vector3& other) const;
	void Normalize();
	Vector3 GetNormalized() const;

	float Magnitude() const;
	float MagnitudeSqr() const;
	float InverseMagnitude() const;

	bool IsEqual(const Vector3& other, float epsilon = Math::InternalTolerence) const;
	bool IsZero(float epsilon = Math::InternalTolerence) const;


	// Unary operators
	Vector3 operator+() const;
	Vector3 operator-() const;

	// Arithmetic operators
	Vector3& operator+=(const Vector3& other);
	Vector3& operator-=(const Vector3& other);

	Vector3 operator+(const Vector3& other) const;
	Vector3 operator-(const Vector3& other) const;

	Vector3& operator*=(f32 s);
	Vector3 operator*(f32 s) const;
	friend MATH_API Vector3 operator*(f32 s, const Vector3 & vec);

	friend MATH_API bool operator==(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.IsEqual(rhs);
	}

	friend MATH_API bool operator!=(const Vector3& lhs, const Vector3& rhs)
	{
		return !lhs.IsEqual(rhs);
	}

	friend MATH_API bool operator>(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x > rhs.x
			&& lhs.y > rhs.y
			&& lhs.z > rhs.z;
	}

	friend MATH_API bool operator>=(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x >= rhs.x
			&& lhs.y >= rhs.y
			&& lhs.z >= rhs.z;
	}

	friend MATH_API bool operator<(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x < rhs.x
			&& lhs.y < rhs.y
			&& lhs.z < rhs.z;
	}

	friend MATH_API bool operator<=(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x <= rhs.x
			&& lhs.y <= rhs.y
			&& lhs.z <= rhs.z;
	}
};
