// Copyright 2020, Nathan Blane

#include "FastFourierTransform.hpp"
#include "Math/MathConstants.hpp"
#include "Math/MathFunctions.hpp"
#include "FastFourierTransform.hpp"
#include "Utilities/BitUtilities.hpp"

namespace
{
u32 NumberOfBitsNeeded(u32 samplesPow2)
{
	Assert(IsPowerOf2(samplesPow2));
	Assert(samplesPow2 >= 2);

	for (u32 i = 0;; ++i)
	{
		if (samplesPow2 & (1 << i))
		{
			return i;
		}
	}
}

u32 ReverseBits(u32 index, u32 numBits)
{
	u32 revBits = 0;
	for (u32 i = 0; i < numBits; ++i)
	{
		revBits = (revBits << 1) | (index & 1);
		index >>= 1;
	}
	return revBits;
}
}

void FastFourier(u32 numSamples, const DynamicArray<float>& realIn, const DynamicArray<float>& imaginaryIn, DynamicArray<float>& realOut, DynamicArray<float>& imaginaryOut)
{
	Assert(IsPowerOf2(numSamples));
	u32 numBitsNeeded = NumberOfBitsNeeded(numSamples);

	for (u32 i = 0; i < numSamples; ++i)
	{
		u32 ind = ReverseBits(i, numBitsNeeded);
		realOut[ind] = realIn[i];
		imaginaryOut[ind] = imaginaryIn[i];
	}

	u32 blockEnd = 1;
	for (u32 blockSize = 2; blockSize <= numSamples; blockSize <<= 1)
	{
		float deltaAngle = Math::TwoPi / blockSize;
		float sm2 = Math::Sin(-2.f * deltaAngle);
		float sm1 = Math::Sin(-deltaAngle);
		float cm2 = Math::Cos(-2.f * deltaAngle);
		float cm1 = Math::Cos(-deltaAngle);
		float w = 2.f * cm1;
		float ar[3], ai[3];

		for (u32 i = 0; i < numSamples; i += blockSize)
		{
			ar[2] = cm2;
			ar[1] = cm1;

			ai[2] = sm2;
			ai[1] = sm1;

			for (u32 j = i, n = 0; n < blockEnd; j++, n++)
			{
				ar[0] = w * ar[1] - ar[2];
				ar[2] = ar[1];
				ar[1] = ar[0];

				ai[0] = w * ai[1] - ai[2];
				ai[2] = ai[1];
				ai[1] = ai[0];

				u32 k = j + blockEnd;
				float tr = ar[0] * realOut[k] - ai[0] * imaginaryOut[k];
				float ti = ar[0] * imaginaryOut[k] + ai[0] * realOut[k];

				realOut[k] = realOut[j] - tr;
				imaginaryOut[k] = imaginaryOut[j] - ti;

				realOut[j] += tr;
				imaginaryOut[j] += ti;
			}
		}

		blockEnd = blockSize;
	}
}

void InverseFastFourier(u32 numSamples, const DynamicArray<float>& realIn, const DynamicArray<float>& imageIn, DynamicArray<float>& realOut, DynamicArray<float>& imageOut)
{
	FastFourier(numSamples, realIn, imageIn, realOut, imageOut);

	float denom = static_cast<float>(numSamples);

	for (u32 i = 0; i < numSamples; ++i)
	{
		realOut[i] /= denom;
		imageOut[i] /= denom;
	}
}
