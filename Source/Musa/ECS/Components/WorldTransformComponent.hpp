// Copyright 2020, Nathan Blane

#pragma once

#include "ECS/Component.hpp"
#include "Math/Matrix4.hpp"

struct WorldTransformComponent : Musa::Component
{
	Matrix4 transform;
	// TODO - Could put the direction vectors for the matrix in here as well
};
