// Copyright 2020, Nathan Blane

#include "ApplicationInputMap.hpp"
#include "Internal/InputInternal.hpp"
#include "InputEvents.hpp"
#include "Window/Window.h"
#include "Application/MusaApp.hpp"

#include "Debugging/DebugOutput.hpp"

ApplicationInputMap::ApplicationInputMap(MusaApp& app)
	: application(app)
{
	ZeroMem(inputMap.internalData, sizeof(Input::DownState) * inputMap.Size());
}

Input::DownState ApplicationInputMap::KeyUp(Input::Buttons input)
{
	inputMap[input].endedDown = false;
	return inputMap[input];

// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver) 
// 	{
// 		Assert(receiver);
// 		return receiver->OnKeyUp(input);
// 	});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnKeyUp(input);
// 		HandleInputEvents(events);
// 	}
}

Input::DownState ApplicationInputMap::KeyDown(Input::Buttons input)
{
	inputMap[input].endedDown = true;
	return inputMap[input];

// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 	{
// 		Assert(receiver);
// 		return receiver->OnKeyDown(input, isRepeated);
// 	});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnKeyDown(input, isRepeated);
// 		HandleInputEvents(events);
// 	}
}

Input::DownState ApplicationInputMap::MouseDown(Input::Buttons mouseButton)
{
	inputMap[mouseButton].endedDown = true;
	return inputMap[mouseButton];

// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 	{
// 		Assert(receiver);
// 		return receiver->OnMouseDown(mouseButton);
// 	});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnMouseDown(mouseButton);
// 		HandleInputEvents(events);
// 	}
}

Input::DownState ApplicationInputMap::MouseUp(Input::Buttons mouseButton)
{
	inputMap[mouseButton].endedDown = false;
	return inputMap[mouseButton];

// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 	{
// 		Assert(receiver);
// 		return receiver->OnMouseUp(mouseButton);
// 	});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnMouseUp(mouseButton);
// 		HandleInputEvents(events);
// 	}
}

void ApplicationInputMap::MouseMove(const IntVector2& mousePosition_, const IntVector2& /*clientMousePos_*/)
{
	mousePosition = mousePosition_;

	// TODO - This wouldn't be handled by this class. This would be handled by the input event creation system that
	// communicates with the ECS world.
// 	if (!rawInputTurnedOn)
// 	{
// 		IntVector2 mouseMovePos = IntVector2(mouseX, mouseY);
// 		currentMousePos = mouseMovePos;
// 		IntVector2 delta = currentMousePos - prevMousePos;
// 
// 		if (currentMousePos != prevMousePos)
// 		{
// 			InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 			{
// 				Assert(receiver);
// 				return receiver->OnMouseMove(mouseMovePos, prevMousePos, delta);
// 			});
// 
// 			if (!events.IsInputHandled())
// 			{
// 				events = gameInput.OnMouseMove(mouseMovePos, prevMousePos, delta);
// 				HandleInputEvents(events);
// 			}
// 
// 			prevMousePos = mouseMovePos;
// 		}
// 	}
}

// void ApplicationInputMap::RawMouseMove(u32 mouseX, u32 mouseY, i32 deltaX, i32 deltaY)
// {
// 	// TODO - This wouldn't be handled by this class. This would be handled by the input event creation system that
// 	// communicates with the ECS world.
// 	IntVector2 delta(deltaX, deltaY);
// 	IntVector2 mouseMovePos(mouseX, mouseY);
// 	currentMousePos = mouseMovePos;
// 
// 	if (deltaX || deltaY)
// 	{
// 		InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 		{
// 			Assert(receiver);
// 			return receiver->OnMouseMove(mouseMovePos, prevMousePos, delta);
// 		});
// 
// 		if (!events.IsInputHandled())
// 		{
// 			events = gameInput.OnMouseMove(mouseMovePos, prevMousePos, delta);
// 			HandleInputEvents(events);
// 		}
// 
// 		prevMousePos = mouseMovePos;
// 	}
// }

// void WindowInputHandler::HandleControllerAnalogChange(u32 controllerIndex, Input::Gamepad analogType, f32 analogValue)
// {
// 	Assert(analogType >= Input::Gamepad_LeftTrigger && Input::Gamepad_RightStick_YAxis);
// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 		{
// 			Assert(receiver);
// 			return receiver->OnControllerAnalogChange(controllerIndex, analogType, analogValue);
// 		});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnControllerAnalogChange(controllerIndex, analogType, analogValue);
// 		HandleInputEvents(events);
// 	}
// 
// }

// void WindowInputHandler::HandleControllerButtonDown(u32 /*controllerIndex*/, Input::Gamepad analogType)
// {
// 	
// 
// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 	{
// 		Assert(receiver);
// 		return receiver->OnKeyDown(analogType, false);
// 	});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnKeyDown(analogType, false);
// 		HandleInputEvents(events);
// 	}
// }

