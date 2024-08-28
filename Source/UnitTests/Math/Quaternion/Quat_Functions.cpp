// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Matrix4.hpp"
#include "Math/Quat.hpp"

TEST(getConjugate, QuatFunctionsTests)
{
	Matrix4	mm2, mm3;
	Quat	q1, q2, q3, qtmp;
	Vector4	v1, v2, v3, v4;

	// Conjugate

	//q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Quat qx(ROT_X, Math::TwoPiOver3);
	Quat qy(ROT_Y, Math::ThreePiOver8);
	Quat qz(ROT_Z, Math::ThreePiOver4);

	// => q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4
	q1 = qx * qy*qz;

	CHECK_FLTEQ(q1.x, 0.01887f);
	CHECK_FLTEQ(q1.y, 0.7715f);
	CHECK_FLTEQ(q1.z, 0.1999f);
	CHECK_FLTEQ(q1.w, 0.6036f);

	q2 = q1.GetConjugate();
	// => q2 = q1.getConj();\n"););

	CHECK_FLTEQ(q2.x, -0.01887f);
	CHECK_FLTEQ(q2.y, -0.7715f);
	CHECK_FLTEQ(q2.z, -0.1999f);
	CHECK_FLTEQ(q2.w, 0.6036f);


	// Matrix to Quat
	//m2.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Matrix4 mx(ROT_X, Math::TwoPiOver3);
	Matrix4 my(ROT_Y, Math::ThreePiOver8);
	Matrix4 mz(ROT_Z, Math::ThreePiOver4);

	mm2 = mx * my * mz;

	qtmp.Set(mm2);

	CHECK_FLTEQ(q1.x, qtmp.x);
	CHECK_FLTEQ(q1.y, qtmp.y);
	CHECK_FLTEQ(q1.z, qtmp.z);
	CHECK_FLTEQ(q1.w, qtmp.w);

	// Quaternion to Matrix
	mm3.Set(q1);

	CHECK_FLTEQ(mm2[m0], mm3[m0]);
	CHECK_FLTEQ(mm2[m1], mm3[m1]);
	CHECK_FLTEQ(mm2[m2], mm3[m2]);
	CHECK_FLTEQ(mm2[m3], mm3[m3]);
	CHECK_FLTEQ(mm2[m4], mm3[m4]);
	CHECK_FLTEQ(mm2[m5], mm3[m5]);
	CHECK_FLTEQ(mm2[m6], mm3[m6]);
	CHECK_FLTEQ(mm2[m7], mm3[m7]);
	CHECK_FLTEQ(mm2[m8], mm3[m8]);
	CHECK_FLTEQ(mm2[m9], mm3[m9]);
	CHECK_FLTEQ(mm2[m10], mm3[m10]);
	CHECK_FLTEQ(mm2[m11], mm3[m11]);
	CHECK_FLTEQ(mm2[m12], mm3[m12]);
	CHECK_FLTEQ(mm2[m13], mm3[m13]);
	CHECK_FLTEQ(mm2[m14], mm3[m14]);
	CHECK_FLTEQ(mm2[m15], mm3[m15]);
}

TEST(QuatConjugateInPlace, QuatFunctionsTests)
{
	Quat q1;
	
	// Conjugate in place 

	//q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	// => q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Quat qx(ROT_X, Math::TwoPiOver3);
	Quat qy(ROT_Y, Math::ThreePiOver8);
	Quat qz(ROT_Z, Math::ThreePiOver4);

	// => q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4
	q1 = qx*qy*qz;
	
	CHECK_FLTEQ(q1.x,0.01887f);
	CHECK_FLTEQ(q1.y,0.7715f);
	CHECK_FLTEQ(q1.z,0.1999f);
	CHECK_FLTEQ(q1.w,0.6036f);
	
	q1.Conjugate();
	// => q1.conj()

	CHECK_FLTEQ(q1.x,-0.01887f);
	CHECK_FLTEQ(q1.y,-0.7715f);
	CHECK_FLTEQ(q1.z,-0.1999f);
	CHECK_FLTEQ(q1.w,0.6036f);
}

