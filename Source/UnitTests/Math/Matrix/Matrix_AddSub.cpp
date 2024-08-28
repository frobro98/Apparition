// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Matrix4.hpp"


TEST( Matrix_add_matrix, matix_tests )
{
	Vector4 V0(1.0f,2.0f,3.0f,4.0f);
	Vector4 V1(5.0f,6.0f,7.0f,8.0f);
	Vector4 V2(9.0f,10.0f,11.0f,12.0f);
	Vector4 V3(13.0f,14.0f,15.0f,16.0f);

	Matrix4 A(V0,V1,V2,V3);

	CHECK_FLTEQ( A[m0], 1.0f );
	CHECK_FLTEQ( A[m1], 2.0f );
	CHECK_FLTEQ( A[m2], 3.0f );
	CHECK_FLTEQ( A[m3], 4.0f );
	CHECK_FLTEQ( A[m4], 5.0f );
	CHECK_FLTEQ( A[m5], 6.0f );
	CHECK_FLTEQ( A[m6], 7.0f );	
	CHECK_FLTEQ( A[m7], 8.0f );
	CHECK_FLTEQ( A[m8], 9.0f );
	CHECK_FLTEQ( A[m9], 10.0f );
	CHECK_FLTEQ( A[m10], 11.0f );
	CHECK_FLTEQ( A[m11], 12.0f );
	CHECK_FLTEQ( A[m12], 13.0f );
	CHECK_FLTEQ( A[m13], 14.0f );
	CHECK_FLTEQ( A[m14], 15.0f );
	CHECK_FLTEQ( A[m15], 16.0f );

	Vector4 V4(20.0f,21.0f,22.0f,23.0f);
	Vector4 V5(24.0f,25.0f,26.0f,27.0f);
	Vector4 V6(28.0f,29.0f,30.0f,31.0f);
	Vector4 V7(32.0f,33.0f,34.0f,35.0f);

	Matrix4 B(V4,V5,V6,V7);

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	Matrix4 C;

	C = A + B;

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	CHECK_FLTEQ( A[m0], 1.0f );
	CHECK_FLTEQ( A[m1], 2.0f );
	CHECK_FLTEQ( A[m2], 3.0f );
	CHECK_FLTEQ( A[m3], 4.0f );
	CHECK_FLTEQ( A[m4], 5.0f );
	CHECK_FLTEQ( A[m5], 6.0f );
	CHECK_FLTEQ( A[m6], 7.0f );	
	CHECK_FLTEQ( A[m7], 8.0f );
	CHECK_FLTEQ( A[m8], 9.0f );
	CHECK_FLTEQ( A[m9], 10.0f );
	CHECK_FLTEQ( A[m10], 11.0f );
	CHECK_FLTEQ( A[m11], 12.0f );
	CHECK_FLTEQ( A[m12], 13.0f );
	CHECK_FLTEQ( A[m13], 14.0f );
	CHECK_FLTEQ( A[m14], 15.0f );
	CHECK_FLTEQ( A[m15], 16.0f );

	CHECK_FLTEQ( C[m0], 21.0f );
	CHECK_FLTEQ( C[m1], 23.0f );
	CHECK_FLTEQ( C[m2], 25.0f );
	CHECK_FLTEQ( C[m3], 27.0f );
	CHECK_FLTEQ( C[m4], 29.0f );
	CHECK_FLTEQ( C[m5], 31.0f );
	CHECK_FLTEQ( C[m6], 33.0f );	
	CHECK_FLTEQ( C[m7], 35.0f );
	CHECK_FLTEQ( C[m8], 37.0f );
	CHECK_FLTEQ( C[m9], 39.0f );
	CHECK_FLTEQ( C[m10], 41.0f );
	CHECK_FLTEQ( C[m11], 43.0f );
	CHECK_FLTEQ( C[m12], 45.0f );
	CHECK_FLTEQ( C[m13], 47.0f );
	CHECK_FLTEQ( C[m14], 49.0f );
	CHECK_FLTEQ( C[m15], 51.0f );
}