// void WindowInputHandler::HandleControllerButtonUp(u32 /*controllerIndex*/, Input::Gamepad analogType)
// {
// 	
// 
// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 	{
// 		Assert(receiver);
// 		return receiver->OnKeyUp(analogType);
// 	});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnKeyUp(analogType);
// 		HandleInputEvents(events);
// 	}
// }

// void WindowInputHandler::HandleWindowResized(u32 newWidth, u32 newHeight)
// {
// 	if (window)
// 	{
// 		window->Resize(newWidth, newHeight);
// 		viewportResize = IntVector2(newWidth, newHeight);
// 	}
// }
// 
// void WindowInputHandler::HandleWindowClose()
// {
// 	if (window)
// 	{
// 		window->Close();
// 		gameInput.OnWindowClose();
// 	}
// }
// 
// void WindowInputHandler::HandleActivationChanged(bool activated)
// {
// 	activation = activated;
// 	InputEvents events = ProcessInputReceivers([&](IInputReceiver* receiver)
// 		{
// 			Assert(receiver);
// 			return receiver->OnActivationChanged(activated);
// 		});
// 
// 	if (!events.IsInputHandled())
// 	{
// 		events = gameInput.OnActivationChanged(activated);
// 		HandleInputEvents(events);
// 	}
// }
// 
// void WindowInputHandler::SetInputFocusToGame()
// {
// 	InputEvents events = gameInput.OnFocusReceived();
// 	HandleInputEvents(events);
// }

void ApplicationInputMap::SetGamepadInformation(const Input::GamepadStates& states, const Input::ActiveGamepads& active)
{
	gamepads = states;
	activeGamepads = active;
}

void ApplicationInputMap::CopyKeyboardMouseInput(Input::StateMap& outInputs) const
{
	Memcpy(outInputs.internalData, inputMap.internalData, outInputs.Size());
}

Input::ModifierFlags ApplicationInputMap::GetModifierFlags() const
{
	return modFlags;
}

void ApplicationInputMap::CopyGamepadStates(Input::GamepadStates& gps, Input::ActiveGamepads& activeGPs) const
{
	Memcpy(gps.internalData, gamepads.internalData, gps.Size());
	Memcpy(activeGPs.internalData, activeGamepads.internalData, activeGPs.Size());
}

// void WindowInputHandler::PostUpdateInput()
// {
// 	gameInput.ProcessGameInputs();
// }

void ApplicationInputMap::PrepInputForFrame()
{
	for (u32 i = 0; i < inputMap.Size(); ++i)
	{
		inputMap[i].previouslyDown = inputMap[i].endedDown;
	}

	for (u32 i = 0; i < Input::MaxSupportedControllers; ++i)
	{
		if (activeGamepads[i])
		{
			Input::GamepadState& state = gamepads[i];
			for (u32 j = 0; j < Input::GP_Max; ++j)
			{
				state.buttonStates[j].previouslyDown = state.buttonStates[j].endedDown;
			}
		}
	}
}

bool ApplicationInputMap::IsPressed(Input::Buttons key)
{
	const Input::DownState state = inputMap[key];
	return state.endedDown && !state.previouslyDown;
}

bool ApplicationInputMap::IsDown(Input::Buttons key)
{
	const Input::DownState state = inputMap[key];
	return state.endedDown;
}

IntVector2 ApplicationInputMap::GetMousePosition() const
{
	return application.GetMousePosition(); 
}

// void ApplicationInputMap::HandleInputEvents(const InputEvents& events)
// {
// 	if (events.ContainsEvents())
// 	{
// 		std::optional<IntVector2> mousePos = events.GetChangedMousePosition();
// 		if (mousePos)
// 		{
// 			mousePosition = *mousePos;
			//prevMousePos = mousePosition;
			//application.SetMousePosition(mousePos.value());
// 		}
// 
// 		std::optional<bool> lockCursor = events.GetLockCursor();
// 		if (lockCursor)
// 		{
			// TODO - Having lock and unlock as 2 separate calls might not make sense
// 			bool shouldLock = *lockCursor;
// 			if (shouldLock)
// 			{
// 				application.LockCursor();
// 			}
// 			else
// 			{
// 				application.UnlockCursor();
// 			}
// 		}
// 
// 		std::optional<bool> showCursor = events.GetShowCursor();
// 		if (showCursor)
// 		{
			//rawInputTurnedOn = !showCursor.value();
// 			application.ShowCursor(*showCursor);
// 			application.GetOSApp().SetRawMouseInput(rawInputTurnedOn, *window);
// 		}
// 	}
// }
