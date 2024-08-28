// Copyright 2020, Nathan Blane

#include "Input.hpp"
#include "Internal/InputInternal.hpp"
#include "Containers/StaticArray.hpp"
#include "Window/Window.h"
#include "Debugging/MetricInterface.hpp"

#include "Application/MusaApp.hpp"

DECLARE_METRIC_GROUP(Input);
METRIC_STAT(UpdateInputs, Input);
METRIC_STAT(InputMapUpdate, Input);


static ApplicationInputMap* inputHandler;

namespace Internal
{

// void UpdateInputMap()
// {
// 	BEGIN_TIMED_BLOCK(InputMapUpdate);
// 	inputHandler->PrepInputForFrame();
// 	END_TIMED_BLOCK(InputMapUpdate);
// }
}

namespace Input
{
void InitializeInput(ApplicationInputMap& handler)
{
	inputHandler = &handler;
	Assert(inputHandler);
}

bool IsPressed(Input::Buttons key)
{
	return inputHandler->IsPressed(key);
}

bool IsDown(Input::Buttons key)
{
	return inputHandler->IsDown(key);
}

IntVector2 GetMousePosition()
{
	return inputHandler->GetMousePosition();
}
}

