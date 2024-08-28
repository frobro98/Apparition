// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Math/MathAPI.hpp"

struct Vector2;

struct MATH_API IntVector2
{
	static const IntVector2 Zero;
	static const IntVector2 One;

public:
	i32 x = 0;
	i32 y = 0;

	IntVector2() = default;
	explicit IntVector2(i32 x_, i32 y_);
	explicit IntVector2(const Vector2& v);

	bool IsZero() const;

	IntVector2 operator+() const;
	IntVector2 operator-() const;

	IntVector2& operator+=(const IntVector2& other);
	IntVector2& operator-=(const IntVector2& other);
	IntVector2& operator*=(i32 scalar);
	IntVector2& operator/=(i32 scalar);

	friend IntVector2 operator+(const IntVector2& v0, const IntVector2& v1)
	{
		IntVector2 ret(v0);
		ret += v1;
		return ret;
	}

	friend IntVector2 operator-(const IntVector2& v0, const IntVector2& v1)
	{
		IntVector2 ret(v0);
		ret -= v1;
		return ret;
	}

	friend IntVector2 operator*(const IntVector2& v, i32 scalar)
	{
		IntVector2 ret(v);
		ret *= scalar;
		return ret;
	}

	friend IntVector2 operator*(i32 scalar, const IntVector2& v)
	{
		IntVector2 ret(v);
		ret *= scalar;
		return ret;
	}

	friend IntVector2 operator/(const IntVector2& v, i32 scalar)
	{
		IntVector2 ret(v);
		ret /= scalar;
		return ret;
	}

	friend IntVector2 operator/(i32 scalar, const IntVector2& v)
	{
		IntVector2 ret(v);
		ret /= scalar;
		return ret;
	}

	friend bool operator==(const IntVector2& v0, const IntVector2& v1)
	{
		return v0.x == v1.x &&
			v0.y == v1.y;
	}

	friend bool operator!=(const IntVector2& v0, const IntVector2& v1)
	{
		return v0.x != v1.x &&
			v0.y != v1.y;
	}

	friend bool operator>(const IntVector2& v0, const IntVector2& v1)
	{
		return v0.x > v1.x &&
			v0.y > v1.y;
	}

	friend bool operator<(const IntVector2& v0, const IntVector2& v1)
	{
		return v0.x < v1.x &&
			v0.y < v1.y;
	}

	friend bool operator>=(const IntVector2& v0, const IntVector2& v1)
	{
		return v0.x >= v1.x &&
			v0.y >= v1.y;
	}

	friend bool operator<=(const IntVector2& v0, const IntVector2& v1)
	{
		return v0.x <= v1.x &&
			v0.y <= v1.y;
	}
};

