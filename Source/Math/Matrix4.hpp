// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Vector4.hpp"
#include "Math/MathDefinitions.hpp"
#include "Math/MathAPI.hpp"

struct Quat;

// TODO - Make the set and constructor methods be free floating methods that
// return a constructed matrix of that type

class MATH_API Matrix4
{
public:
	static const Matrix4 Identity;

public:
	Matrix4();
	explicit Matrix4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);
	explicit Matrix4(MatrixSpecialType specialEnum);
	explicit Matrix4(MatrixTransType transEnum, const Vector4& transVec);
	explicit Matrix4(MatrixTransType transEnum, f32 x, f32 y, f32 z);
	explicit Matrix4(RotType rotationEnum, f32 angle);
	explicit Matrix4(Rot3AxisType multiAxisEnum, f32 xAngleRad, f32 yAngleRad, f32 zAngleRad);
	explicit Matrix4(RotAxisAngleType axisEnum, const Vector4& vect, f32 angleRads);
	explicit Matrix4(RotOrientType orientEnum, const Vector4& dof, const Vector4& up);
	explicit Matrix4(MatrixScaleType scaleEnum, const Vector4& scaleVec);
	explicit Matrix4(MatrixScaleType scaleEnum, f32 sx, f32 sy, f32 sz);
	Matrix4(const Quat& q);

	// Setting specific matrix types
	void Set(MatrixTransType transEnum, const Vector4& transVec);
	void Set(MatrixTransType transEnum, f32 x, f32 y, f32 z);
	void Set(RotType rotationEnum, f32 angle);
	void Set(Rot3AxisType axisEnum, f32 xRad, f32 yRad, f32 zRad);
	void Set(RotAxisAngleType axisEnum, const Vector4& vect, f32 angleRads);
	void Set(RotOrientType orientEnum, const Vector4& dof, const Vector4& up);
	void Set(const struct Quat& q);
	void Set(MatrixScaleType scaleEnum, const Vector4& scaleVec);
	void Set(MatrixScaleType scaleEnum, f32 sx, f32 sy, f32 sz);
	void Set(MatrixSpecialType specialEnum);
	void Set(const Vector4 &axis, f32 angle);

	// Setting matrix
	void Set(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);
	void Set(MatrixRowType rowEnum, const Vector4& rowVec);
	// Get row of matrix
	Vector4 Get(MatrixRowType rowEnum) const;
	
	// Matrix operations

	// Determinant
	f32  Determinant() const;

	// Inverse
	void Inverse();
	Matrix4 GetInverse() const;

	// Transpose
	void Transpose();
	Matrix4 GetTranspose() const;

	bool IsIdentity(f32 epsilon = Math::InternalTolerence) const;
	bool IsEqual(const Matrix4& m, f32 epsilon = Math::InternalTolerence) const;

	Matrix4 operator+() const;
	Matrix4 operator-() const;

	Matrix4 operator+(const Matrix4& m) const;
	Matrix4 operator-(const Matrix4& m) const;
	Matrix4 operator*(const Matrix4& m) const;
	Matrix4 operator*(f32 s) const;

	Matrix4& operator+=(const Matrix4& m);
	Matrix4& operator-=(const Matrix4& m);
	Matrix4& operator*=(const Matrix4& m);
	Matrix4& operator*=(f32 s);

	//friend MATH_API Vector3 operator*(const Vector3& v, const Matrix4& m);
	friend MATH_API Vector4 operator*(const Vector4& v, const Matrix4& m);
	friend MATH_API Matrix4 operator*(f32 s, const Matrix4& m);

	friend MATH_API Vector4& operator*=(Vector4& v, const Matrix4& m);

	friend MATH_API bool operator==(const Matrix4& lhs, const Matrix4& rhs)
	{
		return lhs.IsEqual(rhs);
	}
	
	friend MATH_API bool operator!=(const Matrix4& lhs, const Matrix4& rhs)
	{
		return !lhs.IsEqual(rhs);
	}

	forceinline f32& m0();
	forceinline f32& m1();
	forceinline f32& m2();
	forceinline f32& m3();
	forceinline f32& m4();
	forceinline f32& m6();
	forceinline f32& m7();
	forceinline f32& m8();
	forceinline f32& m9();
	forceinline f32& m5();
	forceinline f32& m10();
	forceinline f32& m11();
	forceinline f32& m12();
	forceinline f32& m13();
	forceinline f32& m14();
	forceinline f32& m15();

	forceinline const f32& m0() const;
	forceinline const f32& m1() const;
	forceinline const f32& m2() const;
	forceinline const f32& m3() const;
	forceinline const f32& m4() const;
	forceinline const f32& m5() const;
	forceinline const f32& m6() const;
	forceinline const f32& m7() const;
	forceinline const f32& m8() const;
	forceinline const f32& m9() const;
	forceinline const f32& m10() const;
	forceinline const f32& m11() const;
	forceinline const f32& m12() const;
	forceinline const f32& m13() const;
	forceinline const f32& m14() const;
	forceinline const f32& m15() const;

	forceinline f32& operator[](m0_enum);
	forceinline f32& operator[](m1_enum);
	forceinline f32& operator[](m2_enum);
	forceinline f32& operator[](m3_enum);
	forceinline f32& operator[](m4_enum);
	forceinline f32& operator[](m5_enum);
	forceinline f32& operator[](m6_enum);
	forceinline f32& operator[](m7_enum);
	forceinline f32& operator[](m8_enum);
	forceinline f32& operator[](m9_enum);
	forceinline f32& operator[](m10_enum);
	forceinline f32& operator[](m11_enum);
	forceinline f32& operator[](m12_enum);
	forceinline f32& operator[](m13_enum);
	forceinline f32& operator[](m14_enum);
	forceinline f32& operator[](m15_enum);

	forceinline const f32& operator[](m0_enum) const;
	forceinline const f32& operator[](m1_enum) const;
	forceinline const f32& operator[](m2_enum) const;
	forceinline const f32& operator[](m3_enum) const;
	forceinline const f32& operator[](m4_enum) const;
	forceinline const f32& operator[](m5_enum) const;
	forceinline const f32& operator[](m6_enum) const;
	forceinline const f32& operator[](m7_enum) const;
	forceinline const f32& operator[](m8_enum) const;
	forceinline const f32& operator[](m9_enum) const;
	forceinline const f32& operator[](m10_enum) const;
	forceinline const f32& operator[](m11_enum) const;
	forceinline const f32& operator[](m12_enum) const;
	forceinline const f32& operator[](m13_enum) const;
	forceinline const f32& operator[](m14_enum) const;
	forceinline const f32& operator[](m15_enum) const;
	

