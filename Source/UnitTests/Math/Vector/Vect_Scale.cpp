// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Vector4.hpp"

TEST(scale_Vect, vector_tests)
{
	Vector4 A(2.0f, 3.0f, 4.0f, 5.0f);
	float s = 5.0f;
	Vector4 C;

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	C = s * A;

	CHECK_FLTEQ(C.x, 10.0f);
	CHECK_FLTEQ(C.y, 15.0f);
	CHECK_FLTEQ(C.z, 20.0f);
	CHECK_FLTEQ(C.w, 1.0f);

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);
}

TEST(Vect_times_Scale, vector_tests)
{
	Vector4 A(2.0f, 3.0f, 4.0f, 5.0f);
	float s = 5.0f;
	Vector4 C;

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	C = A*s;

	CHECK_FLTEQ(C.x, 10.0f);
	CHECK_FLTEQ(C.y, 15.0f);
	CHECK_FLTEQ(C.z, 20.0f);
	CHECK_FLTEQ(C.w, 1.0f);

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);
}

TEST(Vect_ScaleEquals, vector_tests)
{
	Vector4 A(2.0f, 3.0f, 4.0f, 5.0f);
	float s = 5.0f;

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	A *= s;

	CHECK_FLTEQ(A.x, 10.0f);
	CHECK_FLTEQ(A.y, 15.0f);
	CHECK_FLTEQ(A.z, 20.0f);
	CHECK_FLTEQ(A.w, 1.0f);
}


