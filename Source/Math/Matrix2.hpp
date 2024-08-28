// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Vector2.hpp"
#include "Math/MathDefinitions.hpp"
#include "Math/MathAPI.hpp"

struct Quat2;

class MATH_API Matrix2
{
public:
	static const Matrix2 Identity;
	static const Matrix2 Zero;

public:
	Matrix2() = default;
	explicit Matrix2(const Vector2& row0, const Vector2& row1);
	explicit Matrix2(float angle);
	//explicit Matrix2(RotOrientType orientEnum, const Vector4& dof, const Vector4& up);
	explicit Matrix2(MatrixScaleType, f32 uniformScale);
	explicit Matrix2(MatrixScaleType, const Vector2& scaleVec);
	explicit Matrix2(MatrixScaleType, float sx, float sy);
	Matrix2(const Quat2& q);

	// Setting specific matrix types
	void Set(float angle);
	//void Set(RotOrientType orientEnum, const Vector4& dof, const Vector4& up);
	void Set(const Quat2& q);
	void Set(MatrixScaleType scaleEnum, f32 scale);
	void Set(MatrixScaleType scaleEnum, const Vector2& scaleVec);
	void Set(MatrixScaleType scaleEnum, f32 sx, f32 sy);
	void Set(const Vector2& row0, const Vector2& row1);

	// Matrix operations

	// Determinant
	float Determinant() const;

	// Inverse
	void Inverse();
	Matrix2 GetInverse() const;

	// Transpose
	void Transpose();
	Matrix2 GetTranspose() const;

	bool IsIdentity(float epsilon = Math::InternalTolerence) const;
	bool IsEqual(const Matrix2& m) const;

	// Math operators
	Matrix2 operator+(const Matrix2& m) const;
	Matrix2 operator-(const Matrix2& m) const;
	Matrix2 operator*(const Matrix2& m) const;
	Matrix2 operator*(float s) const;

	Matrix2& operator+=(const Matrix2& m);
	Matrix2& operator-=(const Matrix2& m);
	Matrix2 operator*=(const Matrix2& m);
	Matrix2& operator*=(float s);

	friend MATH_API Vector2 operator*(const Vector2& v, const Matrix2& m);
	friend MATH_API Matrix2 operator*(float s, const Matrix2& m);
	friend MATH_API Vector2& operator*=(Vector2& v, const Matrix2& m);

	// Unary operators
	Matrix2 operator+() const;
	Matrix2 operator-() const;


	// Mutable individual getters
	float& m0();
	float& m1();
	float& m2();
	float& m3();

	// Non-mutable individual getters
	const float& m0() const;
	const float& m1() const;
	const float& m2() const;
	const float& m3() const;


	// Mutable bracket operators
	float& operator[](m0_enum);
	float& operator[](m1_enum);
	float& operator[](m2_enum);
	float& operator[](m3_enum);

	// Non-mutable bracket operators
	const float& operator[](m0_enum) const;
	const float& operator[](m1_enum) const;
	const float& operator[](m2_enum) const;
	const float& operator[](m3_enum) const;


private:
	Vector2 v0;
	Vector2 v1;
};

