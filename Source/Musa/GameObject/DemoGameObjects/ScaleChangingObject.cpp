#include "ScaleChangingObject.hpp"
#include "Math/MathFunctions.hpp"

ScaleChangingObject::ScaleChangingObject(f32 scaleA, f32 scaleB, f32 delta_)
	: A(scaleA, scaleA, scaleA),
	B(scaleB, scaleB, scaleB),
	srcVec(&A),
	dstVec(&B),
	delta(delta_)
{
	//scale = A;
}

void ScaleChangingObject::Update(f32 /*tick*/)
{
	if (totalDelta >= 1.f)
	{
		srcVec = srcVec == &A ? &B : &A;
		dstVec = dstVec == &B ? &A : &B;
		totalDelta = 0.f;
	}

// 	scale = Math::Lerp(*srcVec, *dstVec, totalDelta);
// 
// 	totalDelta += tick;
// 
// 	GameObject::Update(tick);
}
