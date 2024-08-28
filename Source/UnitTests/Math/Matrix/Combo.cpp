// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Vector4.hpp"
#include "Math/Matrix4.hpp"
#include "Math/Quat.hpp"
#include "Math/MathFunctions.hpp"

TEST( VectorLine, combo_tests )
{
	Vector4 A(0.0f, 50.0f, 300.0f, 5.0f);
	Vector4 B(10.0f, 100.0f, 1000.0f, 51.0f);
	Vector4 C;

	CHECK_FLTEQ( A.x, 0.0 );
	CHECK_FLTEQ( A.y, 50.0f );
	CHECK_FLTEQ( A.z, 300.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 100.0f );
	CHECK_FLTEQ( B.z, 1000.0f );
	CHECK_FLTEQ( B.w, 51.0f );

	Vector4 Line;
	float t;

	t = 0.0f;
	Line = A + t * ( B - A);

	CHECK_FLTEQ( Line.x, 0.0f );
	CHECK_FLTEQ( Line.y, 50.0f );
	CHECK_FLTEQ( Line.z, 300.0f );
	CHECK_FLTEQ( Line.w, 1.0f );

	CHECK_FLTEQ( A.x, 0.0f );
	CHECK_FLTEQ( A.y, 50.0f );
	CHECK_FLTEQ( A.z, 300.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 100.0f );
	CHECK_FLTEQ( B.z, 1000.0f );
	CHECK_FLTEQ( B.w, 51.0f );

	t = 1.0f;
	Line = A + t * ( B - A);

	CHECK_FLTEQ( Line.x, 10.0f );
	CHECK_FLTEQ( Line.y, 100.0f );
	CHECK_FLTEQ( Line.z, 1000.0f );
	CHECK_FLTEQ( Line.w, 1.0f );

	CHECK_FLTEQ( A.x, 0.0f );
	CHECK_FLTEQ( A.y, 50.0f );
	CHECK_FLTEQ( A.z, 300.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 100.0f );
	CHECK_FLTEQ( B.z, 1000.0f );
	CHECK_FLTEQ( B.w, 51.0f );

	t = 0.5f;
	Line = A + t * ( B - A);

	CHECK_FLTEQ( Line.x, 5.0f );
	CHECK_FLTEQ( Line.y, 75.0f );
	CHECK_FLTEQ( Line.z, 650.0f );
	CHECK_FLTEQ( Line.w, 1.0f );

	CHECK_FLTEQ( A.x, 0.0f );
	CHECK_FLTEQ( A.y, 50.0f );
	CHECK_FLTEQ( A.z, 300.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 100.0f );
	CHECK_FLTEQ( B.z, 1000.0f );
	CHECK_FLTEQ( B.w, 51.0f );
}

TEST( VectorCrossAddSub, combo_tests )
{
	Vector4 A(1.0f, 2.0f, 3.0f, 5.0f);
	Vector4 B(10.0f, 11.0f, 12.0f, 13.0f);
	Vector4 C;

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );

	C = A + (A-B).Cross(B);

	CHECK_FLTEQ( C.x, -8.0f );
	CHECK_FLTEQ( C.y, 20.0f );
	CHECK_FLTEQ( C.z, -6.0f );
	CHECK_FLTEQ( C.w, 1.0f );

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );
}

TEST( VectorCrossAddSubMatrixMult, combo_tests )
{
	Vector4 A(1.0f, 2.0f, 3.0f, 5.0f);
	Vector4 B(10.0f, 11.0f, 12.0f, 13.0f);
	Vector4 C;

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );

	C = A + (A-B).Cross(B);
	Vector4 D = C.Dot(A) * B;

	CHECK_FLTEQ( D.x, 140.0f );
	CHECK_FLTEQ( D.y, 154.0f );
	CHECK_FLTEQ( D.z, 168.0f );
	CHECK_FLTEQ( D.w, 1.0f );

	CHECK_FLTEQ( C.x, -8.0f );
	CHECK_FLTEQ( C.y, 20.0f );
	CHECK_FLTEQ( C.z, -6.0f );
	CHECK_FLTEQ( C.w, 1.0f );

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );

	Matrix4 M(A,B,C,D);

	A = B * M;

	CHECK_FLTEQ( A.x, 1844.0f );
	CHECK_FLTEQ( A.y, 2383.0f );
	CHECK_FLTEQ( A.z, 2274.0f );
	CHECK_FLTEQ( A.w, 218.0f );
}