TEST(QuatConjugateByValue, QuatFunctionsTests)
{
	Quat q1,q2,qtmp;
	Matrix4 mm2, mm3;
	
	// Conjugate by value
	// q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Quat qx(ROT_X, Math::TwoPiOver3);
	Quat qy(ROT_Y, Math::ThreePiOver8);
	Quat qz(ROT_Z, Math::ThreePiOver4);

	// => q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4
	q1 = qx*qy*qz;
	
	CHECK_FLTEQ(q1.x,0.01887f);
	CHECK_FLTEQ(q1.y,0.7715f);
	CHECK_FLTEQ(q1.z,0.1999f);
	CHECK_FLTEQ(q1.w,0.6036f);
	
	q2 = q1.GetConjugate();
	// => q2 = q1.getConj();
	
	CHECK_FLTEQ(q2.x,-0.01887f);
	CHECK_FLTEQ(q2.y,-0.7715f);
	CHECK_FLTEQ(q2.z,-0.1999f);
	CHECK_FLTEQ(q2.w,0.6036f);

	// Matrix to Quat
	//m2.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);

	Matrix4 mx(ROT_X, Math::TwoPiOver3);
	Matrix4 my(ROT_Y, Math::ThreePiOver8);
	Matrix4 mz(ROT_Z, Math::ThreePiOver4);

	mm2 = mx * my * mz;
 
	qtmp.Set(mm2);
	
	CHECK_FLTEQ(q1.x,qtmp.x);
	CHECK_FLTEQ(q1.y,qtmp.y);
	CHECK_FLTEQ(q1.z,qtmp.z);
	CHECK_FLTEQ(q1.w,qtmp.w);

	// Quaternion to Matrix
	mm3.Set(q1);
	CHECK_FLTEQ(mm2[m0],mm3[m0]);
	CHECK_FLTEQ(mm2[m1],mm3[m1]);
	CHECK_FLTEQ(mm2[m2],mm3[m2]);
	CHECK_FLTEQ(mm2[m3],mm3[m3]);
	CHECK_FLTEQ(mm2[m4],mm3[m4]);
	CHECK_FLTEQ(mm2[m5],mm3[m5]);
	CHECK_FLTEQ(mm2[m6],mm3[m6]);
	CHECK_FLTEQ(mm2[m7],mm3[m7]);
	CHECK_FLTEQ(mm2[m8],mm3[m8]);
	CHECK_FLTEQ(mm2[m9],mm3[m9]);
	CHECK_FLTEQ(mm2[m10],mm3[m10]);
	CHECK_FLTEQ(mm2[m11],mm3[m11]);
	CHECK_FLTEQ(mm2[m12],mm3[m12]);
	CHECK_FLTEQ(mm2[m13],mm3[m13]);
	CHECK_FLTEQ(mm2[m14],mm3[m14]);
	CHECK_FLTEQ(mm2[m15],mm3[m15]);
}

