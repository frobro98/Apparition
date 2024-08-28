// Copyright 2020, Nathan Blane

#pragma once

#include "ECS/Component.hpp"
#include "Math/Quat.hpp"

struct Rotation : Musa::Component
{
	Quat rotation;
};