TEST( VectorCrossAddSubMatrixMultCross, combo_tests )
{
	Vector4 A(1.0f, 2.0f, 3.0f, 5.0f);
	Vector4 B(10.0f, 11.0f, 12.0f, 13.0f);
	Vector4 C;

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );
	
	C = A + (A-B).Cross(B);
	Vector4 D = C.Dot(A) * B;
	CHECK_FLTEQ( D.x, 140.0f );
	CHECK_FLTEQ( D.y, 154.0f );
	CHECK_FLTEQ( D.z, 168.0f );
	CHECK_FLTEQ( D.w, 1.0f );

	CHECK_FLTEQ( C.x, -8.0f );
	CHECK_FLTEQ( C.y, 20.0f );
	CHECK_FLTEQ( C.z, -6.0f );
	CHECK_FLTEQ( C.w, 1.0f );

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );

	Matrix4 M(A,B,C,D);

	A = Vector4(1.0f, 0.0f, 4.0f,1.0f).Cross(B * M);

	CHECK_FLTEQ( A.x, -9532.0f );
	CHECK_FLTEQ( A.y, 5102.0f );
	CHECK_FLTEQ( A.z, 2383.0f );
	CHECK_FLTEQ( A.w, 1.0f );
}

TEST( VectorCrossCrossNormalize, combo_tests )
{
	Vector4 A(1.0f, 2.0f, 3.0f, 5.0f);
	Vector4 B(10.0f, 11.0f, 12.0f, 13.0f);
	Vector4 C;

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );
	
	C = (A-B).Cross(B);
	Vector4 D = C.Cross(B);
	D.Normalize();

	CHECK_FLTEQ(D.x, 0.74790420f);
	CHECK_FLTEQ(D.y, 0.04273730f);
	CHECK_FLTEQ(D.z, -0.6624290f);
	CHECK_FLTEQ( D.w, 1.0f );

	CHECK_FLTEQ( C.x, -9.0f );
	CHECK_FLTEQ( C.y, 18.0f );
	CHECK_FLTEQ( C.z, -9.0f );
	CHECK_FLTEQ( C.w, 1.0f );

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );
}

TEST( junkfood, combo_tests )
{
	Vector4 A(1.0f, 2.0f, 3.0f, 5.0f);
	Vector4 B(10.0f, 11.0f, 12.0f, 13.0f);
	Vector4 C;

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );

	float junk = A.Dot((A-B).Cross(B))+5.0f;

	CHECK_FLTEQ( junk, 5.0f );

	CHECK_FLTEQ( A.x, 1.0f );
	CHECK_FLTEQ( A.y, 2.0f );
	CHECK_FLTEQ( A.z, 3.0f );
	CHECK_FLTEQ( A.w, 5.0f );

	CHECK_FLTEQ( B.x, 10.0f );
	CHECK_FLTEQ( B.y, 11.0f );
	CHECK_FLTEQ( B.z, 12.0f );
	CHECK_FLTEQ( B.w, 13.0f );
}

