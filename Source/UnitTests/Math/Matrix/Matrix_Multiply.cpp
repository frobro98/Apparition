// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Matrix4.hpp"

TEST( Matrix_mult_matrix, matix_tests )
{
	Vector4 v0(  1.0f,  2.0f,  3.0f,  4.0f);
	Vector4 v1(  5.0f,  6.0f,  7.0f,  8.0f);
	Vector4 v2(  9.0f, 10.0f, 11.0f, 12.0f);
	Vector4 v3( 13.0f, 14.0f, 15.0f, 16.0f);
	Vector4 v4( 10.0f,  20.0f,  30.0f,  40.0f);
	Vector4 v5(  50.0f,  60.0f,  70.0f,  80.0f);
	Vector4 v6(  90.0f, 100.0f, 110.0f, 120.0f);
	Vector4 v7( 130.0f, 140.0f, 150.0f, 160.0f);

	Matrix4 A(v0,v1,v2,v3);
	Matrix4 B(v4,v5,v6,v7);
	Matrix4 C;

	CHECK_FLTEQ(A[m0],  1.0f);
	CHECK_FLTEQ(A[m1],  2.0f);
	CHECK_FLTEQ(A[m2],  3.0f);
	CHECK_FLTEQ(A[m3],  4.0f);
	CHECK_FLTEQ(A[m4],  5.0f);
	CHECK_FLTEQ(A[m5],  6.0f);
	CHECK_FLTEQ(A[m6],  7.0f);
	CHECK_FLTEQ(A[m7],  8.0f);
	CHECK_FLTEQ(A[m8],  9.0f);
	CHECK_FLTEQ(A[m9], 10.0f);
	CHECK_FLTEQ(A[m10], 11.0f);
	CHECK_FLTEQ(A[m11], 12.0f);
	CHECK_FLTEQ(A[m12], 13.0f);
	CHECK_FLTEQ(A[m13], 14.0f);
	CHECK_FLTEQ(A[m14], 15.0f);
	CHECK_FLTEQ(A[m15], 16.0f);

	CHECK_FLTEQ(B[m0],  10.0f);
	CHECK_FLTEQ(B[m1],  20.0f);
	CHECK_FLTEQ(B[m2],  30.0f);
	CHECK_FLTEQ(B[m3],  40.0f);
	CHECK_FLTEQ(B[m4],  50.0f);
	CHECK_FLTEQ(B[m5],  60.0f);
	CHECK_FLTEQ(B[m6],  70.0f);
	CHECK_FLTEQ(B[m7],  80.0f);
	CHECK_FLTEQ(B[m8],  90.0f);
	CHECK_FLTEQ(B[m9], 100.0f);
	CHECK_FLTEQ(B[m10], 110.0f);
	CHECK_FLTEQ(B[m11], 120.0f);
	CHECK_FLTEQ(B[m12], 130.0f);
	CHECK_FLTEQ(B[m13], 140.0f);
	CHECK_FLTEQ(B[m14], 150.0f);
	CHECK_FLTEQ(B[m15], 160.0f);

	C = A * B;

	CHECK_FLTEQ(A[m0],  1.0f);
	CHECK_FLTEQ(A[m1],  2.0f);
	CHECK_FLTEQ(A[m2],  3.0f);
	CHECK_FLTEQ(A[m3],  4.0f);
	CHECK_FLTEQ(A[m4],  5.0f);
	CHECK_FLTEQ(A[m5],  6.0f);
	CHECK_FLTEQ(A[m6],  7.0f);
	CHECK_FLTEQ(A[m7],  8.0f);
	CHECK_FLTEQ(A[m8],  9.0f);
	CHECK_FLTEQ(A[m9], 10.0f);
	CHECK_FLTEQ(A[m10], 11.0f);
	CHECK_FLTEQ(A[m11], 12.0f);
	CHECK_FLTEQ(A[m12], 13.0f);
	CHECK_FLTEQ(A[m13], 14.0f);
	CHECK_FLTEQ(A[m14], 15.0f);
	CHECK_FLTEQ(A[m15], 16.0f);

	CHECK_FLTEQ(B[m0],  10.0f);
	CHECK_FLTEQ(B[m1],  20.0f);
	CHECK_FLTEQ(B[m2],  30.0f);
	CHECK_FLTEQ(B[m3],  40.0f);
	CHECK_FLTEQ(B[m4],  50.0f);
	CHECK_FLTEQ(B[m5],  60.0f);
	CHECK_FLTEQ(B[m6],  70.0f);
	CHECK_FLTEQ(B[m7],  80.0f);
	CHECK_FLTEQ(B[m8],  90.0f);
	CHECK_FLTEQ(B[m9], 100.0f);
	CHECK_FLTEQ(B[m10], 110.0f);
	CHECK_FLTEQ(B[m11], 120.0f);
	CHECK_FLTEQ(B[m12], 130.0f);
	CHECK_FLTEQ(B[m13], 140.0f);
	CHECK_FLTEQ(B[m14], 150.0f);
	CHECK_FLTEQ(B[m15], 160.0f);

	CHECK_FLTEQ(C[m0],  900.0f);
	CHECK_FLTEQ(C[m1], 1000.0f);
	CHECK_FLTEQ(C[m2], 1100.0f);
	CHECK_FLTEQ(C[m3], 1200.0f);
	CHECK_FLTEQ(C[m4], 2020.0f);
	CHECK_FLTEQ(C[m5], 2280.0f);
	CHECK_FLTEQ(C[m6], 2540.0f);
	CHECK_FLTEQ(C[m7], 2800.0f);
	CHECK_FLTEQ(C[m8], 3140.0f);
	CHECK_FLTEQ(C[m9], 3560.0f);
	CHECK_FLTEQ(C[m10], 3980.0f);
	CHECK_FLTEQ(C[m11], 4400.0f);
	CHECK_FLTEQ(C[m12], 4260.0f);
	CHECK_FLTEQ(C[m13], 4840.0f);
	CHECK_FLTEQ(C[m14], 5420.0f);
	CHECK_FLTEQ(C[m15], 6000.0f);
}