TEST(QuatTransposeToDest, QuatFunctionsTests)
{
	// transpose
	// transpose to destination
	Quat q1,q2,q3,qtmp;
	//q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);

	Quat qx(ROT_X, Math::TwoPiOver3);
	Quat qy(ROT_Y, Math::ThreePiOver8);
	Quat qz(ROT_Z, Math::ThreePiOver4);

	// => q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4
	q1 = qx*qy*qz;

	CHECK_FLTEQ(q1.x,0.01887f);
	CHECK_FLTEQ(q1.y,0.7715f);
	CHECK_FLTEQ(q1.z,0.1999f);
	CHECK_FLTEQ(q1.w,0.6036f);
	
	q2 = q1.GetTranspose();

	// => q2 = q1.GetTranspose()
	
	CHECK_FLTEQ(q2.x,-0.01887f);
	CHECK_FLTEQ(q2.y,-0.7715f);
	CHECK_FLTEQ(q2.z,-0.1999f);
	CHECK_FLTEQ(q2.w,0.6036f);

	// Matrix to Quat
	Matrix4 mm2, mm3;
	//mm2.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Matrix4 mx(ROT_X, Math::TwoPiOver3);
	Matrix4 my(ROT_Y, Math::ThreePiOver8);
	Matrix4 mz(ROT_Z, Math::ThreePiOver4);

	mm2 = mx * my * mz;

	qtmp.Set(mm2);
	CHECK_FLTEQ(q1.x,qtmp.x);
	CHECK_FLTEQ(q1.y,qtmp.y);
	CHECK_FLTEQ(q1.z,qtmp.z);
	CHECK_FLTEQ(q1.w,qtmp.w);

	// Quaternion to Matrix
	mm3.Set(q1);

	CHECK_FLTEQ(mm2[m0],mm3[m0]);
	CHECK_FLTEQ(mm2[m1],mm3[m1]);
	CHECK_FLTEQ(mm2[m2],mm3[m2]);
	CHECK_FLTEQ(mm2[m3],mm3[m3]);
	CHECK_FLTEQ(mm2[m4],mm3[m4]);
	CHECK_FLTEQ(mm2[m5],mm3[m5]);
	CHECK_FLTEQ(mm2[m6],mm3[m6]);
	CHECK_FLTEQ(mm2[m7],mm3[m7]);
	CHECK_FLTEQ(mm2[m8],mm3[m8]);
	CHECK_FLTEQ(mm2[m9],mm3[m9]);
	CHECK_FLTEQ(mm2[m10],mm3[m10]);
	CHECK_FLTEQ(mm2[m11],mm3[m11]);
	CHECK_FLTEQ(mm2[m12],mm3[m12]);
	CHECK_FLTEQ(mm2[m13],mm3[m13]);
	CHECK_FLTEQ(mm2[m14],mm3[m14]);
	CHECK_FLTEQ(mm2[m15],mm3[m15]);

	// Matrix to Quat
	mm2.Transpose();
	qtmp.Set(mm2);
	CHECK_FLTEQ(q2.x,qtmp.x);
	CHECK_FLTEQ(q2.y,qtmp.y);
	CHECK_FLTEQ(q2.z,qtmp.z);
	CHECK_FLTEQ(q2.w,qtmp.w);

	// Quaternion to Matrix
	mm3.Set(q2);

	CHECK_FLTEQ(mm2[m0],mm3[m0]);
	CHECK_FLTEQ(mm2[m1],mm3[m1]);
	CHECK_FLTEQ(mm2[m2],mm3[m2]);
	CHECK_FLTEQ(mm2[m3],mm3[m3]);
	CHECK_FLTEQ(mm2[m4],mm3[m4]);
	CHECK_FLTEQ(mm2[m5],mm3[m5]);
	CHECK_FLTEQ(mm2[m6],mm3[m6]);
	CHECK_FLTEQ(mm2[m7],mm3[m7]);
	CHECK_FLTEQ(mm2[m8],mm3[m8]);
	CHECK_FLTEQ(mm2[m9],mm3[m9]);
	CHECK_FLTEQ(mm2[m10],mm3[m10]);
	CHECK_FLTEQ(mm2[m11],mm3[m11]);
	CHECK_FLTEQ(mm2[m12],mm3[m12]);
	CHECK_FLTEQ(mm2[m13],mm3[m13]);
	CHECK_FLTEQ(mm2[m14],mm3[m14]);
	CHECK_FLTEQ(mm2[m15],mm3[m15]);

	q3 = q2 * q1;
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
	q3 = q1 * q2;
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
}

