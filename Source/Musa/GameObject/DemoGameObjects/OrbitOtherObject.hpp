#pragma once

#include "Math/Vector4.hpp"

class OrbitOtherObject// : public GameObject
{
public:
	OrbitOtherObject(/*const GameObject& obj, */float r, const Vector4& axis);

	virtual void Update(float tick) /*override*/;

private:
	void InitializeStartPosition();

private:
	Vector4 axis;
	Vector4 prevObjectPosition;
	Vector4 orbitPosition;
	//const GameObject& object;
	float radius = 0.f;
};