TEST( Matrix_multEqual_matrix, matix_tests )
{
	Vector4 v0(  1.0f,  2.0f,  3.0f,  4.0f);
	Vector4 v1(  5.0f,  6.0f,  7.0f,  8.0f);
	Vector4 v2(  9.0f, 10.0f, 11.0f, 12.0f);
	Vector4 v3( 13.0f, 14.0f, 15.0f, 16.0f);
	Vector4 v4( 10.0f,  20.0f,  30.0f,  40.0f);
	Vector4 v5(  50.0f,  60.0f,  70.0f,  80.0f);
	Vector4 v6(  90.0f, 100.0f, 110.0f, 120.0f);
	Vector4 v7( 130.0f, 140.0f, 150.0f, 160.0f);

	Matrix4 A(v0,v1,v2,v3);
	Matrix4 B(v4,v5,v6,v7);
	Matrix4 C;

	CHECK_FLTEQ(A[m0],  1.0f);
	CHECK_FLTEQ(A[m1],  2.0f);
	CHECK_FLTEQ(A[m2],  3.0f);
	CHECK_FLTEQ(A[m3],  4.0f);
	CHECK_FLTEQ(A[m4],  5.0f);
	CHECK_FLTEQ(A[m5],  6.0f);
	CHECK_FLTEQ(A[m6],  7.0f);
	CHECK_FLTEQ(A[m7],  8.0f);
	CHECK_FLTEQ(A[m8],  9.0f);
	CHECK_FLTEQ(A[m9], 10.0f);
	CHECK_FLTEQ(A[m10], 11.0f);
	CHECK_FLTEQ(A[m11], 12.0f);
	CHECK_FLTEQ(A[m12], 13.0f);
	CHECK_FLTEQ(A[m13], 14.0f);
	CHECK_FLTEQ(A[m14], 15.0f);
	CHECK_FLTEQ(A[m15], 16.0f);

	CHECK_FLTEQ(B[m0],  10.0f);
	CHECK_FLTEQ(B[m1],  20.0f);
	CHECK_FLTEQ(B[m2],  30.0f);
	CHECK_FLTEQ(B[m3],  40.0f);
	CHECK_FLTEQ(B[m4],  50.0f);
	CHECK_FLTEQ(B[m5],  60.0f);
	CHECK_FLTEQ(B[m6],  70.0f);
	CHECK_FLTEQ(B[m7],  80.0f);
	CHECK_FLTEQ(B[m8],  90.0f);
	CHECK_FLTEQ(B[m9], 100.0f);
	CHECK_FLTEQ(B[m10], 110.0f);
	CHECK_FLTEQ(B[m11], 120.0f);
	CHECK_FLTEQ(B[m12], 130.0f);
	CHECK_FLTEQ(B[m13], 140.0f);
	CHECK_FLTEQ(B[m14], 150.0f);
	CHECK_FLTEQ(B[m15], 160.0f);

	A *= B;

	CHECK_FLTEQ(B[m0],  10.0f);
	CHECK_FLTEQ(B[m1],  20.0f);
	CHECK_FLTEQ(B[m2],  30.0f);
	CHECK_FLTEQ(B[m3],  40.0f);
	CHECK_FLTEQ(B[m4],  50.0f);
	CHECK_FLTEQ(B[m5],  60.0f);
	CHECK_FLTEQ(B[m6],  70.0f);
	CHECK_FLTEQ(B[m7],  80.0f);
	CHECK_FLTEQ(B[m8],  90.0f);
	CHECK_FLTEQ(B[m9], 100.0f);
	CHECK_FLTEQ(B[m10], 110.0f);
	CHECK_FLTEQ(B[m11], 120.0f);
	CHECK_FLTEQ(B[m12], 130.0f);
	CHECK_FLTEQ(B[m13], 140.0f);
	CHECK_FLTEQ(B[m14], 150.0f);
	CHECK_FLTEQ(B[m15], 160.0f);

	CHECK_FLTEQ(A[m0],  900.0f);
	CHECK_FLTEQ(A[m1], 1000.0f);
	CHECK_FLTEQ(A[m2], 1100.0f);
	CHECK_FLTEQ(A[m3], 1200.0f);
	CHECK_FLTEQ(A[m4], 2020.0f);
	CHECK_FLTEQ(A[m5], 2280.0f);
	CHECK_FLTEQ(A[m6], 2540.0f);
	CHECK_FLTEQ(A[m7], 2800.0f);
	CHECK_FLTEQ(A[m8], 3140.0f);
	CHECK_FLTEQ(A[m9], 3560.0f);
	CHECK_FLTEQ(A[m10], 3980.0f);
	CHECK_FLTEQ(A[m11], 4400.0f);
	CHECK_FLTEQ(A[m12], 4260.0f);
	CHECK_FLTEQ(A[m13], 4840.0f);
	CHECK_FLTEQ(A[m14], 5420.0f);
	CHECK_FLTEQ(A[m15], 6000.0f);
}