TEST(QuatTransposeByValue, QuatFunctionsTests)
{
	// "transpose by value
	Quat q1,q2,q3,qtmp;
	Matrix4 mm1,mm2,mm3;
	//q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Quat qx(ROT_X, Math::TwoPiOver3);
	Quat qy(ROT_Y, Math::ThreePiOver8);
	Quat qz(ROT_Z, Math::ThreePiOver4);

	// => q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4
	q1 = qx*qy*qz;

	CHECK_FLTEQ(q1.x,0.01887f);
	CHECK_FLTEQ(q1.y,0.7715f);
	CHECK_FLTEQ(q1.z,0.1999f);
	CHECK_FLTEQ(q1.w,0.6036f);

	q2 = q1.GetTranspose();

	// => q2 = q1.GetTranspose()

	CHECK_FLTEQ(q2.x,-0.01887f);
	CHECK_FLTEQ(q2.y,-0.7715f);
	CHECK_FLTEQ(q2.z,-0.1999f);
	CHECK_FLTEQ(q2.w,0.6036f);

	// Matrix to Quat
	//m2.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Matrix4 mx(ROT_X, Math::TwoPiOver3);
	Matrix4 my(ROT_Y, Math::ThreePiOver8);
	Matrix4 mz(ROT_Z, Math::ThreePiOver4);

	mm2 = mx * my * mz;

	qtmp.Set(mm2);
	CHECK_FLTEQ(q1.x,qtmp.x);
	CHECK_FLTEQ(q1.y,qtmp.y);
	CHECK_FLTEQ(q1.z,qtmp.z);
	CHECK_FLTEQ(q1.w,qtmp.w);

	// Quaternion to Matrix
	mm3.Set(q1);

	CHECK_FLTEQ(mm2[m0],mm3[m0]);
	CHECK_FLTEQ(mm2[m1],mm3[m1]);
	CHECK_FLTEQ(mm2[m2],mm3[m2]);
	CHECK_FLTEQ(mm2[m3],mm3[m3]);
	CHECK_FLTEQ(mm2[m4],mm3[m4]);
	CHECK_FLTEQ(mm2[m5],mm3[m5]);
	CHECK_FLTEQ(mm2[m6],mm3[m6]);
	CHECK_FLTEQ(mm2[m7],mm3[m7]);
	CHECK_FLTEQ(mm2[m8],mm3[m8]);
	CHECK_FLTEQ(mm2[m9],mm3[m9]);
	CHECK_FLTEQ(mm2[m10],mm3[m10]);
	CHECK_FLTEQ(mm2[m11],mm3[m11]);
	CHECK_FLTEQ(mm2[m12],mm3[m12]);
	CHECK_FLTEQ(mm2[m13],mm3[m13]);
	CHECK_FLTEQ(mm2[m14],mm3[m14]);
	CHECK_FLTEQ(mm2[m15],mm3[m15]);

	// Matrix to Quat
	mm2.Transpose();
	qtmp.Set(mm2);
	CHECK_FLTEQ(q2.x,qtmp.x);
	CHECK_FLTEQ(q2.y,qtmp.y);
	CHECK_FLTEQ(q2.z,qtmp.z);
	CHECK_FLTEQ(q2.w,qtmp.w);

	// Quaternion to Matrix
	mm3.Set(q2);

	CHECK_FLTEQ(mm2[m0],mm3[m0]);
	CHECK_FLTEQ(mm2[m1],mm3[m1]);
	CHECK_FLTEQ(mm2[m2],mm3[m2]);
	CHECK_FLTEQ(mm2[m3],mm3[m3]);
	CHECK_FLTEQ(mm2[m4],mm3[m4]);
	CHECK_FLTEQ(mm2[m5],mm3[m5]);
	CHECK_FLTEQ(mm2[m6],mm3[m6]);
	CHECK_FLTEQ(mm2[m7],mm3[m7]);
	CHECK_FLTEQ(mm2[m8],mm3[m8]);
	CHECK_FLTEQ(mm2[m9],mm3[m9]);
	CHECK_FLTEQ(mm2[m10],mm3[m10]);
	CHECK_FLTEQ(mm2[m11],mm3[m11]);
	CHECK_FLTEQ(mm2[m12],mm3[m12]);
	CHECK_FLTEQ(mm2[m13],mm3[m13]);
	CHECK_FLTEQ(mm2[m14],mm3[m14]);
	CHECK_FLTEQ(mm2[m15],mm3[m15]);
}

