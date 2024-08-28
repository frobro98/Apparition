// Copyright 2020, Nathan Blane

#include "PointLight.hpp"

LightDescription PointLight::GetLightDescription()
{
//	worldLight = worldTransform.GetInverse();

	LightDescription info;
// 	info.position = position;
// 	info.color = lightColor;
// 	info.direction = -GetDirection();
// 	info.cosOuterAngle = -2.f; // Ensures that the clamped value will be 1
// 	info.inverseCosDiff = 1.f;
// 	info.radius = attenuationRadius;

	return info;
}
