// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Vector4.hpp"

TEST(unary_Plus, vector_tests)
{
	Vector4 A(-2.0f, -3.0f, -4.0f, -5.0f);

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);

	+A;

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);
}

TEST(unary_equal_Plus, vector_tests)
{
	Vector4 A(-2.0f, -3.0f, -4.0f, -5.0f);
	Vector4 B(21.0f, 31.0f, 41.0f, 51.0f);

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);

	B = +A;

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);

	CHECK_FLTEQ(B.x, -2.0f);
	CHECK_FLTEQ(B.y, -3.0f);
	CHECK_FLTEQ(B.z, -4.0f);
	CHECK_FLTEQ(B.w, 1.0f);
}

TEST(unary_Minus, vector_tests)
{
	Vector4 A(-2.0f, -3.0f, -4.0f, -5.0f);

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);

	-A;

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);
}

TEST(unary_equal_Minus, vector_tests)
{
	Vector4 A(-2.0f, -3.0f, -4.0f, -5.0f);
	Vector4 B(21.0f, 31.0f, 41.0f, 51.0f);

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);

	B = -A;

	CHECK_FLTEQ(A.x, -2.0f);
	CHECK_FLTEQ(A.y, -3.0f);
	CHECK_FLTEQ(A.z, -4.0f);
	CHECK_FLTEQ(A.w, -5.0f);

	CHECK_FLTEQ(B.x, 2.0f);
	CHECK_FLTEQ(B.y, 3.0f);
	CHECK_FLTEQ(B.z, 4.0f);
	CHECK_FLTEQ(B.w, 1.0f);
}


