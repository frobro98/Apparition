// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Matrix4.hpp"

TEST(Matrix_Scale, matix_tests )
{
	Vector4 V0(1.0f,2.0f,3.0f,4.0f);
	Vector4 V1(5.0f,6.0f,7.0f,8.0f);
	Vector4 V2(9.0f,10.0f,11.0f,12.0f);
	Vector4 V3(13.0f,14.0f,15.0f,16.0f);

	Matrix4 A(V0,V1,V2,V3);
	Matrix4 C;
	float scale = 10.0f;

	CHECK_FLTEQ(A[m0], 1.0f );
	CHECK_FLTEQ(A[m1], 2.0f );
	CHECK_FLTEQ(A[m2], 3.0f );
	CHECK_FLTEQ(A[m3], 4.0f );
	CHECK_FLTEQ(A[m4], 5.0f );
	CHECK_FLTEQ(A[m5], 6.0f );
	CHECK_FLTEQ(A[m6], 7.0f );	
	CHECK_FLTEQ(A[m7], 8.0f );
	CHECK_FLTEQ(A[m8], 9.0f );
	CHECK_FLTEQ(A[m9], 10.0f );
	CHECK_FLTEQ(A[m10], 11.0f );
	CHECK_FLTEQ(A[m11], 12.0f );
	CHECK_FLTEQ(A[m12], 13.0f );
	CHECK_FLTEQ(A[m13], 14.0f );
	CHECK_FLTEQ(A[m14], 15.0f );
	CHECK_FLTEQ(A[m15], 16.0f );

	C = scale * A;

	CHECK_FLTEQ(A[m0], 1.0f );
	CHECK_FLTEQ(A[m1], 2.0f );
	CHECK_FLTEQ(A[m2], 3.0f );
	CHECK_FLTEQ(A[m3], 4.0f );
	CHECK_FLTEQ(A[m4], 5.0f );
	CHECK_FLTEQ(A[m5], 6.0f );
	CHECK_FLTEQ(A[m6], 7.0f );	
	CHECK_FLTEQ(A[m7], 8.0f );
	CHECK_FLTEQ(A[m8], 9.0f );
	CHECK_FLTEQ(A[m9], 10.0f );
	CHECK_FLTEQ(A[m10], 11.0f );
	CHECK_FLTEQ(A[m11], 12.0f );
	CHECK_FLTEQ(A[m12], 13.0f );
	CHECK_FLTEQ(A[m13], 14.0f );
	CHECK_FLTEQ(A[m14], 15.0f );
	CHECK_FLTEQ(A[m15], 16.0f );

	CHECK_FLTEQ(C[m0], 10.0f );
	CHECK_FLTEQ(C[m1], 20.0f );
	CHECK_FLTEQ(C[m2], 30.0f );
	CHECK_FLTEQ(C[m3], 40.0f );
	CHECK_FLTEQ(C[m4], 50.0f );
	CHECK_FLTEQ(C[m5], 60.0f );
	CHECK_FLTEQ(C[m6], 70.0f );	
	CHECK_FLTEQ(C[m7], 80.0f );
	CHECK_FLTEQ(C[m8], 90.0f );
	CHECK_FLTEQ(C[m9], 100.0f );
	CHECK_FLTEQ(C[m10], 110.0f );
	CHECK_FLTEQ(C[m11], 120.0f );
	CHECK_FLTEQ(C[m12], 130.0f );
	CHECK_FLTEQ(C[m13], 140.0f );
	CHECK_FLTEQ(C[m14], 150.0f );
	CHECK_FLTEQ(C[m15], 160.0f );
}