private:
	// 00, 01, 02, 03
	Vector4 v0;
	// 10, 11, 12, 13
	Vector4 v1;
	// 20, 21, 22, 23
	Vector4 v2;
	// 30, 31, 32, 33
	Vector4 v3;

};

f32& Matrix4::m0()
{
	return v0.x;
}

forceinline f32& Matrix4::m1()
{
	return v0.y;
}

forceinline f32& Matrix4::m2()
{
	return v0.z;
}

forceinline f32& Matrix4::m3()
{
	return v0.w;
}

forceinline f32& Matrix4::m4()
{
	return v1.x;
}

forceinline f32& Matrix4::m5()
{
	return v1.y;
}

forceinline f32& Matrix4::m6()
{
	return v1.z;
}

forceinline f32& Matrix4::m7()
{
	return v1.w;
}

forceinline f32& Matrix4::m8()
{
	return v2.x;
}

forceinline f32& Matrix4::m9()
{
	return v2.y;
}

forceinline f32& Matrix4::m10()
{
	return v2.z;
}

forceinline f32& Matrix4::m11()
{
	return v2.w;
}

forceinline f32& Matrix4::m12()
{
	return v3.x;
}

forceinline f32& Matrix4::m13()
{
	return v3.y;
}

forceinline f32& Matrix4::m14()
{
	return v3.z;
}

forceinline f32& Matrix4::m15()
{
	return v3.w;
}

forceinline const f32& Matrix4::m0() const
{
	return v0.x;
}

