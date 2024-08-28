// Copyright 2020, Nathan Blane

#include "Debugging/Assertion.hpp"
#include "Quat.hpp"
#include "Vector4.hpp"
#include "MathFunctions.hpp"
#include "Internal/QuaternionImplementation.hpp"

const Quat Quat::Identity = Quat(0, 0, 0, 1);
const Quat Quat::Zero = Quat(0, 0, 0, 0);

Quat::Quat(float x_, float y_, float z_, float w_)
	: x(x_),
	y(y_),
	z(z_),
	w(w_)
{
}

Quat::Quat(RotType type, float radAngle)
{
	float angle = radAngle * .5f;
	switch (type)
	{
		case ROT_X:
		{
			Vector4 v(1.f, 0.f, 0.f);
			v *= Math::Sin(angle);
			x = v.x;
			y = v.y;
			z = v.z;
			w = Math::Cos(angle);
		}break;
		case ROT_Y:
		{
			Vector4 v(0.f, 1.f, 0.f);
			v *= Math::Sin(angle);
			x = v.x;
			y = v.y;
			z = v.z;
			w = Math::Cos(angle);
		}break;
		case ROT_Z:
		{
			Vector4 v(0.f, 0.f, 1.f);
			v *= Math::Sin(angle);
			x = v.x;
			y = v.y;
			z = v.z;
			w = Math::Cos(angle);
		}break;
		default:
		{
			Assert(false);
		}break;
	}
}

Quat::Quat(Rot3AxisType type, float radX, float radY, float radZ)
{
	Set(type, radX, radY, radZ);
}

Quat::Quat(RotAxisAngleType type, const Vector4& axis, float angleRad)
{
	Set(type, axis, angleRad);
}

Quat::Quat(RotOrientType type, const Vector4& dof, const Vector4& up)
{
	Set(type, dof, up);
}

Quat::Quat(const Vector4& v, float angle)
{
	Set(v, angle);
}

Quat::Quat(const Matrix4 & mat)
{
	Set(mat);
}

Quat& Quat::operator=(const Matrix4& m)
{
	Set(m);
	return *this;
}

void Quat::Set(float x_, float y_, float z_, float w_)
{
	x = x_;
	y = y_;
	z = z_;
	w = w_;
}

void Quat::Set(const Matrix4& mat)
{
	Internal::QuaternionSetMatrix(*this, mat);
}

void Quat::Set(const Vector4& vect, float real)
{
	x = vect.x;
	y = vect.y;
	z = vect.z;
	w = real;
}

void Quat::Set(MatrixSpecialType type)
{
	if (type == IDENTITY)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		w = 1.f;
	}
	else if (type == ZERO)
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
		w = 0.f;
	}
}

void Quat::Set(RotType type, float radAngle)
{
	float angle = radAngle * .5f;
	switch (type)
	{
	case ROT_X:
	{
		Vector4 v(1.f, 0.f, 0.f);
		v *= Math::Sin(angle);
		x = v.x;
		y = v.y;
		z = v.z;
		w = Math::Cos(angle);
	}break;
	case ROT_Y:
	{
		Vector4 v(0.f, 1.f, 0.f);
		v *= Math::Sin(angle);
		x = v.x;
		y = v.y;
		z = v.z;
		w = Math::Cos(angle);
	}break;
	case ROT_Z:
	{
		Vector4 v(0.f, 0.f, 1.f);
		v *= Math::Sin(angle);
		x = v.x;
		y = v.y;
		z = v.z;
		w = Math::Cos(angle);
	}break;
	default:
	{
		Assert(false);
	}break;
	}
}

void Quat::Set(Rot3AxisType, float radX, float radY, float radZ)
{
	Quat qX(ROT_X, radX);
	Quat qY(ROT_Y, radY);
	Quat qZ(ROT_Z, radZ);
	Set(qX*qY*qZ);
}

