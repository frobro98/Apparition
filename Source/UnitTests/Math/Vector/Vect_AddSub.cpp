// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Vector4.hpp"

TEST(Vect_Add_Vect, vector_tests)
{
	Vector4 A(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4 B(21.0f, 31.0f, 41.0f, 51.0f);
	Vector4 C;

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);

	C = A + B;

	CHECK_FLTEQ(C.x, 23.0f);
	CHECK_FLTEQ(C.y, 34.0f);
	CHECK_FLTEQ(C.z, 45.0f);
	CHECK_FLTEQ(C.w, 1.0f);

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);
}

TEST(Vect_plusEqual_Vect, vector_tests)
{
	Vector4 A(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4 B(21.0f, 31.0f, 41.0f, 51.0f);

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);

	A += B;

	CHECK_FLTEQ(A.x, 23.0f);
	CHECK_FLTEQ(A.y, 34.0f);
	CHECK_FLTEQ(A.z, 45.0f);
	CHECK_FLTEQ(A.w, 1.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);
}

TEST(Vect_Sub_Vect, vector_tests)
{
	Vector4 A(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4 B(21.0f, 31.0f, 41.0f, 51.0f);
	Vector4 C;

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);

	C = B - A;

	CHECK_FLTEQ(C.x, 19.0f);
	CHECK_FLTEQ(C.y, 28.0f);
	CHECK_FLTEQ(C.z, 37.0f);
	CHECK_FLTEQ(C.w, 1.0f);

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);
}

TEST(Vect_SubEqual_Vect, vector_tests)
{
	Vector4 A(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4 B(21.0f, 31.0f, 41.0f, 51.0f);

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);

	CHECK_FLTEQ(B.x, 21.0f);
	CHECK_FLTEQ(B.y, 31.0f);
	CHECK_FLTEQ(B.z, 41.0f);
	CHECK_FLTEQ(B.w, 51.0f);

	B -= A;

	CHECK_FLTEQ(B.x, 19.0f);
	CHECK_FLTEQ(B.y, 28.0f);
	CHECK_FLTEQ(B.z, 37.0f);
	CHECK_FLTEQ(B.w, 1.0f);

	CHECK_FLTEQ(A.x, 2.0f);
	CHECK_FLTEQ(A.y, 3.0f);
	CHECK_FLTEQ(A.z, 4.0f);
	CHECK_FLTEQ(A.w, 5.0f);
}