TEST( Matrix_addEqual_matrix, matix_tests )
{
	Vector4 V0(1.0f,2.0f,3.0f,4.0f);
	Vector4 V1(5.0f,6.0f,7.0f,8.0f);
	Vector4 V2(9.0f,10.0f,11.0f,12.0f);
	Vector4 V3(13.0f,14.0f,15.0f,16.0f);

	Matrix4 A(V0,V1,V2,V3);

	CHECK_FLTEQ( A[m0], 1.0f );
	CHECK_FLTEQ( A[m1], 2.0f );
	CHECK_FLTEQ( A[m2], 3.0f );
	CHECK_FLTEQ( A[m3], 4.0f );
	CHECK_FLTEQ( A[m4], 5.0f );
	CHECK_FLTEQ( A[m5], 6.0f );
	CHECK_FLTEQ( A[m6], 7.0f );	
	CHECK_FLTEQ( A[m7], 8.0f );
	CHECK_FLTEQ( A[m8], 9.0f );
	CHECK_FLTEQ( A[m9], 10.0f );
	CHECK_FLTEQ( A[m10], 11.0f );
	CHECK_FLTEQ( A[m11], 12.0f );
	CHECK_FLTEQ( A[m12], 13.0f );
	CHECK_FLTEQ( A[m13], 14.0f );
	CHECK_FLTEQ( A[m14], 15.0f );
	CHECK_FLTEQ( A[m15], 16.0f );

	Vector4 V4(20.0f,21.0f,22.0f,23.0f);
	Vector4 V5(24.0f,25.0f,26.0f,27.0f);
	Vector4 V6(28.0f,29.0f,30.0f,31.0f);
	Vector4 V7(32.0f,33.0f,34.0f,35.0f);

	Matrix4 B(V4,V5,V6,V7);

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	Matrix4 C;

	A += B;

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	CHECK_FLTEQ( A[m0], 21.0f );
	CHECK_FLTEQ( A[m1], 23.0f );
	CHECK_FLTEQ( A[m2], 25.0f );
	CHECK_FLTEQ( A[m3], 27.0f );
	CHECK_FLTEQ( A[m4], 29.0f );
	CHECK_FLTEQ( A[m5], 31.0f );
	CHECK_FLTEQ( A[m6], 33.0f );	
	CHECK_FLTEQ( A[m7], 35.0f );
	CHECK_FLTEQ( A[m8], 37.0f );
	CHECK_FLTEQ( A[m9], 39.0f );
	CHECK_FLTEQ( A[m10], 41.0f );
	CHECK_FLTEQ( A[m11], 43.0f );
	CHECK_FLTEQ( A[m12], 45.0f );
	CHECK_FLTEQ( A[m13], 47.0f );
	CHECK_FLTEQ( A[m14], 49.0f );
	CHECK_FLTEQ( A[m15], 51.0f );
}

TEST( Matrix_sub_matrix, matix_tests )
{
	Vector4 V0(30.0f,29.0f,28.0f,27.0f);
	Vector4 V1(26.0f,25.0f,24.0f,23.0f);
	Vector4 V2(22.0f,21.0f,20.0f,19.0f);
	Vector4 V3(18.0f,17.0f,16.0f,15.0f);

	Matrix4 A(V0,V1,V2,V3);

	CHECK_FLTEQ( A[m0], 30.0f );
	CHECK_FLTEQ( A[m1], 29.0f );
	CHECK_FLTEQ( A[m2], 28.0f );
	CHECK_FLTEQ( A[m3], 27.0f );
	CHECK_FLTEQ( A[m4], 26.0f );
	CHECK_FLTEQ( A[m5], 25.0f );
	CHECK_FLTEQ( A[m6], 24.0f );	
	CHECK_FLTEQ( A[m7], 23.0f );
	CHECK_FLTEQ( A[m8], 22.0f );
	CHECK_FLTEQ( A[m9], 21.0f );
	CHECK_FLTEQ( A[m10], 20.0f );
	CHECK_FLTEQ( A[m11], 19.0f );
	CHECK_FLTEQ( A[m12], 18.0f );
	CHECK_FLTEQ( A[m13], 17.0f );
	CHECK_FLTEQ( A[m14], 16.0f );
	CHECK_FLTEQ( A[m15], 15.0f );

	Vector4 V4(20.0f,21.0f,22.0f,23.0f);
	Vector4 V5(24.0f,25.0f,26.0f,27.0f);
	Vector4 V6(28.0f,29.0f,30.0f,31.0f);
	Vector4 V7(32.0f,33.0f,34.0f,35.0f);

	Matrix4 B(V4,V5,V6,V7);

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	Matrix4 C;

	C = A - B;

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	CHECK_FLTEQ( A[m0], 30.0f );
	CHECK_FLTEQ( A[m1], 29.0f );
	CHECK_FLTEQ( A[m2], 28.0f );
	CHECK_FLTEQ( A[m3], 27.0f );
	CHECK_FLTEQ( A[m4], 26.0f );
	CHECK_FLTEQ( A[m5], 25.0f );
	CHECK_FLTEQ( A[m6], 24.0f );	
	CHECK_FLTEQ( A[m7], 23.0f );
	CHECK_FLTEQ( A[m8], 22.0f );
	CHECK_FLTEQ( A[m9], 21.0f );
	CHECK_FLTEQ( A[m10], 20.0f );
	CHECK_FLTEQ( A[m11], 19.0f );
	CHECK_FLTEQ( A[m12], 18.0f );
	CHECK_FLTEQ( A[m13], 17.0f );
	CHECK_FLTEQ( A[m14], 16.0f );
	CHECK_FLTEQ( A[m15], 15.0f );

	CHECK_FLTEQ( C[m0], 10.0f );
	CHECK_FLTEQ( C[m1], 8.0f );
	CHECK_FLTEQ( C[m2], 6.0f );
	CHECK_FLTEQ( C[m3], 4.0f );
	CHECK_FLTEQ( C[m4], 2.0f );
	CHECK_FLTEQ( C[m5], 0.0f );
	CHECK_FLTEQ( C[m6], -2.0f );	
	CHECK_FLTEQ( C[m7], -4.0f );
	CHECK_FLTEQ( C[m8], -6.0f );
	CHECK_FLTEQ( C[m9], -8.0f );
	CHECK_FLTEQ( C[m10], -10.0f );
	CHECK_FLTEQ( C[m11], -12.0f );
	CHECK_FLTEQ( C[m12], -14.0f );
	CHECK_FLTEQ( C[m13], -16.0f );
	CHECK_FLTEQ( C[m14], -18.0f );
	CHECK_FLTEQ( C[m15], -20.0f );
}