TEST(QuatTransposeInplace, QuatFunctionsTests)
{
	// transpose inplace
	Quat q1;
	//q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4);
	Quat qx(ROT_X, Math::TwoPiOver3);
	Quat qy(ROT_Y, Math::ThreePiOver8);
	Quat qz(ROT_Z, Math::ThreePiOver4);

	// => q1.Set(ROT_XYZ,Math::2PiOver3,Math::3PiOver8,Math::3PiOver4
	q1 = qx*qy*qz;

	CHECK_FLTEQ(q1.x,0.01887f);
	CHECK_FLTEQ(q1.y,0.7715f);
	CHECK_FLTEQ(q1.z,0.1999f);
	CHECK_FLTEQ(q1.w,0.6036f);

	q1.Transpose();
	// => q1.T();

	CHECK_FLTEQ(q1.x,-0.01887f);
	CHECK_FLTEQ(q1.y,-0.7715f);
	CHECK_FLTEQ(q1.z,-0.1999f);
	CHECK_FLTEQ(q1.w,0.6036f);
}

TEST(QuatMag, QuatFunctionsTests)
{
	// magnitude()
	Quat q1;
	float value;
	q1.Set(1.0f,2.0f,3.0f,4.0f);

	// => mag = q1.Magnitude) 
	value = q1.Magnitude();
	
	CHECK_FLTEQ(value,5.477225f);
}

TEST(QuatMagSquared, QuatFunctionsTests)
{
	// magnitude Squared()
	
	Quat q1;
	float value;
	q1.Set(1.0f,2.0f,3.0f,4.0f);
	
	// => value = q1.MagnitudeSquared() 
	value = q1.MagnitudeSqr();

	CHECK_FLTEQ(value,30.0f);
}

TEST(QuatInvMag, QuatFunctionsTests)
{
	// inverse magnitude()
	Quat q1;
	float value;

	q1.Set(1.0f,2.0f,3.0f,4.0f);
	// => invMag = q1.invMagnitude) 
	value = q1.InverseMagnitude();

	CHECK_FLTEQ(value,0.1825742f);
}

TEST(Norm, QuatFunctionsTests)
{
	Quat q1,q2;
	// Normalize()

	q1.Set(1.0f,2.0f,3.0f,4.0f);
	// => q2 = q1.Normalize() 
	q2 = q1.Normalize();

	CHECK_FLTEQ(q2.Magnitude(),1.0f);
	CHECK_FLTEQ(q1.x,0.18257418f);
	CHECK_FLTEQ(q1.y,0.36514837f);
	CHECK_FLTEQ(q1.z,0.54772258f);
	CHECK_FLTEQ(q1.w,0.73029673f);
	
	CHECK_TRUE(q1.IsNormalized(Math::InternalTolerence));
	
	CHECK_FLTEQ(q1.MagnitudeSqr(),1.0f);
	
	CHECK_FLTEQ(q2.x,0.18257418f);
	CHECK_FLTEQ(q2.y,0.36514837f);
	CHECK_FLTEQ(q2.z,0.54772258f);
	CHECK_FLTEQ(q2.w,0.73029673f);
	
	CHECK_TRUE(q2.IsNormalized(Math::InternalTolerence));
	CHECK_FLTEQ(q2.MagnitudeSqr(),1.0f);
}

TEST(getNorm, QuatFunctionsTests)
{
	Quat q1,q2;
	// GetNormalized()
	q1.Set(1.0f,2.0f,3.0f,4.0f);
	// => q2 = q1.GetNormalized(); 
	q2 = q1.GetNormalized();

	CHECK_FLTEQ(q2.Magnitude(),1.0f);
	CHECK_FLTEQ(q2.x,0.18257418f);
	CHECK_FLTEQ(q2.y,0.36514837f);
	CHECK_FLTEQ(q2.z,0.54772258f);
	CHECK_FLTEQ(q2.w,0.73029673f);
	
	CHECK_FLTEQ(q1.x, 1.0f);
	CHECK_FLTEQ(q1.y, 2.0f);
	CHECK_FLTEQ(q1.z, 3.0f);
	CHECK_FLTEQ(q1.w, 4.0f);
}

