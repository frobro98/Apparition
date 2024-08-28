// Copyright 2020, Nathan Blane

#include "Vector3.hpp"
#include "Vector2.hpp"
#include "Vector4.hpp"
#include "Math/MathFunctions.hpp"
#include "Internal/VectorImplementation.h"

const Vector3 Vector3::RightAxis(1, 0, 0);
const Vector3 Vector3::UpAxis(0, 1, 0);
const Vector3 Vector3::ForwardAxis(0, 0, 1);
const Vector3 Vector3::Zero(0, 0, 0);
const Vector3 Vector3::One(1, 1, 1);

Vector3::Vector3(f32 x_, f32 y_, f32 z_)
	: x(x_), y(y_), z(z_)
{
}

Vector3::Vector3(const Vector4& v4)
	: x(v4.x), y(v4.y), z(v4.z)
{
}

Vector3::Vector3(const Vector2& v2, f32 inZ)
	: x(v2.x), y(v2.y), z(inZ)
{
}

float Vector3::Dot(const Vector3& other) const
{
	return Internal::VectorDotVector(*this, other);
}

Vector3 Vector3::Cross(const Vector3& other) const
{
	Vector3 out;
	Internal::VectorCrossVector(*this, other, out);
	return out;
}

void Vector3::Normalize()
{
	Internal::VectorNormalize(*this);
}

Vector3 Vector3::GetNormalized() const
{
	Vector3 ret(*this);
	Internal::VectorNormalize(ret);
	return ret;
}

float Vector3::Magnitude() const
{
	return Internal::VectorMagnitude(*this);
}

float Vector3::MagnitudeSqr() const
{
	return Internal::VectorMagnitudeSqr(*this);
}

float Vector3::InverseMagnitude() const
{
	return Internal::VectorInverseMagnitude(*this);
}

bool Vector3::IsEqual(const Vector3& other, float epsilon) const
{
	return (Math::IsEqual(x, other.x, epsilon) &&
		Math::IsEqual(y, other.y, epsilon) &&
		Math::IsEqual(z, other.z, epsilon));
}

bool Vector3::IsZero(float epsilon) const
{
	return (Math::IsZero(x, epsilon) &&
		Math::IsZero(y, epsilon) &&
		Math::IsZero(z, epsilon));
}

Vector3 Vector3::operator+() const
{
	return Vector3(+x, +y, +z);
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3 & Vector3::operator+=(const Vector3& other)
{
	Internal::VectorAddTo(*this, other);
	return *this;
}

Vector3 & Vector3::operator-=(const Vector3& other)
{
	Internal::VectorSubTo(*this, other);
	return *this;
}

Vector3 Vector3::operator+(const Vector3& other) const
{
	Vector3 v(*this);
	return v += other;
}

Vector3 Vector3::operator-(const Vector3& other) const
{
	Vector3 v(*this);
	return v -= other;
}

Vector3 & Vector3::operator*=(f32 s)
{
	Internal::VectorMultiTo(*this, s);
	return *this;
}

Vector3 Vector3::operator*(f32 s) const
{
	Vector3 v(*this);
	v *= s;
	return v;
}

Vector3 operator*(f32 s, const Vector3& vec)
{
	Vector3 v(vec);
	v *= s;
	return v;
}
