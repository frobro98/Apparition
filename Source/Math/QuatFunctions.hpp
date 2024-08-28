// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Quat.hpp"
#include "Math/MathFunctions.hpp"

//----------------------------------------------------------------------------- 
// Mutates result to be a slerp between the source and target by the
// specified factor.
// For a factor of 0.0, result == source.
// For a factor of 1.0, result == target.
//-----------------------------------------------------------------------------

namespace Math
{
	inline Quat Slerp(const Quat &source, const Quat &target, const float slerpFactor)
	{
		Quat normSrc = source.GetNormalized();
		Quat normTar = target.GetNormalized();
		Quat result;
		if (!normSrc.IsEquivalent(normTar, .001f))
		{
			float tmpCos = normSrc.Dot(normTar);
			if (tmpCos < 0.f)
			{
				tmpCos = -tmpCos;
				normTar = -normTar;
			}

			float theta = Math::Acos(tmpCos);
			if (Math::IsZero(theta) || theta != theta) // Nan test
			{
				result = normSrc;
			}
			else
			{
				float srcDelta = Math::Sin((1 - slerpFactor) * theta) / Math::Sin(theta);
				float tarDelta = Math::Sin(slerpFactor * theta) / Math::Sin(theta);
				result = normSrc * srcDelta + normTar * tarDelta;
			}
		}
		else
		{
			result = normSrc;
		}

		return result;
	}
    
	inline void SlerpArray(Quat *result, const Quat *source, const Quat *target, const float slerpFactor, const int numQuats)
	{
		for (i32 i = 0; i < numQuats; ++i)
		{
			result[i] = Slerp(source[i], target[i], slerpFactor);
		}
	}
};
	