TEST( Matrix_subEqual_matrix, matix_tests )
{
	Vector4 V0(30.0f,29.0f,28.0f,27.0f);
	Vector4 V1(26.0f,25.0f,24.0f,23.0f);
	Vector4 V2(22.0f,21.0f,20.0f,19.0f);
	Vector4 V3(18.0f,17.0f,16.0f,15.0f);

	Matrix4 A(V0,V1,V2,V3);

	CHECK_FLTEQ( A[m0], 30.0f );
	CHECK_FLTEQ( A[m1], 29.0f );
	CHECK_FLTEQ( A[m2], 28.0f );
	CHECK_FLTEQ( A[m3], 27.0f );
	CHECK_FLTEQ( A[m4], 26.0f );
	CHECK_FLTEQ( A[m5], 25.0f );
	CHECK_FLTEQ( A[m6], 24.0f );	
	CHECK_FLTEQ( A[m7], 23.0f );
	CHECK_FLTEQ( A[m8], 22.0f );
	CHECK_FLTEQ( A[m9], 21.0f );
	CHECK_FLTEQ( A[m10], 20.0f );
	CHECK_FLTEQ( A[m11], 19.0f );
	CHECK_FLTEQ( A[m12], 18.0f );
	CHECK_FLTEQ( A[m13], 17.0f );
	CHECK_FLTEQ( A[m14], 16.0f );
	CHECK_FLTEQ( A[m15], 15.0f );

	Vector4 V4(20.0f,21.0f,22.0f,23.0f);
	Vector4 V5(24.0f,25.0f,26.0f,27.0f);
	Vector4 V6(28.0f,29.0f,30.0f,31.0f);
	Vector4 V7(32.0f,33.0f,34.0f,35.0f);

	Matrix4 B(V4,V5,V6,V7);

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	Matrix4 C;

	A -= B;

	CHECK_FLTEQ( B[m0], 20.0f );
	CHECK_FLTEQ( B[m1], 21.0f );
	CHECK_FLTEQ( B[m2], 22.0f );
	CHECK_FLTEQ( B[m3], 23.0f );
	CHECK_FLTEQ( B[m4], 24.0f );
	CHECK_FLTEQ( B[m5], 25.0f );
	CHECK_FLTEQ( B[m6], 26.0f );	
	CHECK_FLTEQ( B[m7], 27.0f );
	CHECK_FLTEQ( B[m8], 28.0f );
	CHECK_FLTEQ( B[m9], 29.0f );
	CHECK_FLTEQ( B[m10], 30.0f );
	CHECK_FLTEQ( B[m11], 31.0f );
	CHECK_FLTEQ( B[m12], 32.0f );
	CHECK_FLTEQ( B[m13], 33.0f );
	CHECK_FLTEQ( B[m14], 34.0f );
	CHECK_FLTEQ( B[m15], 35.0f );

	CHECK_FLTEQ( A[m0], 10.0f );
	CHECK_FLTEQ( A[m1], 8.0f );
	CHECK_FLTEQ( A[m2], 6.0f );
	CHECK_FLTEQ( A[m3], 4.0f );
	CHECK_FLTEQ( A[m4], 2.0f );
	CHECK_FLTEQ( A[m5], 0.0f );
	CHECK_FLTEQ( A[m6], -2.0f );	
	CHECK_FLTEQ( A[m7], -4.0f );
	CHECK_FLTEQ( A[m8], -6.0f );
	CHECK_FLTEQ( A[m9], -8.0f );
	CHECK_FLTEQ( A[m10], -10.0f );
	CHECK_FLTEQ( A[m11], -12.0f );
	CHECK_FLTEQ( A[m12], -14.0f );
	CHECK_FLTEQ( A[m13], -16.0f );
	CHECK_FLTEQ( A[m14], -18.0f );
	CHECK_FLTEQ( A[m15], -20.0f );
}
