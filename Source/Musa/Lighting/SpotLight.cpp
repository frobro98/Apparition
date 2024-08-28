// Copyright 2020, Nathan Blane

#include "SpotLight.hpp"
#include "Math/MathFunctions.hpp"

constexpr f32 RadAdjustment = .001f;
constexpr f32 MaxInnerRad = Math::DegreesToRadians(89.f);
constexpr f32 MaxOuterRad = MaxInnerRad + .001f;

LightDescription SpotLight::GetLightDescription()
{
//	worldLight = worldTransform.GetInverse();

	const f32 innerRad = Math::DegreesToRadians(innerAngleDegrees);
	const f32 outerRad = Math::DegreesToRadians(outerAngleDegrees);

	const f32 clampedInnerRad = Math::Clamp(innerRad, 0.f, MaxInnerRad);
	const f32 clampedOuterRad = Math::Clamp(outerRad, clampedInnerRad + RadAdjustment, MaxOuterRad);

	const f32 cosInner = Math::Cos(clampedInnerRad);
	const f32 cosOuter = Math::Cos(clampedOuterRad);

	LightDescription info = {};
	info.direction = -GetDirection();
	info.cosOuterAngle = cosOuter;
	info.inverseCosDiff = 1 / (cosInner - cosOuter);
	//info.position = position;
	info.radius = attenuationRadius;

	return info;
}