TEST(Matrix_times_Scale, Matrix_tests )
{
	Vector4 V0(1.0f,2.0f,3.0f,4.0f);
	Vector4 V1(5.0f,6.0f,7.0f,8.0f);
	Vector4 V2(9.0f,10.0f,11.0f,12.0f);
	Vector4 V3(13.0f,14.0f,15.0f,16.0f);

	Matrix4 A(V0,V1,V2,V3);
	Matrix4 C;
	float scale = 10.0f;

	CHECK_FLTEQ(A[m0], 1.0f );
	CHECK_FLTEQ(A[m1], 2.0f );
	CHECK_FLTEQ(A[m2], 3.0f );
	CHECK_FLTEQ(A[m3], 4.0f );
	CHECK_FLTEQ(A[m4], 5.0f );
	CHECK_FLTEQ(A[m5], 6.0f );
	CHECK_FLTEQ(A[m6], 7.0f );	
	CHECK_FLTEQ(A[m7], 8.0f );
	CHECK_FLTEQ(A[m8], 9.0f );
	CHECK_FLTEQ(A[m9], 10.0f );
	CHECK_FLTEQ(A[m10], 11.0f );
	CHECK_FLTEQ(A[m11], 12.0f );
	CHECK_FLTEQ(A[m12], 13.0f );
	CHECK_FLTEQ(A[m13], 14.0f );
	CHECK_FLTEQ(A[m14], 15.0f );
	CHECK_FLTEQ(A[m15], 16.0f );

	C = A * scale;

	CHECK_FLTEQ(A[m0], 1.0f );
	CHECK_FLTEQ(A[m1], 2.0f );
	CHECK_FLTEQ(A[m2], 3.0f );
	CHECK_FLTEQ(A[m3], 4.0f );
	CHECK_FLTEQ(A[m4], 5.0f );
	CHECK_FLTEQ(A[m5], 6.0f );
	CHECK_FLTEQ(A[m6], 7.0f );	
	CHECK_FLTEQ(A[m7], 8.0f );
	CHECK_FLTEQ(A[m8], 9.0f );
	CHECK_FLTEQ(A[m9], 10.0f );
	CHECK_FLTEQ(A[m10], 11.0f );
	CHECK_FLTEQ(A[m11], 12.0f );
	CHECK_FLTEQ(A[m12], 13.0f );
	CHECK_FLTEQ(A[m13], 14.0f );
	CHECK_FLTEQ(A[m14], 15.0f );
	CHECK_FLTEQ(A[m15], 16.0f );

	CHECK_FLTEQ(C[m0], 10.0f );
	CHECK_FLTEQ(C[m1], 20.0f );
	CHECK_FLTEQ(C[m2], 30.0f );
	CHECK_FLTEQ(C[m3], 40.0f );
	CHECK_FLTEQ(C[m4], 50.0f );
	CHECK_FLTEQ(C[m5], 60.0f );
	CHECK_FLTEQ(C[m6], 70.0f );	
	CHECK_FLTEQ(C[m7], 80.0f );
	CHECK_FLTEQ(C[m8], 90.0f );
	CHECK_FLTEQ(C[m9], 100.0f );
	CHECK_FLTEQ(C[m10], 110.0f );
	CHECK_FLTEQ(C[m11], 120.0f );
	CHECK_FLTEQ(C[m12], 130.0f );
	CHECK_FLTEQ(C[m13], 140.0f );
	CHECK_FLTEQ(C[m14], 150.0f );
	CHECK_FLTEQ(C[m15], 160.0f );
}

TEST(Matrix_ScaleEquals, Matrix_tests )
{

	Vector4 V0(1.0f,2.0f,3.0f,4.0f);
	Vector4 V1(5.0f,6.0f,7.0f,8.0f);
	Vector4 V2(9.0f,10.0f,11.0f,12.0f);
	Vector4 V3(13.0f,14.0f,15.0f,16.0f);

	Matrix4 A(V0,V1,V2,V3);
	float scale = 10.0f;

	CHECK_FLTEQ(A[m0], 1.0f );
	CHECK_FLTEQ(A[m1], 2.0f );
	CHECK_FLTEQ(A[m2], 3.0f );
	CHECK_FLTEQ(A[m3], 4.0f );
	CHECK_FLTEQ(A[m4], 5.0f );
	CHECK_FLTEQ(A[m5], 6.0f );
	CHECK_FLTEQ(A[m6], 7.0f );	
	CHECK_FLTEQ(A[m7], 8.0f );
	CHECK_FLTEQ(A[m8], 9.0f );
	CHECK_FLTEQ(A[m9], 10.0f );
	CHECK_FLTEQ(A[m10], 11.0f );
	CHECK_FLTEQ(A[m11], 12.0f );
	CHECK_FLTEQ(A[m12], 13.0f );
	CHECK_FLTEQ(A[m13], 14.0f );
	CHECK_FLTEQ(A[m14], 15.0f );
	CHECK_FLTEQ(A[m15], 16.0f );

	A *= scale;

	CHECK_FLTEQ(A[m0], 10.0f );
	CHECK_FLTEQ(A[m1], 20.0f );
	CHECK_FLTEQ(A[m2], 30.0f );
	CHECK_FLTEQ(A[m3], 40.0f );
	CHECK_FLTEQ(A[m4], 50.0f );
	CHECK_FLTEQ(A[m5], 60.0f );
	CHECK_FLTEQ(A[m6], 70.0f );	
	CHECK_FLTEQ(A[m7], 80.0f );
	CHECK_FLTEQ(A[m8], 90.0f );
	CHECK_FLTEQ(A[m9], 100.0f );
	CHECK_FLTEQ(A[m10], 110.0f );
	CHECK_FLTEQ(A[m11], 120.0f );
	CHECK_FLTEQ(A[m12], 130.0f );
	CHECK_FLTEQ(A[m13], 140.0f );
	CHECK_FLTEQ(A[m14], 150.0f );
	CHECK_FLTEQ(A[m15], 160.0f );
}


