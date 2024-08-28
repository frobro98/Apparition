// Copyright 2020, Nathan Blane

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Math/MathFunctions.hpp"
#include "Internal/VectorImplementation.h"

const Vector2 Vector2::RightAxis(1, 0);
const Vector2 Vector2::UpAxis(0, 1);
const Vector2 Vector2::Zero(0, 0);
const Vector2 Vector2::One(1, 1);

Vector2::Vector2(f32 x_, f32 y_)
	:x(x_), y(y_)
{
}

Vector2::Vector2(const Vector4& v4)
	: x(v4.x), y(v4.y)
{
}

Vector2::Vector2(const Vector3& v3)
	: x(v3.x), y(v3.y)
{
}

float Vector2::Dot(const Vector2& other) const
{
	return Internal::VectorDotVector(*this, other);
}

void Vector2::Normalize()
{
	Internal::VectorNormalize(*this);
}

Vector2 Vector2::GetNormalized() const
{
	Vector2 ret(*this);
	Internal::VectorNormalize(ret);
	return ret;
}

float Vector2::Magnitude() const
{
	return Internal::VectorMagnitude(*this);
}

float Vector2::MagnitudeSqr() const
{
	return Internal::VectorMagnitudeSqr(*this);
}

float Vector2::InverseMagnitude() const
{
	return Internal::VectorInverseMagnitude(*this);
}

bool Vector2::IsEqual(const Vector2 & other, float epsilon) const
{
	return (Math::IsEqual(x, other.x, epsilon) &&
		Math::IsEqual(y, other.y, epsilon));
}

bool Vector2::IsZero(float epsilon) const
{
	return (Math::IsZero(x, epsilon) &&
		Math::IsZero(y, epsilon));
}

Vector2 Vector2::operator+() const
{
	return Vector2(+x, +y);
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2& Vector2::operator+=(const Vector2& other)
{
	Internal::VectorAddTo(*this, other);
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& other)
{
	Internal::VectorSubTo(*this, other);
	return *this;
}

Vector2& Vector2::operator*=(const Vector2& other)
{
	Internal::VectorMultiTo(*this, other);
	return *this;
}

Vector2 Vector2::operator+(const Vector2& other) const
{
	Vector2 v(*this);
	return v += other;
}

Vector2 Vector2::operator-(const Vector2 & other) const
{
	Vector2 v(*this);
	return v -= other;
}

Vector2 Vector2::operator*(const Vector2 & other) const
{
	Vector2 v(*this);
	v *= other;
	return v;
}

Vector2& Vector2::operator*=(f32 s)
{
	Internal::VectorMultiTo(*this, s);
	return *this;
}

Vector2 Vector2::operator*(f32 s) const
{
	Vector2 v(*this);
	v *= s;
	return v;
}

Vector2 operator*(f32 s, const Vector2& vec)
{
	Vector2 v(vec);
	v *= s;
	return v;
}
