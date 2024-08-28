// Copyright 2020, Nathan Blane

#pragma once

#include "Lighting/Light.hpp"

class PointLight : public Light
{
public:
	virtual LightDescription GetLightDescription() override;

	f32 attenuationRadius;


};