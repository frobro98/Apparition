#pragma once

//#include "GameObject/GameObject.h"
#include "Math/Vector4.hpp"

class MoveBetweenObject// : public GameObject
{
public:
	MoveBetweenObject(const Vector4& pointA, const Vector4& pointB, f32 startingDelta = 0.f);

	virtual void Update(float tick) /*override*/;

private:
	Vector4 A, B;
	Vector4* srcVec;
	Vector4* dstVec;
	float totalDelta;
};