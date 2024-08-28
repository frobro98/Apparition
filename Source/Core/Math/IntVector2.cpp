// Copyright 2020, Nathan Blane

#include "IntVector2.hpp"
#include "Vector2.hpp"

const IntVector2 IntVector2::Zero(0, 0);
const IntVector2 IntVector2::One(1, 1);

IntVector2::IntVector2(i32 x_, i32 y_)
	: x(x_),
	y(y_)
{
}

IntVector2::IntVector2(const Vector2& v)
	: x((i32)v.x),
	y((i32)v.y)
{
}

bool IntVector2::IsZero() const
{
	return x == 0 && y == 0;
}

IntVector2 IntVector2::operator+() const
{
	return IntVector2(+x, +y);
}

IntVector2 IntVector2::operator-() const
{
	return IntVector2(-x, -y);
}

IntVector2& IntVector2::operator+=(const IntVector2& other)
{
	x += other.x;
	y += other.y;
	return *this;
}

IntVector2& IntVector2::operator-=(const IntVector2& other)
{
	x -= other.x;
	y -= other.y;
	return *this;
}

IntVector2& IntVector2::operator*=(i32 scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

IntVector2& IntVector2::operator/=(i32 scalar)
{
	x /= scalar;
	y /= scalar;
	return *this;
}