void Quat::Set(RotAxisAngleType, const Vector4& axis, float angleRad)
{
	Vector4 v = axis.GetNormalized();
	float angle = angleRad * .5f;
	float sinAngle = Math::Sin(angle);
	x = v.x * sinAngle;
	y = v.y * sinAngle;
	z = v.z * sinAngle;
	w = Math::Cos(angle);
}

void Quat::Set(RotOrientType type, const Vector4& dof, const Vector4& up)
{
	Matrix4 m(type, dof, up);
	Set(m);
}

void Quat::SetVector(const Vector4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

Vector4 Quat::GetVector() const
{
	return Vector4(x, y, z);
}

Vector4 Quat::GetAxis() const
{
	return Vector4(x, y, z);
}

float Quat::GetAngle() const
{
	return 2.f * Math::Acos(w);
}

Quat& Quat::Normalize()
{
	Internal::QuaternionNormalize(*this);
	return *this;
}

float Quat::Dot(const Quat& q) const
{
	return Internal::QuaternionDotQuaternion(*this, q);
}

Quat Quat::GetNormalized() const
{
	Quat q(*this);
	Internal::QuaternionNormalize(q);
	return q;
}

void Quat::Inverse()
{
	Internal::QuaternionInverse(*this);
}


Quat Quat::GetInverse() const
{
	Quat q(*this);
	Internal::QuaternionInverse(q);
	return q;
}

void Quat::Conjugate()
{
	x = -x;
	y = -y;
	z = -z;
}

Quat Quat::GetConjugate() const
{
	return Quat(-x, -y, -z, w);
}

void Quat::Transpose()
{
	Matrix4 m(*this);
	m.Transpose();
	Set(m);
}

Quat Quat::GetTranspose() const
{
	Matrix4 m(*this);
	m.Transpose();
	return Quat(m);
}

float Quat::Magnitude() const
{
	return Internal::QuaternionMagnitude(*this);
}

float Quat::MagnitudeSqr() const
{
	return Internal::QuaternionMagnitudeSqr(*this);
}

float Quat::InverseMagnitude() const
{
	return Internal::QuaternionMagnitudeInverse(*this);
}

void Quat::Lqcvq(const Vector4& v, Vector4& vOut) const
{
	Internal::QuaternionLqcvq(*this, v, vOut);
}

void Quat::Lqvqc(const Vector4& v, Vector4 & vOut) const
{
	Internal::QuaternionLqvqc(*this, v, vOut);
}

bool Quat::IsEqual(const Quat& q, float tolerance) const
{
	return Math::IsEqual(x, q.x, tolerance) &&
		   Math::IsEqual(y, q.y, tolerance) &&
		   Math::IsEqual(z, q.z, tolerance) &&
		   Math::IsEqual(w, q.w, tolerance);
}

bool Quat::IsNegativeEqual(const Quat& q, float tolerance) const
{
	return Math::IsEqual(x, -q.x, tolerance) &&
		   Math::IsEqual(y, -q.y, tolerance) &&
		   Math::IsEqual(z, -q.z, tolerance) &&
		   Math::IsEqual(w, -q.w, tolerance);
}

bool Quat::IsEquivalent(const Quat& q, float tolerance) const
{
	return IsEqual(q, tolerance) || IsNegativeEqual(q, tolerance);
}

bool Quat::IsConjugateEqual(const Quat& q, float tolerance) const
{
	return Math::IsEqual(x, -q.x, tolerance) &&
		   Math::IsEqual(y, -q.y, tolerance) &&
		   Math::IsEqual(z, -q.z, tolerance) &&
		   Math::IsEqual(w,  q.w, tolerance);
}

bool Quat::IsIdentity(float tolerance) const
{
	return Math::IsEqual(x, 0.f, tolerance) &&
		   Math::IsEqual(y, 0.f, tolerance) &&
		   Math::IsEqual(z, 0.f, tolerance) &&
		   Math::IsEqual(w, 1.f, tolerance);
}

bool Quat::IsNormalized(float tolerance) const
{
	return Math::IsEqual(MagnitudeSqr(), 1.f, tolerance);
}

bool Quat::IsZero(float tolerance) const
{
	return Math::IsEqual(x, 0.f, tolerance) &&
		   Math::IsEqual(y, 0.f, tolerance) &&
		   Math::IsEqual(z, 0.f, tolerance) &&
		   Math::IsEqual(w, 0.f, tolerance);
}

Quat Quat::MultiplyByElement(const Quat& q) const
{
	return Quat(x*q.x, y*q.y, z*q.z, w*q.w);
}

Quat Quat::operator+() const
{
	return Quat(+x, +y, +z, +w);
}

Quat Quat::operator-() const
{
	return Quat(-x, -y, -z, -w);
}

Quat& Quat::operator+=(const Quat& q)
{
	Internal::QuaternionAddTo(*this, q);
	return *this;
}

Quat& Quat::operator-=(const Quat& q)
{
	Internal::QuaternionSubTo(*this, q);
	return *this;
}

Quat& Quat::operator*=(const Quat& q)
{
	Internal::QuaternionMultiTo(*this, q);
	return *this;
}

Quat& Quat::operator/=(const Quat& q)
{
	Internal::QuaternionDivTo(*this, q);
	return *this;
}

Quat Quat::operator+(const Quat& q) const
{
	Quat quat(*this);
	quat += q;
	return quat;
}

Quat Quat::operator-(const Quat& q) const
{
	Quat quat(*this);
	quat -= q;
	return quat;
}

Quat Quat::operator*(const Quat& q) const
{
	Quat quat(*this);
	quat *= q;
	return quat;
}

Quat Quat::operator/(const Quat& q) const
{
	Quat quat(*this);
	quat /= q;
	return quat;
}

Matrix4& operator*=(Matrix4& m, const Quat& q)
{
	Matrix4 mq(q);
	m *= mq;
	return m;
}

Quat& operator*=(Quat& q, const Matrix4& m)
{
	Quat mQ(m);
	q *= mQ;
	return q;
}

Matrix4 operator*(const Matrix4& m, const Quat& q)
{
	Matrix4 mq(q);
	return Matrix4(m * mq);
}

Matrix4 operator*(const Quat& q, const Matrix4& m)
{
	Matrix4 mq(q);
	return Matrix4(mq * m);
}

Vector4& operator*=(Vector4& v, const Quat& q)
{
	q.Lqcvq(v, v);
	return v;
}

Vector4 operator*(const Vector4& v, const Quat& q)
{
	Vector4 vec(v);
	vec *= q;
	return vec;
}

Vector4 operator*(const Quat& q, const Vector4& v)
{
	Vector4 vec;
	q.Lqvqc(v, vec);
	return vec;
}

Quat& operator+=(Quat& q, float val)
{
	Internal::QuaternionAddTo(q, val);
	return q;
}

Quat& operator-=(Quat& q, float val)
{
	Internal::QuaternionSubTo(q, val);
	return q;
}

Quat& operator*=(Quat& q, float val)
{
	Internal::QuaternionMultiTo(q, val);
	return q;
}

Quat& operator/=(Quat& q, float val)
{
	Internal::QuaternionDivTo(q, val);
	return q;
}

Quat operator+(const Quat& q, float val)
{
	Quat quat(q);
	quat += val;
	return quat;
}

Quat operator+(float val, const Quat& q)
{
	Quat quat(q);
	quat += val;
	return quat;
}

Quat operator-(const Quat& q, float val)
{
	Quat quat(q);
	quat -= val;
	return quat;
}

Quat operator-(float val, const Quat& q)
{
	Quat quat(q);
	Internal::QuaternionSubTo(val, quat);
	return quat;
}

Quat operator*(const Quat& q, float val)
{
	Quat quat(q);
	quat *= val;
	return quat;
}

Quat operator*(float val, const Quat& q)
{
	Quat quat(q);
	quat *= val;
	return quat;
}

Quat operator/(const Quat& q, float val)
{
	Quat quat(q);
	quat /= val;
	return quat;
}

Quat operator/(float val, const Quat& q)
{
	Quat quat(q);
	Internal::QuaternionDivTo(val, quat);
	return quat;
}