forceinline const f32& Matrix4::m1() const
{
	return v0.y;
}

forceinline const f32& Matrix4::m2() const
{
	return v0.z;
}

forceinline const f32& Matrix4::m3() const
{
	return v0.w;
}

forceinline const f32& Matrix4::m4() const
{
	return v1.x;
}

forceinline const f32& Matrix4::m5() const
{
	return v1.y;
}

forceinline const f32& Matrix4::m6() const
{
	return v1.z;
}

forceinline const f32& Matrix4::m7() const
{
	return v1.w;
}

forceinline const f32& Matrix4::m8() const
{
	return v2.x;
}

forceinline const f32& Matrix4::m9() const
{
	return v2.y;
}

forceinline const f32& Matrix4::m10() const
{
	return v2.z;
}

forceinline const f32& Matrix4::m11() const
{
	return v2.w;
}

forceinline const f32& Matrix4::m12() const
{
	return v3.x;
}

forceinline const f32& Matrix4::m13() const
{
	return v3.y;
}

forceinline const f32& Matrix4::m14() const
{
	return v3.z;
}

forceinline const f32& Matrix4::m15() const
{
	return v3.w;
}

forceinline f32& Matrix4::operator[](m0_enum)
{
	return v0.x;
}

forceinline f32& Matrix4::operator[](m1_enum)
{
	return v0.y;
}

forceinline f32& Matrix4::operator[](m2_enum)
{
	return v0.z;
}

forceinline f32& Matrix4::operator[](m3_enum)
{
	return v0.w;
}

forceinline f32& Matrix4::operator[](m4_enum)
{
	return v1.x;
}

forceinline f32& Matrix4::operator[](m5_enum)
{
	return v1.y;
}

forceinline f32& Matrix4::operator[](m6_enum)
{
	return v1.z;
}

forceinline f32& Matrix4::operator[](m7_enum)
{
	return v1.w;
}

forceinline f32& Matrix4::operator[](m8_enum)
{
	return v2.x;
}

forceinline f32& Matrix4::operator[](m9_enum)
{
	return v2.y;
}

forceinline f32& Matrix4::operator[](m10_enum)
{
	return v2.z;
}

forceinline f32& Matrix4::operator[](m11_enum)
{
	return v2.w;
}

forceinline f32& Matrix4::operator[](m12_enum)
{
	return v3.x;
}

forceinline f32& Matrix4::operator[](m13_enum)
{
	return v3.y;
}

forceinline f32& Matrix4::operator[](m14_enum)
{
	return v3.z;
}

forceinline f32& Matrix4::operator[](m15_enum)
{
	return v3.w;
}

forceinline const f32& Matrix4::operator[](m0_enum) const
{
	return v0.x;
}

forceinline const f32& Matrix4::operator[](m1_enum) const
{
	return v0.y;
}

forceinline const f32& Matrix4::operator[](m2_enum) const
{
	return v0.z;
}

forceinline const f32& Matrix4::operator[](m3_enum) const
{
	return v0.w;
}

forceinline const f32& Matrix4::operator[](m4_enum) const
{
	return v1.x;
}

forceinline const f32& Matrix4::operator[](m5_enum) const
{
	return v1.y;
}

forceinline const f32& Matrix4::operator[](m6_enum) const
{
	return v1.z;
}

forceinline const f32& Matrix4::operator[](m7_enum) const
{
	return v1.w;
}

forceinline const f32& Matrix4::operator[](m8_enum) const
{
	return v2.x;
}

forceinline const f32& Matrix4::operator[](m9_enum) const
{
	return v2.y;
}

forceinline const f32& Matrix4::operator[](m10_enum) const
{
	return v2.z;
}

forceinline const f32& Matrix4::operator[](m11_enum) const
{
	return v2.w;
}

forceinline const f32& Matrix4::operator[](m12_enum) const
{
	return v3.x;
}

forceinline const f32& Matrix4::operator[](m13_enum) const
{
	return v3.y;
}

forceinline const f32& Matrix4::operator[](m14_enum) const
{
	return v3.z;
}

forceinline const f32& Matrix4::operator[](m15_enum) const
{
	return v3.w;
}
