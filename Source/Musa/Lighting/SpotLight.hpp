// Copyright 2020, Nathan Blane

#pragma once

#include "Lighting/Light.hpp"

class SpotLight : public Light
{
public:

	SpotLight() = default;

	virtual LightDescription GetLightDescription() override;

	f32 innerAngleDegrees;
	f32 outerAngleDegrees;
	f32 attenuationRadius;

private:
	// Eventually, these will be out of here and only calculated when needed. 
	float cosInnerAngle = 0;
	float cosOuterAngle = 0;
};
