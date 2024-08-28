// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Quat.hpp"

TEST(Quat_Default_Constructor, QuatConstructorTests )
{
	// Default Constructor:
	Quat q1;

	CHECK_FLTEQ(0.0f, q1.x );
	CHECK_FLTEQ(0.0f, q1.y );
	CHECK_FLTEQ(0.0f, q1.z );
	CHECK_FLTEQ(1.0f, q1.w );
}

TEST(Quat_Constructor_Set_Every_Component, QuatConstructorTests )
{
	// Set every element constructor
	Quat q10(2.0f,3.0f,4.0f,5.0f);

	CHECK_FLTEQ(2.0f, q10.x );
	CHECK_FLTEQ(3.0f, q10.y );
	CHECK_FLTEQ(4.0f, q10.z );
	CHECK_FLTEQ(5.0f, q10.w );
}

TEST(Quat_Copy_Constructor, QuatConstructorTests )
{
	// Copy Constructor
	Quat q11(2.0f,3.0f,4.0f,5.0f);
	Quat q12(q11);

	CHECK_FLTEQ(2.0f, q11.x );
	CHECK_FLTEQ(3.0f, q11.y );
	CHECK_FLTEQ(4.0f, q11.z );
	CHECK_FLTEQ(5.0f, q11.w );
	CHECK_FLTEQ(q12.x, q11.x );
	CHECK_FLTEQ(q12.y, q11.y );
	CHECK_FLTEQ(q12.z, q11.z );
	CHECK_FLTEQ(q12.w, q11.w );
}

TEST(Quat_Vect_Component_Real_Component_Constructor, QuatConstructorTests )
{
	// Vector component, Real component Constructor
	Vector4 v10(12.0f,13.0f,14.0f,15.0f);
	Quat q13(v10,20.0f);

	CHECK_FLTEQ(12.0f, q13.x );
	CHECK_FLTEQ(13.0f, q13.y );
	CHECK_FLTEQ(14.0f, q13.z );
	CHECK_FLTEQ(20.0f, q13.w );
}

TEST(Quat_From_Matrix_Constructor, QuatConstructorTests )
{
	// Quaternion from a Matrix Constructor
	Matrix4 m40;
	Matrix4 Rx(ROT_X,Math::PiOver2);
	Matrix4 Ry(ROT_Y,-Math::PiOver3);
	Matrix4 Rz(ROT_Z,-Math::SevenPiOver8);

	m40 = Rx*Ry*Rz;

	Quat q14(m40);
	
	CHECK_FLTEQ(0.2273f,q14.x);
	CHECK_FLTEQ(0.6695f,q14.y);
	CHECK_FLTEQ(0.5316f,q14.z);
	CHECK_FLTEQ(-0.4662f,q14.w);

	Matrix4 M3;
	M3.Set(q14);

	CHECK_FLTEQ(m40[m0],M3[m0]);
	CHECK_FLTEQ(m40[m1],M3[m1]);
	CHECK_FLTEQ(m40[m2],M3[m2]);
	CHECK_FLTEQ(m40[m3],M3[m3]);
	CHECK_FLTEQ(m40[m4],M3[m4]);
	CHECK_FLTEQ(m40[m5],M3[m5]);
	CHECK_FLTEQ(m40[m6],M3[m6]);
	CHECK_FLTEQ(m40[m7],M3[m7]);
	CHECK_FLTEQ(m40[m8],M3[m8]);
	CHECK_FLTEQ(m40[m9],M3[m9]);
	CHECK_FLTEQ(m40[m10],M3[m10]);
	CHECK_FLTEQ(m40[m11],M3[m11]);
	CHECK_FLTEQ(m40[m12],M3[m12]);
	CHECK_FLTEQ(m40[m13],M3[m13]);
	CHECK_FLTEQ(m40[m14],M3[m14]);
	CHECK_FLTEQ(m40[m15],M3[m15]);
}

TEST(Quat_IDENTITY_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:
	Quat q2 = Quat::Identity;

	CHECK_FLTEQ(q2.x, 0.0f);
	CHECK_FLTEQ(q2.y, 0.0f);
	CHECK_FLTEQ(q2.z, 0.0f);
	CHECK_FLTEQ(q2.w, 1.0f);

	Matrix4 	M2;
	M2.Set(IDENTITY);
	Quat qtmp;
	qtmp.Set(M2 );
	CHECK_FLTEQ(q2.x,qtmp.x);
	CHECK_FLTEQ(q2.y,qtmp.y);
	CHECK_FLTEQ(q2.z,qtmp.z);
	CHECK_FLTEQ(q2.w,qtmp.w);
	
	Matrix4 M3;
	M3.Set(q2);
	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_ROTXYZ_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:
	Quat qxyz(ROT_XYZ, 0.345f, -1.234f, 0.832f);

   Quat qx(ROT_X, 0.345f);
   Quat qy(ROT_Y, -1.234f);
   Quat qz(ROT_Z,  0.832f);

   Quat qtmp;

   qtmp = qx * qy * qz;

	CHECK_FLTEQ(qxyz.x,qtmp.x);
	CHECK_FLTEQ(qxyz.y,qtmp.y);
	CHECK_FLTEQ(qxyz.z,qtmp.z);
	CHECK_FLTEQ(qxyz.w,qtmp.w);
}

