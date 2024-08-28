// Copyright 2020, Nathan Blane

#include "Matrix2.hpp"
#include "Quat2.hpp"
#include "MathFunctions.hpp"

Matrix2::Matrix2(const Vector2& row0, const Vector2& row1)
	: v0(row0), v1(row1)
{
}

Matrix2::Matrix2(float angle)
{
	f32 cosTheta = Math::Cos(angle);
	f32 sinTheta = Math::Sin(angle);
	v0 = Vector2(cosTheta, sinTheta);
	v1 = Vector2(-sinTheta, cosTheta);
}

Matrix2::Matrix2(MatrixScaleType, f32 uniformScale)
	: v0(uniformScale, 0),
	v1(0, uniformScale)
{
}

Matrix2::Matrix2(MatrixScaleType, const Vector2& scaleVec)
	: v0(scaleVec.x, 0),
	v1(0, scaleVec.y)
{
}

Matrix2::Matrix2(MatrixScaleType, float sx, float sy)
	: v0(sx, 0),
	v1(0, sy)
{
}

Matrix2::Matrix2(const Quat2& q)
	: v0(q.real, q.imaginary),
	v1(-q.imaginary, q.real)
{
}

void Matrix2::Set(float angle)
{
	f32 cosTheta = Math::Cos(angle);
	f32 sinTheta = Math::Sin(angle);
	v0 = Vector2(cosTheta, sinTheta);
	v1 = Vector2(-sinTheta, cosTheta);
}

void Matrix2::Set(const Quat2& q)
{
	v0 = Vector2(q.real, q.imaginary);
	v1 = Vector2(-q.imaginary, q.real);
}

void Matrix2::Set(MatrixScaleType, f32 scale)
{
	v0 = Vector2(scale, 0.f);
	v1 = Vector2(0, scale);
}

void Matrix2::Set(MatrixScaleType, const Vector2& scaleVec)
{
	v0 = Vector2(scaleVec.x, 0.f);
	v1 = Vector2(0, scaleVec.y);
}

void Matrix2::Set(MatrixScaleType, f32 sx, f32 sy)
{
	v0 = Vector2(sx, 0.f);
	v1 = Vector2(0, sy);
}

void Matrix2::Set(const Vector2& row0, const Vector2& row1)
{
	v0 = row0;
	v1 = row1;
}

float Matrix2::Determinant() const
{
	return v0.x * v1.y - v0.y * v1.x;
}

void Matrix2::Inverse()
{
	f32 detScalar = Determinant();
	if (Math::IsNonZero(detScalar))
	{
		f32 invDetScalar = 1.f / detScalar;

		// Transposed cofactor of this matrix
		f32 cofM0 = v1.y; //_m3;
		f32 cofM3 = v0.x; //_m0;
		f32 cofM1 = -v1.x; //-_m2;
		f32 cofM2 = -v0.y; //-_m1;

		v0 = Vector2(
			cofM0 * invDetScalar,
			cofM1 * invDetScalar
		);
		v1 = Vector2(
			cofM2 * invDetScalar,
			cofM3 * invDetScalar
		);
	}
}

Matrix2 Matrix2::GetInverse() const
{
	Matrix2 m(*this);
	m.Inverse();
	return m;
}

void Matrix2::Transpose()
{
	f32 tmp = v0.y;
	v0.y = v1.x;
	v1.x = tmp;
}

Matrix2 Matrix2::GetTranspose() const
{
	Matrix2 m(*this);
	m.Transpose();
	return m;
}

bool Matrix2::IsIdentity(float epsilon) const
{
	return Math::IsEqual(v0.x, 1.f, epsilon) && Math::IsEqual(v0.y, 0.f, epsilon) &&
		Math::IsEqual(v1.x, 0.f, epsilon) && Math::IsEqual(v1.y, 0.f, epsilon);
}

bool Matrix2::IsEqual(const Matrix2& m) const
{
	return v0.IsEqual(m.v0) &&
		v1.IsEqual(m.v1);
}

Matrix2 Matrix2::operator+(const Matrix2& /*m*/) const
{
	return Matrix2();
}

Matrix2 Matrix2::operator-(const Matrix2& /*m*/) const
{
	return Matrix2();
}

Matrix2 Matrix2::operator*(const Matrix2& /*m*/) const
{
	return Matrix2();
}

Matrix2 Matrix2::operator*(float /*s*/) const
{
	return Matrix2();
}

Matrix2& Matrix2::operator+=(const Matrix2& /*m*/)
{
	return *this;
}

Matrix2& Matrix2::operator-=(const Matrix2& /*m*/)
{
	return *this;
}

Matrix2 Matrix2::operator*=(const Matrix2& /*m*/)
{
	return Matrix2();
}

Matrix2& Matrix2::operator*=(float /*s*/)
{
	return *this;
}

Matrix2 Matrix2::operator+() const
{
	return Matrix2();
}

Matrix2 Matrix2::operator-() const
{
	return Matrix2();
}

float& Matrix2::m0()
{
	return v0.x;
}

float& Matrix2::m1()
{
	return v0.y;
}

float& Matrix2::m2()
{
	return v1.x;
}

float& Matrix2::m3()
{
	return v1.y;
}

const float& Matrix2::m0() const
{
	return v0.x;
}

const float& Matrix2::m1() const
{
	return v0.y;
}

const float& Matrix2::m2() const
{
	return v1.x;
}

const float& Matrix2::m3() const
{
	return v1.y;
}

float& Matrix2::operator[](m0_enum)
{
	return v0.x;
}

float& Matrix2::operator[](m1_enum)
{
	return v0.y;
}

float& Matrix2::operator[](m2_enum)
{
	return v1.x;
}

float& Matrix2::operator[](m3_enum)
{
	return v1.y;
}

const float& Matrix2::operator[](m0_enum) const
{
	return v0.x;
}

const float& Matrix2::operator[](m1_enum) const
{
	return v0.y;
}

const float& Matrix2::operator[](m2_enum) const
{
	return v1.x;
}

const float& Matrix2::operator[](m3_enum) const
{
	return v1.y;
}

Vector2 operator*(const Vector2& v, const Matrix2& m)
{
	Vector2 ret(v);
	ret *= m;
	return ret;
}

Matrix2 operator*(float s, const Matrix2& m)
{
	Matrix2 ret(m);
	ret *= s;
	return ret;
}

Vector2& operator*=(Vector2& v, const Matrix2& /*m*/)
{
	return v;
}