TEST(ComboRex, combo_tests)
{
	Vector4 v0(1.0f, 2.0f, 3.0f, 4.0f);
	Vector4 v1(5.0f, 6.0f, 7.0f, 8.0f);
	Vector4 v2(9.0f, 10.0f, 11.0f, 12.0f);
	Vector4 v3(13.0f, 14.0f, 15.0f, 16.0f);
	Vector4 v4(10.0f, 20.0f, 30.0f, 40.0f);
	Vector4 v5(50.0f, 60.0f, 70.0f, 80.0f);
	Vector4 v6(90.0f, 100.0f, 110.0f, 120.0f);
	Vector4 v7(130.0f, 140.0f, 150.0f, 160.0f);

	Matrix4 A(v0, v1, v2, v3);
	Matrix4 B(v4, v5, v6, v7);
	Matrix4 C;
	Matrix4 D;

	CHECK_FLTEQ(A[m0], 1.0f);
	CHECK_FLTEQ(A[m1], 2.0f);
	CHECK_FLTEQ(A[m2], 3.0f);
	CHECK_FLTEQ(A[m3], 4.0f);
	CHECK_FLTEQ(A[m4], 5.0f);
	CHECK_FLTEQ(A[m5], 6.0f);
	CHECK_FLTEQ(A[m6], 7.0f);
	CHECK_FLTEQ(A[m7], 8.0f);
	CHECK_FLTEQ(A[m8], 9.0f);
	CHECK_FLTEQ(A[m9], 10.0f);
	CHECK_FLTEQ(A[m10], 11.0f);
	CHECK_FLTEQ(A[m11], 12.0f);
	CHECK_FLTEQ(A[m12], 13.0f);
	CHECK_FLTEQ(A[m13], 14.0f);
	CHECK_FLTEQ(A[m14], 15.0f);
	CHECK_FLTEQ(A[m15], 16.0f);

	CHECK_FLTEQ(B[m0], 10.0f);
	CHECK_FLTEQ(B[m1], 20.0f);
	CHECK_FLTEQ(B[m2], 30.0f);
	CHECK_FLTEQ(B[m3], 40.0f);
	CHECK_FLTEQ(B[m4], 50.0f);
	CHECK_FLTEQ(B[m5], 60.0f);
	CHECK_FLTEQ(B[m6], 70.0f);
	CHECK_FLTEQ(B[m7], 80.0f);
	CHECK_FLTEQ(B[m8], 90.0f);
	CHECK_FLTEQ(B[m9], 100.0f);
	CHECK_FLTEQ(B[m10], 110.0f);
	CHECK_FLTEQ(B[m11], 120.0f);
	CHECK_FLTEQ(B[m12], 130.0f);
	CHECK_FLTEQ(B[m13], 140.0f);
	CHECK_FLTEQ(B[m14], 150.0f);
	CHECK_FLTEQ(B[m15], 160.0f);

	Quat	Q1;
	Matrix4	M1;

	Q1.Set(ROT_AXIS_ANGLE, Vector4(3, -4, 7), -Math::TwoPiOver3);
	M1.Set(ROT_AXIS_ANGLE, Vector4(3, -4, 7), -Math::TwoPiOver3);

	C = A * Q1 * B;
	D = A * M1 * B;

	CHECK_FLTEQ(C[m0],  D[m0]);
	CHECK_FLTEQ(C[m1],  D[m1]);
	CHECK_FLTEQ(C[m2],  D[m2]);
	CHECK_FLTEQ(C[m3],  D[m3]);
	CHECK_FLTEQ(C[m4],  D[m4]);
	CHECK_FLTEQ(C[m5],  D[m5]);
	CHECK_FLTEQ(C[m6],  D[m6]);
	CHECK_FLTEQ(C[m7],  D[m7]);
	CHECK_FLTEQ(C[m8],  D[m8]);
	CHECK_FLTEQ(C[m9],  D[m9]);
	CHECK_FLTEQ(C[m10], D[m10]);
	CHECK_FLTEQ(C[m11], D[m11]);
	CHECK_FLTEQ(C[m12], D[m12]);
	CHECK_FLTEQ(C[m13], D[m13]);
	CHECK_FLTEQ(C[m14], D[m14]);
	CHECK_FLTEQ(C[m15], D[m15]);
}

