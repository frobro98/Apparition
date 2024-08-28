#pragma once

#include "Math/Vector4.hpp"
//#include "GameObject/GameObject.h"

class ScaleChangingObject //: public GameObject
{
public:
	ScaleChangingObject(f32 scaleA, f32 scaleB, f32 delta);

	virtual void Update(f32 tick) /*override*/;

private:
	Vector4 A, B;
	Vector4* srcVec;
	Vector4* dstVec;
	f32 delta;
	f32 totalDelta = 0.f;
};