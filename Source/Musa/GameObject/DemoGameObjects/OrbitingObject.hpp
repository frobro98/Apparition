#pragma once

#include "Math/Vector4.hpp"

class OrbitingObject// : public GameObject
{
public:
	OrbitingObject(const Vector4& orbitAxis, const Vector4& orbitPos);

	virtual void Update(float tick) /*override*/;

private:
	Vector4 axis;
	Vector4 orbitLocation;
};