TEST(GetInverse, QuatFunctionsTests)
{
	Quat q1,q2,q3;
	// GetInverse()\n");
	q1.Set(1.0f,2.0f,3.0f,4.0f);
	// => q2 = q1.GetInverse() 
	q2 = q1.GetInverse();
	
	CHECK_FLTEQ(q1.x,1.0f);
	CHECK_FLTEQ(q1.y,2.0f);
	CHECK_FLTEQ(q1.z,3.0f);
	CHECK_FLTEQ(q1.w,4.0f);
	
	CHECK_FLTEQ(q2.x,-0.0333333f);
	CHECK_FLTEQ(q2.y,-0.0666666f);
	CHECK_FLTEQ(q2.z,-0.1000000f);
	CHECK_FLTEQ(q2.w,0.1333333f);
	
	q3 = q1 * q2;
	
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
	
	q3 = q2 * q1;
	
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
}

TEST(Inverse, QuatFunctionsTests)
{
	Quat q1,q2,q3;
	// inverse()
	
	q1.Set(1.0f,2.0f,3.0f,4.0f);
	// => q1.Inverse() 
	q2 = q1;
	q1.Inverse();

	CHECK_FLTEQ(q1.x,-0.0333333f);
	CHECK_FLTEQ(q1.y,-0.0666666f);
	CHECK_FLTEQ(q1.z,-0.1000000f);
	CHECK_FLTEQ(q1.w,0.1333333f);
	
	q3 = q1 * q2;
	
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
	
	q3 = q2 * q1;
	
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
}

TEST(QuatInverse_3, QuatFunctionsTests)
{
	Quat q1,q2,q3;
	// inverse()
	
	q1.Set(1.0f,2.0f,3.0f,4.0f);
	// => q1.Inverse(q1) 
	q2 = q1;
	q1.Inverse();
	
	CHECK_FLTEQ(q2.x,1.0f);
	CHECK_FLTEQ(q2.y,2.0f);
	CHECK_FLTEQ(q2.z,3.0f);
	CHECK_FLTEQ(q2.w,4.0f);
	
	CHECK_FLTEQ(q1.x,-0.033333335f);
	CHECK_FLTEQ(q1.y,-0.066666670f);
	CHECK_FLTEQ(q1.z,-0.10000001f);
	CHECK_FLTEQ(q1.w, 0.13333334f);
	
	q3 = q1 * q2;
	
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
	
	q3 = q2 * q1;
	
	CHECK_FLTEQ(q3.x,0.0f);
	CHECK_FLTEQ(q3.y,0.0f);
	CHECK_FLTEQ(q3.z,0.0f);
	CHECK_FLTEQ(q3.w,1.0f);
}

TEST(Inverse_Transpose_Test, QuatFunctionsTests)
{
	Quat q1,q2,q3;
	// inverse()
	
	q1.Set(1.0f,2.0f,3.0f,4.0f);
	// => q1.Inverse(q1) 
	q2 = q1.Normalize();
	q2.Inverse();
	
	CHECK_FLTEQ(q1.Magnitude(), 1.0f);
	CHECK_FLTEQ(q1.x, 0.18257418f);
	CHECK_FLTEQ(q1.y, 0.36514837f);
	CHECK_FLTEQ(q1.z, 0.54772258f);
	CHECK_FLTEQ(q1.w, 0.73029673f);
	
	CHECK_FLTEQ(q2.x, -0.18257418f);
	CHECK_FLTEQ(q2.y, -0.36514837f);
	CHECK_FLTEQ(q2.z, -0.54772258f);
	CHECK_FLTEQ(q2.w, 0.73029673f);
	
	q3 = q1 * q2;
	
	CHECK_FLTEQ(q3.x, 0.0f);
	CHECK_FLTEQ(q3.y, 0.0f);
	CHECK_FLTEQ(q3.z, 0.0f);
	CHECK_FLTEQ(q3.w, 1.0f);
}

