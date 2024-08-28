// Copyright 2020, Nathan Blane

#pragma once

#include "Input/Input.hpp"
#include "Containers/DynamicArray.hpp"

class MusaApp;

// TODO - This class represents a sink in which input gets stored. It shouldn't really handle any input, 
// therefore shouldn't contain any of that kind of behavior. 
class ApplicationInputMap final
{
public:
	ApplicationInputMap(MusaApp& app);

	NODISCARD Input::DownState KeyUp(Input::Buttons input);
	NODISCARD Input::DownState KeyDown(Input::Buttons input);
	NODISCARD Input::DownState MouseDown(Input::Buttons mouseButton);
	NODISCARD Input::DownState MouseUp(Input::Buttons mouseButton);
	void MouseMove(const IntVector2& screenSpaceMousePos, const IntVector2& clientMousePos);
	void SetGamepadInformation(const Input::GamepadStates& states, const Input::ActiveGamepads& activeGamepads);

	void CopyKeyboardMouseInput(Input::StateMap& inputMap) const;
	Input::ModifierFlags GetModifierFlags() const;
	void CopyGamepadStates(Input::GamepadStates& gamepads, Input::ActiveGamepads& activeGPs) const;

	// TEMP
	void PrepInputForFrame();
	bool IsPressed(Input::Buttons key);
	bool IsDown(Input::Buttons key);
	IntVector2 GetMousePosition() const;
	// END TEMP

private:
// 	template <typename Func>
// 	InputEvents ProcessInputReceivers(Func&& receiverFunc)
// 	{
// 		InputEvents events;
// 		for(i32 i = (i32)inputReceivers.Size() - 1; i >= 0 && !events.IsInputHandled(); --i)
// 		{
// 			events = receiverFunc(inputReceivers[i]);
// 			HandleInputEvents(events);
// 		}
// 
// 		return events;
// 	}

//	void HandleInputEvents(const InputEvents& events);

private:
	Input::GamepadStates gamepads;
	Input::ActiveGamepads activeGamepads;
	u32 totalGamepads = 0;
	Input::StateMap inputMap;
	Input::ModifierFlags modFlags;
	IntVector2 mousePosition;
	MusaApp& application;
};