TEST(ComboRex2, combo_tests)
{
	Vector4 v0(1.0f, 2.0f, 3.0f, 4.0f);
	Vector4 v1(5.0f, 6.0f, 7.0f, 8.0f);
	Vector4 v2(9.0f, 10.0f, 11.0f, 12.0f);
	Vector4 v3(13.0f, 14.0f, 15.0f, 16.0f);
	Vector4 v4(10.0f, 20.0f, 30.0f, 40.0f);
	Vector4 v5(50.0f, 60.0f, 70.0f, 80.0f);
	Vector4 v6(90.0f, 100.0f, 110.0f, 120.0f);
	Vector4 v7(130.0f, 140.0f, 150.0f, 160.0f);

	Matrix4 A(v0, v1, v2, v3);
	Matrix4 B(v4, v5, v6, v7);
	Matrix4 C;
	Matrix4 D;

	CHECK_FLTEQ(A[m0], 1.0f);
	CHECK_FLTEQ(A[m1], 2.0f);
	CHECK_FLTEQ(A[m2], 3.0f);
	CHECK_FLTEQ(A[m3], 4.0f);
	CHECK_FLTEQ(A[m4], 5.0f);
	CHECK_FLTEQ(A[m5], 6.0f);
	CHECK_FLTEQ(A[m6], 7.0f);
	CHECK_FLTEQ(A[m7], 8.0f);
	CHECK_FLTEQ(A[m8], 9.0f);
	CHECK_FLTEQ(A[m9], 10.0f);
	CHECK_FLTEQ(A[m10], 11.0f);
	CHECK_FLTEQ(A[m11], 12.0f);
	CHECK_FLTEQ(A[m12], 13.0f);
	CHECK_FLTEQ(A[m13], 14.0f);
	CHECK_FLTEQ(A[m14], 15.0f);
	CHECK_FLTEQ(A[m15], 16.0f);

	CHECK_FLTEQ(B[m0], 10.0f);
	CHECK_FLTEQ(B[m1], 20.0f);
	CHECK_FLTEQ(B[m2], 30.0f);
	CHECK_FLTEQ(B[m3], 40.0f);
	CHECK_FLTEQ(B[m4], 50.0f);
	CHECK_FLTEQ(B[m5], 60.0f);
	CHECK_FLTEQ(B[m6], 70.0f);
	CHECK_FLTEQ(B[m7], 80.0f);
	CHECK_FLTEQ(B[m8], 90.0f);
	CHECK_FLTEQ(B[m9], 100.0f);
	CHECK_FLTEQ(B[m10], 110.0f);
	CHECK_FLTEQ(B[m11], 120.0f);
	CHECK_FLTEQ(B[m12], 130.0f);
	CHECK_FLTEQ(B[m13], 140.0f);
	CHECK_FLTEQ(B[m14], 150.0f);
	CHECK_FLTEQ(B[m15], 160.0f);

	Quat	Q1;
	Matrix4	M1;

	Q1.Set(ROT_AXIS_ANGLE, Vector4(3, -4, 7), -Math::TwoPiOver3);
	M1.Set(ROT_AXIS_ANGLE, Vector4(3, -4, 7), -Math::TwoPiOver3);

	C = Q1 * B * A;
	D = M1 * B * A;

	CHECK_FLTEQ(C[m0], D[m0]);
	CHECK_FLTEQ(C[m1], D[m1]);
	CHECK_FLTEQ(C[m2], D[m2]);
	CHECK_FLTEQ(C[m3], D[m3]);
	CHECK_FLTEQ(C[m4], D[m4]);
	CHECK_FLTEQ(C[m5], D[m5]);
	CHECK_FLTEQ(C[m6], D[m6]);
	CHECK_FLTEQ(C[m7], D[m7]);
	CHECK_FLTEQ(C[m8], D[m8]);
	CHECK_FLTEQ(C[m9], D[m9]);
	CHECK_FLTEQ(C[m10], D[m10]);
	CHECK_FLTEQ(C[m11], D[m11]);
	CHECK_FLTEQ(C[m12], D[m12]);
	CHECK_FLTEQ(C[m13], D[m13]);
	CHECK_FLTEQ(C[m14], D[m14]);
	CHECK_FLTEQ(C[m15], D[m15]);
}

