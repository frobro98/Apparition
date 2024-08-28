// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/VectorFunctions.hpp"
#include "Math/MathFunctions.hpp"

// TODO - This should go into a bigger test of lerping more than just vector

TEST(LERP_Array, VectorLerpTests)
{
	Vector4 vSource[] =
	{
		Vector4(1.00f, 2.0f,   3.0f,  4.0f),
		Vector4(21.00f, 12.0f, 23.0f, 40.0f),
		Vector4(13.00f, 24.0f, 33.0f, 40.0f),
		Vector4(0.50f, 22.0f, 43.0f, 40.0f),
		Vector4(1.30f, 23.0f, 35.0f, 40.0f),
		Vector4(15.00f, 72.0f, 35.0f, 40.0f),
		Vector4(21.00f, 92.0f, 33.0f, 40.0f),
	};

	Vector4 vTarget[] =
	{
		Vector4(-1.00f, 12.0f,   13.0f,  41.0f),
		Vector4(213.00f, -12.0f, 263.0f, 140.0f),
		Vector4(133.00f, -24.0f, 363.0f, 240.0f),
		Vector4(3.50f, -22.0f, 453.0f, 340.0f),
		Vector4(13.30f, -23.0f, 365.0f, 440.0f),
		Vector4(153.00f, -72.0f, 735.0f, 540.0f),
		Vector4(231.00f, -92.0f, 733.0f, 340.0f),
	};

	constexpr size_t NumVectors = ArraySize(vSource);

	Vector4 vResult[NumVectors];
	Vector4 vout;

	Math::LerpArray(vResult, vSource, vTarget, 0.65f, NumVectors);

	for (int i = 0; i < NumVectors; i++)
	{
		vout = Math::Lerp(vSource[i], vTarget[i], 0.65f);

		CHECK_FLTEQ(vout.x, vResult[i].x);
		CHECK_FLTEQ(vout.y, vResult[i].y);
		CHECK_FLTEQ(vout.z, vResult[i].z);
		CHECK_FLTEQ(vout.w, vResult[i].w);
	}
}

TEST(MathLerp, MathTests)
{
	Vector4 va(0.0f, 0.0f, 0.0f);
	Vector4 vb(10.0f, 20.0f, 30.0f);
	Vector4 vout;

	vout = Math::Lerp(va, vb, 0.0f);

	CHECK_FLTEQ(vout.x, 0.0f);
	CHECK_FLTEQ(vout.y, 0.0f);
	CHECK_FLTEQ(vout.z, 0.0f);
	CHECK_FLTEQ(vout.w, 1.0f);
}

TEST(MathLerp2, MathTests)
{
	Vector4 va(0.0f, 0.0f, 0.0f);
	Vector4 vb(10.0f, 20.0f, 30.0f);
	Vector4 vout;

	vout = Math::Lerp(va, vb, 0.5f);

	CHECK_FLTEQ(vout.x, 5.0f);
	CHECK_FLTEQ(vout.y, 10.0f);
	CHECK_FLTEQ(vout.z, 15.0f);
	CHECK_FLTEQ(vout.w, 1.0f);
}

TEST(MathLerp3, MathTests)
{
	Vector4 va(0.0f, 0.0f, 0.0f);
	Vector4 vb(10.0f, 20.0f, 30.0f);
	Vector4 vout;

	vout = Math::Lerp(va, vb, 0.25f);

	CHECK_FLTEQ(vout.x, 2.5f);
	CHECK_FLTEQ(vout.y, 5.0f);
	CHECK_FLTEQ(vout.z, 7.50f);
	CHECK_FLTEQ(vout.w, 1.0f);
}

TEST(MathLerp4, MathTests)
{
	Vector4 va(0.0f, 0.0f, 0.0f);
	Vector4 vb(10.0f, 20.0f, 30.0f);
	Vector4 vout;

	vout = Math::Lerp(va, vb, 1.0f);

	CHECK_FLTEQ(vout.x, 10.0f);
	CHECK_FLTEQ(vout.y, 20.0f);
	CHECK_FLTEQ(vout.z, 30.0f);
	CHECK_FLTEQ(vout.w, 1.0f);
}