TEST(QuatLqcvq_Lqvqc_1, QuatFunctionsTests)
{
	// Lqcvq() and Lqvqc() operator
	
	//Matrix Mxyz(ROT_XYZ,Math::PiOver3,Math::3PiOver4,Math::PiOver2);
	Matrix4 mx(ROT_X,Math::PiOver3);
	Matrix4 my(ROT_Y,Math::ThreePiOver4);
	Matrix4 mz(ROT_Z,Math::PiOver2);
	Matrix4 Mxyz = mx*my*mz;

	Matrix4 MTxyz(Mxyz.GetTranspose());
	//Quat   Qxyz(ROT_XYZ,Math::PiOver3,Math::3PiOver4,Math::PiOver2);
	Quat qx(ROT_X,Math::PiOver3);
	Quat qy(ROT_Y,Math::ThreePiOver4);
	Quat qz(ROT_Z,Math::PiOver2);
	Quat Qxyz = qx*qy*qz;

	Quat   QTxyz(Qxyz.GetTranspose());

	Vector4 vx(1.0f, 0.0f, 0.0f, 1.0f);
	Vector4 vy(0.0f, 1.0f, 0.0f, 1.0f);
	Vector4 vz(0.0f, 0.0f, 1.0f, 1.0f);
	Vector4 vo(1.0f,2.0f,3.0f);
	Vector4 Vout1_x,Vout1_y,Vout1_z,Vout1_o;
	Vector4 Vout2_x,Vout2_y,Vout2_z,Vout2_o;

	Vout1_x = vx * Mxyz;
	Vout1_y = vy * Mxyz;
	Vout1_z = vz * Mxyz;
	Vout1_o = vo * Mxyz;

	// Vout = vIn * Quat
	Qxyz.Lqcvq(vx, Vout2_x);
	Qxyz.Lqcvq(vy, Vout2_y);
	Qxyz.Lqcvq(vz, Vout2_z);
	Qxyz.Lqcvq(vo, Vout2_o);

	// => Vout1_x = vx * Mxyz;
	// => Vout1_y = vy * Mxyz;
	// => Vout1_z = vz * Mxyz;
	// => Vout1_o = vo * Mxyz;
	// => Qxyz.Lqcvq(vx, Vout2_x);
	// => Qxyz.Lqcvq(vy, Vout2_y);
	// => Qxyz.Lqcvq(vz, Vout2_z);  
	// => Qxyz.Lqcvq(vo, Vout2_o);  

	CHECK_FLTEQ(Vout1_x.x,Vout2_x.x);
	CHECK_FLTEQ(Vout1_x.y,Vout2_x.y);
	CHECK_FLTEQ(Vout1_x.z,Vout2_x.z);
	CHECK_FLTEQ(Vout1_x.w,Vout2_x.w);

	CHECK_FLTEQ(Vout1_y.x,Vout2_y.x);
	CHECK_FLTEQ(Vout1_y.y,Vout2_y.y);
	CHECK_FLTEQ(Vout1_y.z,Vout2_y.z);
	CHECK_FLTEQ(Vout1_y.w,Vout2_y.w);

	CHECK_FLTEQ(Vout1_z.x,Vout2_z.x);
	CHECK_FLTEQ(Vout1_z.y,Vout2_z.y);
	CHECK_FLTEQ(Vout1_z.z,Vout2_z.z);
	CHECK_FLTEQ(Vout1_z.w,Vout2_z.w);

	CHECK_FLTEQ(Vout1_o.x,Vout2_o.x);
	CHECK_FLTEQ(Vout1_o.y,Vout2_o.y);
	CHECK_FLTEQ(Vout1_o.z,Vout2_o.z);
	CHECK_FLTEQ(Vout1_o.w,Vout2_o.w);

	Vout1_x = vx * Mxyz.GetTranspose();
	Vout1_y = vy * Mxyz.GetTranspose();
	Vout1_z = vz * Mxyz.GetTranspose();
	Vout1_o = vo * Mxyz.GetTranspose();

	// Vout = vIn * Quat.T()
	Qxyz.Lqvqc(vx, Vout2_x);
	Qxyz.Lqvqc(vy, Vout2_y);
	Qxyz.Lqvqc(vz, Vout2_z);
	Qxyz.Lqvqc(vo, Vout2_o);

	// => Vout1_x = vx * Mxyz.T();
	// => Vout1_y = vy * Mxyz.T();
	// => Vout1_z = vz * Mxyz.T();
	// => Vout1_o = vo * Mxyz.T();
	// => Qxyz.Lqvqc(vx, Vout2_x);
	// => Qxyz.Lqvqc(vy, Vout2_y);
	// => Qxyz.Lqvqc(vz, Vout2_z); 
	// => Qxyz.Lqvqc(vo, Vout2_o);  

	CHECK_FLTEQ(Vout1_x.x,Vout2_x.x);
	CHECK_FLTEQ(Vout1_x.y,Vout2_x.y);
	CHECK_FLTEQ(Vout1_x.z,Vout2_x.z);
	CHECK_FLTEQ(Vout1_x.w,Vout2_x.w);

	CHECK_FLTEQ(Vout1_y.x,Vout2_y.x);
	CHECK_FLTEQ(Vout1_y.y,Vout2_y.y);
	CHECK_FLTEQ(Vout1_y.z,Vout2_y.z);
	CHECK_FLTEQ(Vout1_y.w,Vout2_y.w);

	CHECK_FLTEQ(Vout1_z.x,Vout2_z.x);
	CHECK_FLTEQ(Vout1_z.y,Vout2_z.y);
	CHECK_FLTEQ(Vout1_z.z,Vout2_z.z);
	CHECK_FLTEQ(Vout1_z.w,Vout2_z.w);

	CHECK_FLTEQ(Vout1_o.x,Vout2_o.x);
	CHECK_FLTEQ(Vout1_o.y,Vout2_o.y);
	CHECK_FLTEQ(Vout1_o.z,Vout2_o.z);
	CHECK_FLTEQ(Vout1_o.w,Vout2_o.w);
}

