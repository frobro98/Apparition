// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Vector4.hpp"
#include "Math/VectorFunctions.hpp"

TEST(Dot, vector_tests)
{
	Vector4 v1(1.0f, 2.0f, 3.0f);
	Vector4 v2(4.0f, 5.0f, 6.0f);

	float value1;

	value1 = v1.Dot(v2);
	CHECK_FLTEQ(value1, 32.0f);
}

TEST(Cross, vector_tests)
{
	Vector4 v1(1.0f, 0.0f, 0.0f);
	Vector4 v2(0.0f, 1.0f, 0.0f);
	Vector4 out1;

	out1 = v1.Cross(v2);

	CHECK_FLTEQ(out1.x, 0.0f);
	CHECK_FLTEQ(out1.y, 0.0f);
	CHECK_FLTEQ(out1.z, 1.0f);
	CHECK_FLTEQ(out1.w, 1.0f);
}

TEST(Normalizealize, vector_tests)
{
	Vector4 v1(1.0f, 2.0f, 3.0f);

	v1.Normalize();

	CHECK_FLTEQ(v1.x, 0.2672f);
	CHECK_FLTEQ(v1.y, 0.5345f);
	CHECK_FLTEQ(v1.z, 0.80174f);
	CHECK_FLTEQ(v1.w, 1.0f);
}

TEST(GetNormalize, vector_tests)
{
	Vector4 v1(1.0f, 2.0f, 3.0f);
	Vector4 v2;

	v2 = v1.GetNormalized();

	CHECK_FLTEQ(v2.x, 0.2672f);
	CHECK_FLTEQ(v2.y, 0.5345f);
	CHECK_FLTEQ(v2.z, .80174f);
	CHECK_FLTEQ(v2.w, 1.0f);
	CHECK_FLTEQ(v1.x, 1.0f);
	CHECK_FLTEQ(v1.y, 2.0f);
	CHECK_FLTEQ(v1.z, 3.0f);
	CHECK_FLTEQ(v1.w, 1.0f);
}

TEST(Magnitude, VectFunctionTests)
{
	Vector4 v1(1.0f, 2.0f, 3.0f);
	float value;

	value = v1.Magnitude();

	CHECK_FLTEQ(value, 3.7416f);
}

TEST(MagnitudeSquared, vector_tests)
{
	Vector4 v1(1.0f, 2.0f, 3.0f);
	float value;
	value = v1.MagnitudeSqr();
	CHECK_FLTEQ(value, 14);
}

TEST(GetAngle, vector_tests)
{
	float value;
	Vector4 v1(1.0f, 2.0f, 3.0f);
	Vector4 v2(1.80f, -2.0f, 2.0f);

	value = Math::GetRadiansBetween(v1, v2);

	CHECK_FLTEQ(value, 1.263f);

	value = Math::GetDegreesBetween(v1, v2);

	CHECK_FLTEQ(value, 72.3665f);
}


