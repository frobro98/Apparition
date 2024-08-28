// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Matrix4.hpp"

TEST(Vect_mult_matrix, vector_tests)
{
	Vector4 vA(2.0f, 3.0f, 4.0f, -2.0f);
	Vector4 vOut;

	CHECK_FLTEQ(vA.x, 2.0f);
	CHECK_FLTEQ(vA.y, 3.0f);
	CHECK_FLTEQ(vA.z, 4.0f);
	CHECK_FLTEQ(vA.w, -2.0f);

	Vector4 V0(1.0f, 2.0f, 3.0f, 4.0f);
	Vector4 V1(7.0f, 6.0f, 5.0f, 3.0f);
	Vector4 V2(-4.0f, -2.0f, -1.0f, -4.0f);
	Vector4 V3(9.0f, -7.0f, -2.0f, 5.0f);

	Matrix4 M(V0, V1, V2, V3);

	CHECK_FLTEQ(M[m0], 1.0f);
	CHECK_FLTEQ(M[m1], 2.0f);
	CHECK_FLTEQ(M[m2], 3.0f);
	CHECK_FLTEQ(M[m3], 4.0f);
	CHECK_FLTEQ(M[m4], 7.0f);
	CHECK_FLTEQ(M[m5], 6.0f);
	CHECK_FLTEQ(M[m6], 5.0f);
	CHECK_FLTEQ(M[m7], 3.0f);
	CHECK_FLTEQ(M[m8], -4.0f);
	CHECK_FLTEQ(M[m9], -2.0f);
	CHECK_FLTEQ(M[m10], -1.0f);
	CHECK_FLTEQ(M[m11], -4.0f);
	CHECK_FLTEQ(M[m12], 9.0f);
	CHECK_FLTEQ(M[m13], -7.0f);
	CHECK_FLTEQ(M[m14], -2.0f);
	CHECK_FLTEQ(M[m15], 5.0f);

	vOut = vA * M;

	CHECK_FLTEQ(M[m0], 1.0f);
	CHECK_FLTEQ(M[m1], 2.0f);
	CHECK_FLTEQ(M[m2], 3.0f);
	CHECK_FLTEQ(M[m3], 4.0f);
	CHECK_FLTEQ(M[m4], 7.0f);
	CHECK_FLTEQ(M[m5], 6.0f);
	CHECK_FLTEQ(M[m6], 5.0f);
	CHECK_FLTEQ(M[m7], 3.0f);
	CHECK_FLTEQ(M[m8], -4.0f);
	CHECK_FLTEQ(M[m9], -2.0f);
	CHECK_FLTEQ(M[m10], -1.0f);
	CHECK_FLTEQ(M[m11], -4.0f);
	CHECK_FLTEQ(M[m12], 9.0f);
	CHECK_FLTEQ(M[m13], -7.0f);
	CHECK_FLTEQ(M[m14], -2.0f);
	CHECK_FLTEQ(M[m15], 5.0f);

	CHECK_FLTEQ(vA.x, 2.0f);
	CHECK_FLTEQ(vA.y, 3.0f);
	CHECK_FLTEQ(vA.z, 4.0f);
	CHECK_FLTEQ(vA.w, -2.0f);

	CHECK_FLTEQ(vOut.x, -11.0f);
	CHECK_FLTEQ(vOut.y, 28.0f);
	CHECK_FLTEQ(vOut.z, 21.0f);
	CHECK_FLTEQ(vOut.w, -9.0f);
}

TEST(Vect_multEqual_matrix, vector_tests)
{
	Vector4 vA(2.0f, 3.0f, 4.0f, -2.0f);

	CHECK_FLTEQ(vA.x, 2.0f);
	CHECK_FLTEQ(vA.y, 3.0f);
	CHECK_FLTEQ(vA.z, 4.0f);
	CHECK_FLTEQ(vA.w, -2.0f);


	Vector4 V0(1.0f, 2.0f, 3.0f, 4.0f);
	Vector4 V1(7.0f, 6.0f, 5.0f, 3.0f);
	Vector4 V2(-4.0f, -2.0f, -1.0f, -4.0f);
	Vector4 V3(9.0f, -7.0f, -2.0f, 5.0f);

	Matrix4 M(V0, V1, V2, V3);

	CHECK_FLTEQ(M[m0], 1.0f);
	CHECK_FLTEQ(M[m1], 2.0f);
	CHECK_FLTEQ(M[m2], 3.0f);
	CHECK_FLTEQ(M[m3], 4.0f);
	CHECK_FLTEQ(M[m4], 7.0f);
	CHECK_FLTEQ(M[m5], 6.0f);
	CHECK_FLTEQ(M[m6], 5.0f);
	CHECK_FLTEQ(M[m7], 3.0f);
	CHECK_FLTEQ(M[m8], -4.0f);
	CHECK_FLTEQ(M[m9], -2.0f);
	CHECK_FLTEQ(M[m10], -1.0f);
	CHECK_FLTEQ(M[m11], -4.0f);
	CHECK_FLTEQ(M[m12], 9.0f);
	CHECK_FLTEQ(M[m13], -7.0f);
	CHECK_FLTEQ(M[m14], -2.0f);
	CHECK_FLTEQ(M[m15], 5.0f);

	vA *= M;

	CHECK_FLTEQ(M[m0], 1.0f);
	CHECK_FLTEQ(M[m1], 2.0f);
	CHECK_FLTEQ(M[m2], 3.0f);
	CHECK_FLTEQ(M[m3], 4.0f);
	CHECK_FLTEQ(M[m4], 7.0f);
	CHECK_FLTEQ(M[m5], 6.0f);
	CHECK_FLTEQ(M[m6], 5.0f);
	CHECK_FLTEQ(M[m7], 3.0f);
	CHECK_FLTEQ(M[m8], -4.0f);
	CHECK_FLTEQ(M[m9], -2.0f);
	CHECK_FLTEQ(M[m10], -1.0f);
	CHECK_FLTEQ(M[m11], -4.0f);
	CHECK_FLTEQ(M[m12], 9.0f);
	CHECK_FLTEQ(M[m13], -7.0f);
	CHECK_FLTEQ(M[m14], -2.0f);
	CHECK_FLTEQ(M[m15], 5.0f);

	CHECK_FLTEQ(vA.x, -11.0f);
	CHECK_FLTEQ(vA.y, 28.0f);
	CHECK_FLTEQ(vA.z, 21.0f);
	CHECK_FLTEQ(vA.w, -9.0f);
}


