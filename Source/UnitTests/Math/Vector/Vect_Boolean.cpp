// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Math/Vector4.hpp"

TEST(IsEqualFalse, vector_tests)
{
	Vector4 v1(1.0f, 2.0f, 3.0f);
	Vector4 v2(9.0f, -3.0f, 5.0f);
	bool bvalue1;

	bvalue1 = v1.IsEqual(v2, 0.1f);

	CHECK_FALSE(bvalue1);
}

TEST(IsEqualTrue, vector_tests)
{
	bool bvalue1;
	Vector4 v1(1.0f, 2.0f, 3.0f);
	Vector4 v2(1.0f, 2.0f, 3.05f);

	bvalue1 = v1.IsEqual(v2, 0.1f);

	CHECK_TRUE(bvalue1);
}

TEST(IsZeroFalse, vector_tests)
{
	bool bvalue1;
	Vector4 v1(0.023f, 0.0f, 0.0f, 1.05f);

	bvalue1 = v1.IsZero(0.0f);

	CHECK_FALSE(bvalue1);
}

TEST(IsZeroTrue, vector_tests)
{
	bool bvalue1;
	Vector4 v1(0.0f, 0.0f, 0.0f, 1.05f);

	bvalue1 = v1.IsZero(0.1f);

	CHECK_TRUE(bvalue1);
}

TEST(IsEqualFalse_default, vector_tests)
{
	Vector4 v1(1.0f, 2.0f, 3.0f);
	Vector4 v2(9.0f, -3.0f, 5.0f);
	bool bvalue1;

	bvalue1 = v1.IsEqual(v2);

	CHECK_FALSE(bvalue1);
}

TEST(IsEqualTrue_default, vector_tests)
{
	bool bvalue1;
	Vector4 v1(1.0f, 2.0f, 3.0f);
	Vector4 v2(1.0f, 2.0f, 3.00005f);

	bvalue1 = v1.IsEqual(v2);

	CHECK_TRUE(bvalue1);
}

TEST(IsZeroFalse_default, vector_tests)
{
	bool bvalue1;
	Vector4 v1(0.023f, 0.0f, 0.0f, 1.05f);

	bvalue1 = v1.IsZero();

	CHECK_FALSE(bvalue1);
}

TEST(IsZeroTrue_default, vector_tests)
{
	bool bvalue1;
	Vector4 v1(0.0f, 0.0f, 0.0f, 1.00005f);

	bvalue1 = v1.IsZero();

	CHECK_TRUE(bvalue1);
}


