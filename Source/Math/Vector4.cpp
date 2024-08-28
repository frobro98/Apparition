// Copyright 2020, Nathan Blane

#include "Vector4.hpp"
#include "Vector3.hpp"
#include "Vector2.hpp"
#include "MathFunctions.hpp"
#include "Internal/VectorImplementation.h"

const Vector4 Vector4::RightAxis(1, 0, 0);
const Vector4 Vector4::UpAxis(0, 1, 0);
const Vector4 Vector4::ForwardAxis(0, 0, 1);
const Vector4 Vector4::Zero(0, 0, 0);
const Vector4 Vector4::One(1, 1, 1);

Vector4::Vector4(float x_, float y_, float z_, float w_)
	: x(x_), y(y_), z(z_), w(w_)
{
}

Vector4::Vector4(const Vector3& v, f32 w_)
	: x(v.x), y(v.y), z(v.z), w(w_)
{
}

Vector4::Vector4(const Vector2 & v, f32 z_, f32 w_)
	: x(v.x), y(v.y), z(z_), w(w_)
{
}

float Vector4::Dot(const Vector4& other) const
{
	return Internal::VectorDotVector(*this, other);
}

Vector4 Vector4::Cross(const Vector4& other) const
{
	Vector4 out;
	Internal::VectorCrossVector(*this, other, out);
	return out;
}

void Vector4::Normalize()
{
	Internal::VectorNormalize(*this);
}

Vector4 Vector4::GetNormalized() const
{
	Vector4 ret(*this);
	Internal::VectorNormalize(ret);
	return ret;
}

float Vector4::Magnitude() const
{
	return Internal::VectorMagnitude(*this);
}

float Vector4::MagnitudeSqr() const
{
	return Internal::VectorMagnitudeSqr(*this);
}

float Vector4::InverseMagnitude() const
{
	return Internal::VectorInverseMagnitude(*this);
}

bool Vector4::IsEqual(const Vector4& other, float epsilon) const
{
	return (Math::IsEqual(x, other.x, epsilon) && 
			Math::IsEqual(y, other.y, epsilon) &&
			Math::IsEqual(z, other.z, epsilon) &&
			Math::IsEqual(w, other.w, epsilon));
}

bool Vector4::IsZero(float epsilon) const
{
	return (Math::IsZero(x, epsilon) &&
			Math::IsZero(y, epsilon) && 
			Math::IsZero(z, epsilon) && 
			Math::IsOne(w, epsilon));
}

// float Vector::GetAngle(const Vector& other) const
// {
// 	return Internal::VectorAngleBetweenVectors(*this, other);
// }

Vector4 Vector4::operator+() const
{
	return Vector4(+x, +y, +z);
}

Vector4 Vector4::operator-() const
{
	return Vector4(-x, -y, -z);
}

Vector4& Vector4::operator+=(const Vector4& other)
{
	Internal::VectorAddTo(*this, other);
	return *this;
}

Vector4& Vector4::operator-=(const Vector4& other)
{
	Internal::VectorSubTo(*this, other);
	return *this;
}

Vector4 Vector4::operator+(const Vector4& other) const
{
	Vector4 v(*this);
	return v += other;
}

Vector4 Vector4::operator-(const Vector4& other) const
{
	Vector4 v(*this);
	return v -= other;
}

Vector4& Vector4::operator*=(float s)
{
	Internal::VectorMultiTo(*this, s);
	return *this;
}

Vector4 Vector4::operator*(float s) const
{
	Vector4 v(*this);
	v *= s;
	return v;
}

Vector4 operator*(float s, const Vector4& vec)
{
	Vector4 v(vec);
	v *= s;
	return v;
}
