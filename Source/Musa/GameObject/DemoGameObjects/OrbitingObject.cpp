#include "OrbitingObject.hpp"
#include "Math/Matrix4.hpp"
#include "Math/Quat.hpp"

OrbitingObject::OrbitingObject(const Vector4& orbitAxis, const Vector4& orbitPos)
	: axis(orbitAxis),
	orbitLocation(orbitPos)
{
}

void OrbitingObject::Update(float /*tick*/)
{
// 	constexpr float angleOffsetDeg = .005f;
// 	Matrix4 trans(TRANS, orbitLocation);
// 	Quat rot(ROT_AXIS_ANGLE, axis, angleOffsetDeg * tick);
// 	Matrix4 negTrans(TRANS, -orbitLocation);
// 
// 	Matrix4 orbitTrans = negTrans * rot * trans;
// 
// 	position *= orbitTrans;
// 
// 	GameObject::Update(tick);
}
