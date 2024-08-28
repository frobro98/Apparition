// Copyright 2020, Nathan Blane

#pragma once

#include "String/String.h"
#include "String/StringView.hpp"
#include "Containers/StaticArray.hpp"
#include "Containers/Map.h"
#include "InputDefinitions.hpp"
#include "Math/Vector2.hpp"
#include "Math/IntVector2.hpp"

#include "MusaAPI.hpp"

class ApplicationInputMap;

namespace Input
{
enum class ButtonState : u8
{
	Pressed,
	Repeated,
	Released
};

enum class EventType
{
	Button,
	Mouse,
	Analog
};

struct DownState
{
	u8 endedDown : 1;
	u8 previouslyDown : 1;
};
static_assert(sizeof(DownState) == sizeof(u8));

struct ButtonEvent
{
	Buttons button;
	ButtonState buttonState;
	DownState downState;
	bool isRepeated;
};

struct AnalogEvent
{
	Buttons analogButton;
	f32 normValue;
};

struct MouseEvent
{
	IntVector2 currentScreenSpacePosition;
	IntVector2 currentClientPosition;
	IntVector2 previousScreenSpacePosition;
	IntVector2 previousClientPosition;
	IntVector2 deltaPosition;
	f32 scrollDelta{ 0 };
};

struct Event
{
	Event() {}

	union
	{
		ButtonEvent buttonEvent;
		AnalogEvent analogEvent;
		MouseEvent mouseEvent;
	};
	EventType type;
};

constexpr u32 MaxSupportedControllers = 4;

struct GamepadState
{
	// Analog fields, unnormalized
	Vector2 leftStick;
	Vector2 rightStick;
	f32 leftTrigger;
	f32 rightTrigger;

	// Button frame state
	StaticArray<DownState, Input::GP_Max> buttonStates;
	bool active;
};

using StateMap = StaticArray<Input::DownState, Input::KM_Max>;
using GamepadStates = StaticArray<Input::GamepadState, Input::MaxSupportedControllers>;
using ActiveGamepads = StaticArray<bool, Input::MaxSupportedControllers>;

void InitializeInput(ApplicationInputMap& inputHandler);

bool MUSA_API IsPressed(Input::Buttons key);
bool MUSA_API IsDown(Input::Buttons key);
IntVector2 MUSA_API GetMousePosition();
}
