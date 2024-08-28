// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Math/MathConstants.hpp"
#include "Math/MathAPI.hpp"

struct Vector3;
struct Vector4;

struct MATH_API Vector2
{
	static const Vector2 RightAxis;
	static const Vector2 UpAxis;
	static const Vector2 Zero;
	static const Vector2 One;

public:
	f32 x = 0.f;
	f32 y = 0.f;

	Vector2() = default;
	explicit Vector2(f32 x, f32 y);
	explicit Vector2(const Vector4& v4);
	explicit Vector2(const Vector3& v3);

	float Dot(const Vector2& other) const;
	Vector2 Cross(const Vector2& other) const;
	void Normalize();
	Vector2 GetNormalized() const;

	float Magnitude() const;
	float MagnitudeSqr() const;
	float InverseMagnitude() const;

	bool IsEqual(const Vector2& other, float epsilon = Math::InternalTolerence) const;
	bool IsZero(float epsilon = Math::InternalTolerence) const;


	// Unary operators
	Vector2 operator+() const;
	Vector2 operator-() const;

	// Arithmetic operators
	Vector2& operator+=(const Vector2& other);
	Vector2& operator-=(const Vector2& other);
	Vector2& operator*=(const Vector2& other);

	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator*(const Vector2& other) const;

	Vector2& operator*=(f32 s);
	Vector2 operator*(f32 s) const;
	friend MATH_API Vector2 operator*(f32 s, const Vector2 & vec);

	friend MATH_API bool operator==(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.IsEqual(rhs);
	}

	friend MATH_API bool operator!=(const Vector2& lhs, const Vector2& rhs)
	{
		return !lhs.IsEqual(rhs);
	}

	friend MATH_API bool operator>(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x > rhs.x
			&& lhs.y > rhs.y;
	}

	friend MATH_API bool operator>=(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x >= rhs.x
			&& lhs.y >= rhs.y;
	}

	friend MATH_API bool operator<(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x < rhs.x
			&& lhs.y < rhs.y;
	}

	friend MATH_API bool operator<=(const Vector2& lhs, const Vector2& rhs)
	{
		return lhs.x <= rhs.x
			&& lhs.y <= rhs.y;
	}
};
