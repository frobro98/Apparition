// Copyright 2020, Nathan Blane

#pragma once

#include "ECS/Component.hpp"
#include "Math/Vector3.hpp"

struct ScaleComponent : Musa::Component
{
	Vector3 scale;
};
