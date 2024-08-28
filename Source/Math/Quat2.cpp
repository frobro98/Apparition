// Copyright 2020, Nathan Blane

#include "Quat2.hpp"
#include "MathFunctions.hpp"

const Quat2 Quat2::Identity = Quat2();
const Quat2 Quat2::Zero = Quat2(0, 0);

Quat2::Quat2(f32 real_, f32 imag)
	: real(real_),
	imaginary(imag)
{
}

Quat2::Quat2(f32 angle)
	: real(Math::Cos(angle)),
	imaginary(Math::Sin(angle))
{
}

Quat2::Quat2(const Matrix2& mat)
	: real(mat[m0]),
	imaginary(mat[m1])
{
}

void Quat2::Set(const Matrix2& mat)
{
	real = mat[m0];
	imaginary = mat[m1];
}

void Quat2::Set(f32 theta)
{
	real = Math::Cos(theta);
	imaginary = Math::Sin(theta);
}

float Quat2::GetAngle() const
{
	return Math::Atan2(imaginary, real);
}

Quat2& Quat2::Normalize()
{
	f32 inverseMag = 1.f / MagnitudeSqr();
	real *= inverseMag;
	imaginary *= inverseMag;
	return *this;
}

Quat2 Quat2::GetNormalized() const
{
	Quat2 q(real, imaginary);

	f32 inverseMag = 1.f / MagnitudeSqr();
	q.real *= inverseMag;
	q.imaginary *= inverseMag;

	return q;
}

float Quat2::Dot(const Quat2& q) const
{
	return real * q.real + imaginary * q.imaginary;
}

void Quat2::Inverse()
{
	//float32 invMag = InverseMagnitude();
	//float32 sqInvMag = invMag * invMag;
	f32 invMagSqr = 1.f / MagnitudeSqr();
	Conjugate();
	real *= invMagSqr;
	imaginary *= invMagSqr;
}

Quat2 Quat2::GetInverse() const
{
	Quat2 q(*this);
	q.Inverse();
	return q;
}

void Quat2::Conjugate()
{
	imaginary = -imaginary;
}

Quat2 Quat2::GetConjugate() const
{
	return Quat2(real, -imaginary);
}

void Quat2::Transpose()
{
	Matrix2 m(*this);
	m.Transpose();
	Set(m);
}

Quat2 Quat2::GetTranspose() const
{
	Matrix2 m(*this);
	m.Transpose();
	return Quat2(m);
}

float Quat2::Magnitude() const
{
	return Math::Sqrt(MagnitudeSqr());
}

float Quat2::MagnitudeSqr() const
{
	return Dot(*this);
}

float Quat2::InverseMagnitude() const
{
	return 1.f / Magnitude();
}

bool Quat2::IsEqual(const Quat2& q, float tolerance) const
{
	return Math::IsEqual(real, q.real, tolerance) &&
		Math::IsEqual(imaginary, q.imaginary, tolerance);
}

bool Quat2::IsNegativeEqual(const Quat2& q, float tolerance) const
{
	return Math::IsEqual(real, -q.real, tolerance) &&
		Math::IsEqual(imaginary, -q.imaginary, tolerance);
}

bool Quat2::IsEquivalent(const Quat2& q, float tolerance) const
{
	return IsEqual(q, tolerance) || IsNegativeEqual(q, tolerance);
}

bool Quat2::IsConjugateEqual(const Quat2& q, float tolerance) const
{
	return Math::IsEqual(real, q.real, tolerance) &&
		Math::IsEqual(imaginary, -q.imaginary, tolerance);
}

bool Quat2::IsIdentity(float tolerance) const
{
	return Math::IsEqual(real, 1, tolerance) &&
		Math::IsZero(imaginary, tolerance);
}

bool Quat2::IsNormalized(float tolerance) const
{
	return Math::IsEqual(MagnitudeSqr(), 1.f, tolerance);
}

bool Quat2::IsZero(float tolerance) const
{
	return Math::IsZero(real, tolerance) &&
		Math::IsZero(imaginary, tolerance);
}

Quat2 Quat2::operator+() const
{
	return Quat2(+real, +imaginary);
}

Quat2 Quat2::operator-() const
{
	return Quat2(-real, -imaginary);
}

Quat2& Quat2::operator+=(const Quat2& q)
{
	real += q.real;
	imaginary += q.imaginary;
	return *this;
}

Quat2& Quat2::operator-=(const Quat2& q)
{
	real -= q.real;
	imaginary -= q.imaginary;
	return *this;
}

Quat2& Quat2::operator*=(const Quat2& q)
{
	real = real * q.real - imaginary * q.imaginary;
	imaginary = imaginary * q.real + real * q.imaginary;
	return *this;
}


Quat2& Quat2::operator*=(f32 val)
{
	real *= val;
	imaginary *= val;
	return *this;
}

Quat2& Quat2::operator/=(f32 val)
{
	f32 mod = 1.f / val;
	real *= mod;
	imaginary *= mod;
	return *this;
}



Quat2& operator*=(Quat2& q, const Matrix2& m)
{
	Quat2 qm(m);
	q *= qm;
	return q;
}

Matrix2& operator*=(Matrix2& m, const Quat2& q)
{
	Matrix2 mq(q);
	m *= mq;
	return m;
}

Vector2& operator*=(Vector2& v, const Quat2& q)
{
	v = Vector2(
		v.x * q.real - v.y * q.imaginary,
		v.x * q.imaginary + v.y * q.real
	);
	return v;
}



Quat2 operator+(const Quat2& q0, const Quat2& q1)
{
	Quat2 q(q0);
	q += q1;
	return q;
}

Quat2 operator-(const Quat2& q0, const Quat2& q1)
{
	Quat2 q(q0);
	q -= q1;
	return q;
}

Matrix2 operator*(const Matrix2& m, const Quat2& q)
{
	Matrix2 mq(q);
	return Matrix2(m * mq);
}

Matrix2 operator*(const Quat2& q, const Matrix2& m)
{
	Matrix2 mq(q);
	return Matrix2(mq * m);
}

Vector2 operator*(const Vector2& v, const Quat2& q)
{
	Vector2 vec(v);
	vec *= q;
	return vec;
}

Quat2 operator*(const Quat2& q, f32 val)
{
	Quat2 ret(q);
	ret *= val;
	return ret;
}

Quat2 operator*(f32 val, const Quat2& q)
{
	Quat2 ret(q);
	ret *= val;
	return ret;
}

Quat2 operator/(const Quat2& q, f32 val)
{
	Quat2 ret(q);
	ret /= val;
	return ret;
}

Quat2 operator/(f32 val, const Quat2& q)
{
	Quat2 ret(q);
	ret /= val;
	return ret;
}
