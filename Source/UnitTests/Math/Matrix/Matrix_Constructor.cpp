// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Matrix4.hpp"
#include "Math/Quat.hpp"

TEST(Matrix_default_constructor, matix_tests)
{
	Matrix4 M;

	CHECK_FLTEQ(M[m0], 0.0f);
	CHECK_FLTEQ(M[m1], 0.0f);
	CHECK_FLTEQ(M[m2], 0.0f);
	CHECK_FLTEQ(M[m3], 0.0f);
	CHECK_FLTEQ(M[m4], 0.0f);
	CHECK_FLTEQ(M[m5], 0.0f);
	CHECK_FLTEQ(M[m6], 0.0f);	
	CHECK_FLTEQ(M[m7], 0.0f);
	CHECK_FLTEQ(M[m8], 0.0f);
	CHECK_FLTEQ(M[m9], 0.0f);
	CHECK_FLTEQ(M[m10], 0.0f);
	CHECK_FLTEQ(M[m11], 0.0f);
	CHECK_FLTEQ(M[m12], 0.0f);
	CHECK_FLTEQ(M[m13], 0.0f);
	CHECK_FLTEQ(M[m14], 0.0f);
	CHECK_FLTEQ(M[m15], 0.0f);
}

TEST(Matrix_vector_constructor, matix_tests)
{
	Vector4 V0(1.0f,2.0f,3.0f,4.0f);
	Vector4 V1(7.0f,6.0f,5.0f,3.0f);
	Vector4 V2(-4.0f,-2.0f,-1.0f,-4.0f);
	Vector4 V3(9.0f,-7.0f,-2.0f,5.0f);

	CHECK_FLTEQ(V0.x, 1.0f);
	CHECK_FLTEQ(V0.y, 2.0f);
	CHECK_FLTEQ(V0.z, 3.0f);
	CHECK_FLTEQ(V0.w, 4.0f);
	CHECK_FLTEQ(V1.x, 7.0f);
	CHECK_FLTEQ(V1.y, 6.0f);
	CHECK_FLTEQ(V1.z, 5.0f);	
	CHECK_FLTEQ(V1.w, 3.0f);
	CHECK_FLTEQ(V2.x, -4.0f);
	CHECK_FLTEQ(V2.y, -2.0f);
	CHECK_FLTEQ(V2.z, -1.0f);
	CHECK_FLTEQ(V2.w, -4.0f);
	CHECK_FLTEQ(V3.x, 9.0f);
	CHECK_FLTEQ(V3.y, -7.0f);
	CHECK_FLTEQ(V3.z, -2.0f);
	CHECK_FLTEQ(V3.w, 5.0f);

	Matrix4 M(V0,V1,V2,V3);

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
}

TEST(Matrix_copy_constructor, matix_tests)
{
	Vector4 V0(1.0f,2.0f,3.0f,4.0f);
	Vector4 V1(7.0f,6.0f,5.0f,3.0f);
	Vector4 V2(-4.0f,-2.0f,-1.0f,-4.0f);
	Vector4 V3(9.0f,-7.0f,-2.0f,5.0f);

	CHECK_FLTEQ(V0.x, 1.0f);
	CHECK_FLTEQ(V0.y, 2.0f);
	CHECK_FLTEQ(V0.z, 3.0f);
	CHECK_FLTEQ(V0.w, 4.0f);
	CHECK_FLTEQ(V1.x, 7.0f);
	CHECK_FLTEQ(V1.y, 6.0f);
	CHECK_FLTEQ(V1.z, 5.0f);	
	CHECK_FLTEQ(V1.w, 3.0f);
	CHECK_FLTEQ(V2.x, -4.0f);
	CHECK_FLTEQ(V2.y, -2.0f);
	CHECK_FLTEQ(V2.z, -1.0f);
	CHECK_FLTEQ(V2.w, -4.0f);
	CHECK_FLTEQ(V3.x, 9.0f);
	CHECK_FLTEQ(V3.y, -7.0f);
	CHECK_FLTEQ(V3.z, -2.0f);
	CHECK_FLTEQ(V3.w, 5.0f);

	Matrix4 M(V0,V1,V2,V3);

	Matrix4 N(M);

	CHECK_FLTEQ(N[m0], 1.0f);
	CHECK_FLTEQ(N[m1], 2.0f);
	CHECK_FLTEQ(N[m2], 3.0f);
	CHECK_FLTEQ(N[m3], 4.0f);
	CHECK_FLTEQ(N[m4], 7.0f);
	CHECK_FLTEQ(N[m5], 6.0f);
	CHECK_FLTEQ(N[m6], 5.0f);	
	CHECK_FLTEQ(N[m7], 3.0f);
	CHECK_FLTEQ(N[m8], -4.0f);
	CHECK_FLTEQ(N[m9], -2.0f);
	CHECK_FLTEQ(N[m10], -1.0f);
	CHECK_FLTEQ(N[m11], -4.0f);
	CHECK_FLTEQ(N[m12], 9.0f);
	CHECK_FLTEQ(N[m13], -7.0f);
	CHECK_FLTEQ(N[m14], -2.0f);
	CHECK_FLTEQ(N[m15], 5.0f);

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
}