TEST(dot_Test, QuatFunctionsTests)
{
	Quat q1;
	
	q1.Set(1.0f,2.0f,3.0f,4.0f);

	// 4D dot product
	float value = q1.Dot(q1);

	CHECK_FLTEQ(value, 30.0f);
	CHECK_FLTEQ(q1.x,1.0f);
	CHECK_FLTEQ(q1.y, 2.0f);
	CHECK_FLTEQ(q1.z, 3.0f);
	CHECK_FLTEQ(q1.w, 4.0f);
}

TEST(Assignment_Matrix_Test, QuatFunctionsTests)
{
	Quat q1;
	Matrix4 A(ROT_XYZ, 0.34f, -2.3f, 1.2f);
	
	Quat q2(A);

	q1 = A;

	Matrix4 B(q1);

	CHECK_FLTEQ(A[m0], B[m0]);
	CHECK_FLTEQ(A[m1], B[m1]);
	CHECK_FLTEQ(A[m2], B[m2]);
	CHECK_FLTEQ(A[m3], B[m3]);
	CHECK_FLTEQ(A[m4], B[m4]);
	CHECK_FLTEQ(A[m5], B[m5]);
	CHECK_FLTEQ(A[m6], B[m6]);
	CHECK_FLTEQ(A[m7], B[m7]);
	CHECK_FLTEQ(A[m8], B[m8]);
	CHECK_FLTEQ(A[m9], B[m9]);
	CHECK_FLTEQ(A[m10], B[m10]);
	CHECK_FLTEQ(A[m11], B[m11]);
	CHECK_FLTEQ(A[m12], B[m12]);
	CHECK_FLTEQ(A[m13], B[m13]);
	CHECK_FLTEQ(A[m14], B[m14]);
	CHECK_FLTEQ(A[m15], B[m15]);
}


