// Copyright 2020, Nathan Blane

#pragma once

#include <optional>

#include "ECS/Component.hpp"
#include "Containers/StaticArray.hpp"
#include "Input/InputDefinitions.hpp"
#include "Input/Input.hpp"
#include "Math/IntVector2.hpp"

struct OSInputsComponent : Musa::Component
{
	// Keyboard and mouse
	Input::StateMap inputMap;
	Input::ModifierFlags modFlags;
	IntVector2 currentMousePosition;
	IntVector2 prevMousePosition;
	IntVector2 deltaPosition;

	// Gamepads
	Input::GamepadStates gamepads;
	Input::ActiveGamepads activeGamepads;
};