TEST(MatrixRotAxisAngle, matrix_tests)
{
	// Axis and Angle Type Constructor:
	Vector4  v11(2.0f, 53.0f, 24.0f);
	Matrix4 m54(ROT_AXIS_ANGLE, v11, Math::PiOver3);
			
	// => Vector  v11(2.0f, 53.0f, 24.0f); \n"););
	// => Matrix m54(ROT_AXIS_ANGLE, v11, MATH_PI3);\n"););
	CHECK_FLTEQ(m54[m0], 0.5005f);
	CHECK_FLTEQ(m54[m1], 0.3726f);
	CHECK_FLTEQ(m54[m2],-0.7813f);
	CHECK_FLTEQ(m54[m3], 0.0f);
	CHECK_FLTEQ(m54[m4],-0.3413f);
	CHECK_FLTEQ(m54[m5], 0.9144f);
	CHECK_FLTEQ(m54[m6], 0.2174f);
	CHECK_FLTEQ(m54[m7], 0.0f);
	CHECK_FLTEQ(m54[m8], 0.7955f);
	CHECK_FLTEQ(m54[m9], 0.1579f);
	CHECK_FLTEQ(m54[m10], 0.5849f);
	CHECK_FLTEQ(m54[m11], 0.0f);
	CHECK_FLTEQ(m54[m12], 0.0f);
	CHECK_FLTEQ(m54[m13], 0.0f);
	CHECK_FLTEQ(m54[m14], 0.0f);
	CHECK_FLTEQ(m54[m15], 1.0f);
}

TEST(MatrixRotOrient, matrix_tests)
{
	// Orientation Type Constructor:
	Vector4  v15(2.0f, 53.0f, 24.0f);
	Vector4  v16(0.0f, -24.0f, 53.0f);
	Matrix4 m56(ROT_ORIENT, v15, v16);

	CHECK_FLTEQ(m56[m0],-0.9994f);
	CHECK_FLTEQ(m56[m1], 0.0313f);
	CHECK_FLTEQ(m56[m2], 0.0142f);
	CHECK_FLTEQ(m56[m3], 0.0f);
	CHECK_FLTEQ(m56[m4], 0.0000f);
	CHECK_FLTEQ(m56[m5],-0.4125f);
	CHECK_FLTEQ(m56[m6], 0.9110f);
	CHECK_FLTEQ(m56[m7], 0.0f);
	CHECK_FLTEQ(m56[m8], 0.0344f);
	CHECK_FLTEQ(m56[m9], 0.9104f);
	CHECK_FLTEQ(m56[m10], 0.4123f);
	CHECK_FLTEQ(m56[m11], 0.0f);
	CHECK_FLTEQ(m56[m12], 0.0f);
	CHECK_FLTEQ(m56[m13], 0.0f);
	CHECK_FLTEQ(m56[m14], 0.0f);
	CHECK_FLTEQ(m56[m15], 1.0f);
}

TEST(MatrixRotInverseOrient, matrix_tests)
{
	// Orientation Type Constructor:
	Vector4  v17(2.0f, 53.0f, 24.0f);
	Vector4  v18(0.0f, -24.0f, 53.0f);
	Matrix4 m57(ROT_INVERSE_ORIENT, v17, v18);

	CHECK_FLTEQ(m57[m0],-0.9994f);
	CHECK_FLTEQ(m57[m1], 0.0000f);
	CHECK_FLTEQ(m57[m2], 0.0344f);
	CHECK_FLTEQ(m57[m3], 0.0f);
	CHECK_FLTEQ(m57[m4], 0.0313f);
	CHECK_FLTEQ(m57[m5],-0.4125f);
	CHECK_FLTEQ(m57[m6], 0.9104f);
	CHECK_FLTEQ(m57[m7], 0.0f);
	CHECK_FLTEQ(m57[m8], 0.0142f);
	CHECK_FLTEQ(m57[m9], 0.9110f);
	CHECK_FLTEQ(m57[m10], 0.4123f);
	CHECK_FLTEQ(m57[m11], 0.0f);
	CHECK_FLTEQ(m57[m12], 0.0f);
	CHECK_FLTEQ(m57[m13], 0.0f);
	CHECK_FLTEQ(m57[m14], 0.0f);
	CHECK_FLTEQ(m57[m15], 1.0f);
}

TEST(MatrixQuaternion, matrix_tests)
{
	// Quaternion Type Constructor:
	Matrix4 Rxyz1(ROT_XYZ, Math::PiOver3, Math::FivePiOver8, Math::PiOver4);

	Quat	Qxyz1(Rxyz1);
	Matrix4 Mxyz1(Qxyz1);

	CHECK_FLTEQ(Mxyz1[m0],-0.2705f);
	CHECK_FLTEQ(Mxyz1[m1],-0.2705f);
	CHECK_FLTEQ(Mxyz1[m2],-0.9238f);
	CHECK_FLTEQ(Mxyz1[m3], 0.0f);
	CHECK_FLTEQ(Mxyz1[m4], 0.2122f);
	CHECK_FLTEQ(Mxyz1[m5], 0.9193f);
	CHECK_FLTEQ(Mxyz1[m6],-0.3314f);
	CHECK_FLTEQ(Mxyz1[m7], 0.0f);
	CHECK_FLTEQ(Mxyz1[m8], 0.9390f);
	CHECK_FLTEQ(Mxyz1[m9],-0.2857f);
	CHECK_FLTEQ(Mxyz1[m10],-0.1913f);
	CHECK_FLTEQ(Mxyz1[m11], 0.0f);
	CHECK_FLTEQ(Mxyz1[m12], 0.0f);
	CHECK_FLTEQ(Mxyz1[m13], 0.0f);
	CHECK_FLTEQ(Mxyz1[m14], 0.0f);
	CHECK_FLTEQ(Mxyz1[m15], 1.0f);
}

