#include "OrbitOtherObject.hpp"
#include "Math/Quat.hpp"

OrbitOtherObject::OrbitOtherObject(/*const GameObject& obj, */float r, const Vector4& orbitAxis)
	: axis(orbitAxis.GetNormalized()),
	//object(obj),
	radius(r)
{
	//prevObjectPosition = object.GetPosition();
	InitializeStartPosition();
}

void OrbitOtherObject::Update(float /*tick*/)
{
// 	constexpr float angleOffsetDeg = Math::PiOver2;
// 
// 	Vector4 objectPos = object.GetPosition();
// 	Vector4 dir = objectPos - prevObjectPosition;
// 
// 	Matrix4 trans(TRANS, objectPos);
// 	Quat rot(ROT_AXIS_ANGLE, axis, angleOffsetDeg * tick);
// 	Matrix4 negTrans(TRANS, -objectPos);
// 
// 	Matrix4 orbitTrans = negTrans * rot * trans;
// 
// 	orbitPosition += dir;
// 	orbitPosition *= orbitTrans;
// 
// 	SetPos(orbitPosition);
// 	GameObject::Update(tick);
// 
// 	prevObjectPosition = objectPos;
}

void OrbitOtherObject::InitializeStartPosition()
{
	Vector4 crossPos = prevObjectPosition + Vector4(0, radius, 0);
	Vector4 crossVec = crossPos - prevObjectPosition;
	crossVec.Normalize();
	if (crossVec.IsEqual(axis))
	{
		crossPos = prevObjectPosition + Vector4(radius, 0, 0);
		crossVec = crossPos - prevObjectPosition;
	}
	Vector4 perpVec = crossVec.Cross(axis);
	Vector4 orbitPosVec = perpVec.Cross(axis);
	orbitPosVec.Normalize();
	orbitPosVec *= radius;

	orbitPosition = prevObjectPosition + orbitPosVec;
}