TEST(Quat_SET_ROTXYZ_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:
	Quat qxyz;
   
   qxyz.Set(ROT_XYZ, 0.345f, -1.234f, 0.832f);

   Quat qx(ROT_X, 0.345f);
   Quat qy(ROT_Y, -1.234f);
   Quat qz(ROT_Z,  0.832f);

   Quat qtmp;

   qtmp = qx * qy * qz;

	CHECK_FLTEQ(qxyz.x,qtmp.x);
	CHECK_FLTEQ(qxyz.y,qtmp.y);
	CHECK_FLTEQ(qxyz.z,qtmp.z);
	CHECK_FLTEQ(qxyz.w,qtmp.w);
}

TEST(Quat_X_PI2_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:
	Quat q4(ROT_X, Math::PiOver2);

	// Matrix to Quaternion
	Matrix4 M2;
	M2.Set(ROT_X, Math::PiOver2);

	Quat qtmp;
	qtmp.Set(M2);
	CHECK_FLTEQ(q4.x,qtmp.x);
	CHECK_FLTEQ(q4.y,qtmp.y);
	CHECK_FLTEQ(q4.z,qtmp.z);
	CHECK_FLTEQ(q4.w,qtmp.w);

	// Quaternion to Matrix
	Matrix4 M3;
	M3.Set(q4);
	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_X_PI_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:
	Quat q5(ROT_X, Math::Pi);

	// Matrix to Quaternion
	Matrix4 M2, M3;
	Quat qtmp;
	M2.Set(ROT_X, Math::Pi);
	qtmp.Set(M2 );

	CHECK_FLTEQ(q5.x,qtmp.x);
	CHECK_FLTEQ(q5.y,qtmp.y);
	CHECK_FLTEQ(q5.z,qtmp.z);
	CHECK_FLTEQ(q5.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(q5);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_Y_PI2_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:

	Quat q6(ROT_Y, Math::PiOver2);

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;
	M2.Set(ROT_Y, Math::PiOver2);
	qtmp.Set(M2 );
	CHECK_FLTEQ(q6.x,qtmp.x);
	CHECK_FLTEQ(q6.y,qtmp.y);
	CHECK_FLTEQ(q6.z,qtmp.z);
	CHECK_FLTEQ(q6.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(q6);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_Y_PI_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:
	Quat q7(ROT_Y, Math::Pi);

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;
	M2.Set(ROT_Y, Math::Pi);
	qtmp.Set(M2 );
	CHECK_FLTEQ(q7.x,qtmp.x);
	CHECK_FLTEQ(q7.y,qtmp.y);
	CHECK_FLTEQ(q7.z,qtmp.z);
	CHECK_FLTEQ(q7.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(q7);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_Z_PI2_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:
	Quat q8(ROT_Z, Math::PiOver2);

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;
	M2.Set(ROT_Z, Math::PiOver2);
	qtmp.Set(M2 );
	CHECK_FLTEQ(q8.x,qtmp.x);
	CHECK_FLTEQ(q8.y,qtmp.y);
	CHECK_FLTEQ(q8.z,qtmp.z);
	CHECK_FLTEQ(q8.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(q8);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_Z_PI_Constructor, QuatConstructorTests )
{
	// Special Type Constructor:

	Quat q9(ROT_Z,Math::Pi);

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;
	M2.Set(ROT_Z,Math::Pi);
	qtmp.Set(M2 );
	CHECK_FLTEQ(q9.x,qtmp.x);
	CHECK_FLTEQ(q9.y,qtmp.y);
	CHECK_FLTEQ(q9.z,qtmp.z);
	CHECK_FLTEQ(q9.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(q9);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_X_Constructor, QuatConstructorTests )
{
	// Rot_X Type Constructor:
	Quat Qx(ROT_X, Math::PiOver3);	

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;
	M2.Set(ROT_X, Math::PiOver3);
	qtmp.Set(M2 );
	CHECK_FLTEQ(Qx.x,qtmp.x);
	CHECK_FLTEQ(Qx.y,qtmp.y);
	CHECK_FLTEQ(Qx.z,qtmp.z);
	CHECK_FLTEQ(Qx.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qx);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_Y_Constructor, QuatConstructorTests )
{
	// Rot_Y Type Constructor:
	Quat Qy(ROT_Y, Math::PiOver3);


	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;
	M2.Set(ROT_Y, Math::PiOver3);
	qtmp.Set(M2 );
	CHECK_FLTEQ(Qy.x,qtmp.x);
	CHECK_FLTEQ(Qy.y,qtmp.y);
	CHECK_FLTEQ(Qy.z,qtmp.z);
	CHECK_FLTEQ(Qy.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qy);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_Z_Constructor, QuatConstructorTests )
{
	// Rot_Z Type Constructor:
	Quat Qz(ROT_Z, Math::PiOver3);

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;
	M2.Set(ROT_Z, Math::PiOver3);
	qtmp.Set(M2 );
	CHECK_FLTEQ(Qz.x,qtmp.x);
	CHECK_FLTEQ(Qz.y,qtmp.y);
	CHECK_FLTEQ(Qz.z,qtmp.z);
	CHECK_FLTEQ(Qz.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qz);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_XY_Constructor, QuatConstructorTests )
{
	// Rot_XY Type Constructor:
	Quat Qx(ROT_X, Math::PiOver3);
	Quat Qy(ROT_Y, Math::PiOver4);

	Quat Qxy;
	Qxy = Qx * Qy;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 Mx(ROT_X, Math::PiOver3);
	Matrix4 My(ROT_Y, Math::PiOver4);
	M2 = Mx * My;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qxy.x,qtmp.x);
	CHECK_FLTEQ(Qxy.y,qtmp.y);
	CHECK_FLTEQ(Qxy.z,qtmp.z);
	CHECK_FLTEQ(Qxy.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qxy);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_XZ_Constructor, QuatConstructorTests )
{
	// Rot_XZ Type Constructor:
	Quat Qx(ROT_X, Math::PiOver3);
	Quat Qz(ROT_Z, Math::PiOver4);
	Quat Qxz;
	
	Qxz = Qx * Qz;

	// Matrix to Quaternion
	Matrix4 M2, M3;
	Quat qtmp;
	Matrix4 Mx(ROT_X, Math::PiOver3);
	Matrix4 Mz(ROT_Z, Math::PiOver4);
	M2 = Mx * Mz;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qxz.x,qtmp.x);
	CHECK_FLTEQ(Qxz.y,qtmp.y);
	CHECK_FLTEQ(Qxz.z,qtmp.z);
	CHECK_FLTEQ(Qxz.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qxz);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_YX_Constructor, QuatConstructorTests )
{
	// Rot_YX Type Constructor:
	Quat Qy(ROT_Y, Math::PiOver3);
	Quat Qx(ROT_X, Math::PiOver4);
	Quat Qyx = Qy * Qx;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 My(ROT_Y, Math::PiOver3);
	Matrix4 Mx(ROT_X, Math::PiOver4);
	M2 = My * Mx;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qyx.x,qtmp.x);
	CHECK_FLTEQ(Qyx.y,qtmp.y);
	CHECK_FLTEQ(Qyx.z,qtmp.z);
	CHECK_FLTEQ(Qyx.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qyx);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_YZ_Constructor, QuatConstructorTests )
{
	// Rot_YZ Type Constructor:
	Quat Qy(ROT_Y, Math::PiOver3);
	Quat Qz(ROT_Z, Math::PiOver4);
	Quat Qyz = Qy * Qz;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 My(ROT_Y, Math::PiOver3);
	Matrix4 Mz(ROT_Z, Math::PiOver4);	
	M2 = My * Mz;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qyz.x,qtmp.x);
	CHECK_FLTEQ(Qyz.y,qtmp.y);
	CHECK_FLTEQ(Qyz.z,qtmp.z);
	CHECK_FLTEQ(Qyz.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qyz);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_ZX_Constructor, QuatConstructorTests )
{
	// Rot_ZX Type Constructor:
	Quat Qz(ROT_Z, Math::PiOver3);
	Quat Qx(ROT_X, Math::PiOver4);
	Quat Qzx =Qz * Qx;

	 // Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 Mz(ROT_Z, Math::PiOver3);
	Matrix4 Mx(ROT_X, Math::PiOver4);	
	M2 = Mz * Mx;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qzx.x,qtmp.x);
	CHECK_FLTEQ(Qzx.y,qtmp.y);
	CHECK_FLTEQ(Qzx.z,qtmp.z);
	CHECK_FLTEQ(Qzx.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qzx);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_ZY_Constructor, QuatConstructorTests )
{
	// Rot_ZY Type Constructor:
	Quat Qz(ROT_Z, Math::PiOver3);
	Quat Qy(ROT_Y, Math::PiOver4);
	Quat Qzy =Qz * Qy;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 Mz(ROT_Z, Math::PiOver3);
	Matrix4 My(ROT_Y, Math::PiOver4);	
	M2 = Mz * My;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qzy.x,qtmp.x);
	CHECK_FLTEQ(Qzy.y,qtmp.y);
	CHECK_FLTEQ(Qzy.z,qtmp.z);
	CHECK_FLTEQ(Qzy.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qzy);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_XYZ_Constructor, QuatConstructorTests )
{
	// Rot_XYZ Type Constructor:
	Quat Qx(ROT_X, Math::PiOver3 );
	Quat Qy(ROT_Y, Math::FivePiOver8);
	Quat Qz(ROT_Z, Math::PiOver4 );
	Quat Qxyz = Qx * Qy * Qz;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 Mx(ROT_X, Math::PiOver3);
	Matrix4 My(ROT_Y,  Math::FivePiOver8);
	Matrix4 Mz(ROT_Z,  Math::PiOver4 );
	M2 = Mx * My * Mz;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qxyz.x,qtmp.x);
	CHECK_FLTEQ(Qxyz.y,qtmp.y);
	CHECK_FLTEQ(Qxyz.z,qtmp.z);
	CHECK_FLTEQ(Qxyz.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qxyz);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_XZY_Constructor, QuatConstructorTests )
{
	// Rot_XZY Type Constructor:
	Quat Qx(ROT_X, Math::PiOver3 );
	Quat Qz(ROT_Z, Math::FivePiOver8);
	Quat Qy(ROT_Y, Math::PiOver4 );
	Quat Qxzy = Qx * Qz * Qy;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 Mx(ROT_X, Math::PiOver3);
	Matrix4 Mz(ROT_Z,  Math::FivePiOver8);
	Matrix4 My(ROT_Y,  Math::PiOver4 );
	M2 = Mx * Mz * My;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qxzy.x,qtmp.x);
	CHECK_FLTEQ(Qxzy.y,qtmp.y);
	CHECK_FLTEQ(Qxzy.z,qtmp.z);
	CHECK_FLTEQ(Qxzy.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qxzy);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_YXZ_Constructor, QuatConstructorTests )
{
	// Rot_YXZ Type Constructor:
	Quat Qy(ROT_Y, Math::PiOver3 );
	Quat Qx(ROT_X, Math::FivePiOver8);
	Quat Qz(ROT_Z, Math::PiOver4 );
	Quat Qyxz = Qy * Qx * Qz;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 My(ROT_Y, Math::PiOver3);
	Matrix4 Mx(ROT_X,  Math::FivePiOver8);
	Matrix4 Mz(ROT_Z,  Math::PiOver4 );
	M2 = My * Mx * Mz;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qyxz.x,qtmp.x);
	CHECK_FLTEQ(Qyxz.y,qtmp.y);
	CHECK_FLTEQ(Qyxz.z,qtmp.z);
	CHECK_FLTEQ(Qyxz.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qyxz);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_YZX_Constructor, QuatConstructorTests )
{
	// Rot_YZX Type Constructor:

	Quat Qy(ROT_Y, Math::PiOver3 );
	Quat Qz(ROT_Z, Math::FivePiOver8);
	Quat Qx(ROT_X, Math::PiOver4 );
	Quat Qyzx = Qy * Qz * Qx;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 My(ROT_Y, Math::PiOver3);
	Matrix4 Mz(ROT_Z,  Math::FivePiOver8);
	Matrix4 Mx(ROT_X,  Math::PiOver4 );
	M2 = My * Mz * Mx;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qyzx.x,qtmp.x);
	CHECK_FLTEQ(Qyzx.y,qtmp.y);
	CHECK_FLTEQ(Qyzx.z,qtmp.z);
	CHECK_FLTEQ(Qyzx.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qyzx);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_ZXY_Constructor, QuatConstructorTests )
{
	// Rot_ZXY Type Constructor:
	Quat Qz(ROT_Z, Math::PiOver3 );
	Quat Qx(ROT_X, Math::FivePiOver8);
	Quat Qy(ROT_Y, Math::PiOver4 );
	Quat Qzxy = Qz * Qx * Qy;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 Mz(ROT_Z, Math::PiOver3);
	Matrix4 Mx(ROT_X,  Math::FivePiOver8);
	Matrix4 My(ROT_Y,  Math::PiOver4 );
	M2 = Mz * Mx * My;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qzxy.x,qtmp.x);
	CHECK_FLTEQ(Qzxy.y,qtmp.y);
	CHECK_FLTEQ(Qzxy.z,qtmp.z);
	CHECK_FLTEQ(Qzxy.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qzxy);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_ZYX_Constructor, QuatConstructorTests )
{
	// Rot_ZYX Type Constructor:
	Quat Qz(ROT_Z, Math::PiOver3 );
	Quat Qy(ROT_Y, Math::FivePiOver8);
	Quat Qx(ROT_X, Math::PiOver4 );
	Quat Qzyx = Qz * Qy * Qx;

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	Matrix4 Mz(ROT_Z, Math::PiOver3);
	Matrix4 My(ROT_Y,  Math::FivePiOver8);
	Matrix4 Mx(ROT_X,  Math::PiOver4 );
	M2 = Mz * My * Mx;

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qzyx.x,qtmp.x);
	CHECK_FLTEQ(Qzyx.y,qtmp.y);
	CHECK_FLTEQ(Qzyx.z,qtmp.z);
	CHECK_FLTEQ(Qzyx.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qzyx);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_AXIS_ANGLE_Constructor, QuatConstructorTests )
{
	// Axis and Angle Type Constructor:
	Vector4  v11(2.0f, 53.0f, 24.0f);
	Quat Qa1(ROT_AXIS_ANGLE, v11, Math::PiOver3 );

	// Matrix to Quaternion
	Matrix4 M2,M3;
	Quat qtmp;

	M2.Set(ROT_AXIS_ANGLE, v11, Math::PiOver3 );

	qtmp.Set(M2 );
	CHECK_FLTEQ(Qa1.x,qtmp.x);
	CHECK_FLTEQ(Qa1.y,qtmp.y);
	CHECK_FLTEQ(Qa1.z,qtmp.z);
	CHECK_FLTEQ(Qa1.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(Qa1);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_ROT_ORIENT_Constructor, QuatConstructorTests )
{
	// Orientation Type Constructor:

	Vector4  v15(2.0f, 53.0f, 24.0f);
	Vector4  v16(0.0f, -24.0f, 53.0f);
	Quat q56(ROT_ORIENT, v15, v16 );

	Matrix4 M2,M3;
	Quat qtmp;

	M2.Set(ROT_ORIENT, v15, v16 );
	qtmp.Set(M2 );
	CHECK_FLTEQ(q56.x,qtmp.x);
	CHECK_FLTEQ(q56.y,qtmp.y);
	CHECK_FLTEQ(q56.z,qtmp.z);
	CHECK_FLTEQ(q56.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(q56);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}

TEST(Quat_ROT_INVERSE_ORIENT_Constructor, QuatConstructorTests )
{
	// Orientation Type Constructor:
	Vector4  v17(2.0f, 53.0f, 24.0f);
	Vector4  v18(0.0f, -24.0f, 53.0f);
	Quat q57(ROT_INVERSE_ORIENT, v17, v18 );

	Matrix4 M2,M3;
	Quat qtmp;

	M2.Set(ROT_INVERSE_ORIENT, v17, v18 );
	qtmp.Set(M2 );
	CHECK_FLTEQ(q57.x,qtmp.x);
	CHECK_FLTEQ(q57.y,qtmp.y);
	CHECK_FLTEQ(q57.z,qtmp.z);
	CHECK_FLTEQ(q57.w,qtmp.w);

	// Quaternion to Matrix
	M3.Set(q57);

	CHECK_FLTEQ(M2[m0],M3[m0]);
	CHECK_FLTEQ(M2[m1],M3[m1]);
	CHECK_FLTEQ(M2[m2],M3[m2]);
	CHECK_FLTEQ(M2[m3],M3[m3]);
	CHECK_FLTEQ(M2[m4],M3[m4]);
	CHECK_FLTEQ(M2[m5],M3[m5]);
	CHECK_FLTEQ(M2[m6],M3[m6]);
	CHECK_FLTEQ(M2[m7],M3[m7]);
	CHECK_FLTEQ(M2[m8],M3[m8]);
	CHECK_FLTEQ(M2[m9],M3[m9]);
	CHECK_FLTEQ(M2[m10],M3[m10]);
	CHECK_FLTEQ(M2[m11],M3[m11]);
	CHECK_FLTEQ(M2[m12],M3[m12]);
	CHECK_FLTEQ(M2[m13],M3[m13]);
	CHECK_FLTEQ(M2[m14],M3[m14]);
	CHECK_FLTEQ(M2[m15],M3[m15]);
}


