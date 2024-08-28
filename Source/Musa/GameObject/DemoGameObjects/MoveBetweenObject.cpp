
#include "MoveBetweenObject.hpp"
#include "Math/VectorFunctions.hpp"

MoveBetweenObject::MoveBetweenObject(const Vector4& pointA, const Vector4& pointB, f32 startingDelta)
	: A(pointA), 
	B(pointB),
	srcVec(&A),
	dstVec(&B),
	totalDelta(startingDelta)
{
	//position = Math::Lerp(*srcVec, *dstVec, totalDelta);
}

void MoveBetweenObject::Update(float /*tick*/)
{
	if (totalDelta >= 1.f)
	{
		srcVec = srcVec == &A ? &B : &A;
		dstVec = dstVec == &B ? &A : &B;
		totalDelta = 0.f;
	}

	//position = Math::Lerp(*srcVec, *dstVec, totalDelta);

	totalDelta += .003f;

	//GameObject::Update(tick);
}
