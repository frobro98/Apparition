// Copyright 2020, Nathan Blane

#pragma once


namespace Input
{
	enum Buttons
	{
		Key_A,
		Key_B,
		Key_C,
		Key_D,
		Key_E,
		Key_F,
		Key_G,
		Key_H,
		Key_I,
		Key_J,
		Key_K,
		Key_L,
		Key_M,
		Key_N,
		Key_O,
		Key_P,
		Key_Q,
		Key_R,
		Key_S,
		Key_T,
		Key_U,
		Key_V,
		Key_W,
		Key_X,
		Key_Y,
		Key_Z,

		Key_0,
		Key_1,
		Key_2,
		Key_3,
		Key_4,
		Key_5,
		Key_6,
		Key_7,
		Key_8,
		Key_9,

		Key_Tilde,
		Key_Semicolon,
		Key_Comma,
		Key_Period,
		Key_ForwardSlash,
		Key_Apostrophe,
		Key_LeftBracket,
		Key_RightBracket,
		Key_Backslash,
		Key_Equal,
		Key_Minus,


		Key_Enter,
		Key_Backspace,
		Key_Escape,
		Key_Tab,
		Key_Capslock,
		Key_LeftShift,
		Key_LeftControl,
		Key_LeftAlt,
		Key_Space,
		Key_RightAlt,
		Key_RightControl,
		Key_RightShift,

		Key_PrintScreen,
		Key_ScrollLock,
		Key_Pause,
		Key_Insert,
		Key_Delete,
		Key_Home,
		Key_End,
		Key_PageUp,
		Key_PageDown,
		Key_ArrowUp,
		Key_ArrowDown,
		Key_ArrowLeft,
		Key_ArrowRight,

		Key_NumLock,
		Key_Num0,
		Key_Num1,
		Key_Num2,
		Key_Num3,
		Key_Num4,
		Key_Num5,
		Key_Num6,
		Key_Num7,
		Key_Num8,
		Key_Num9,
		Key_NumEnter,
		Key_NumPlus,
		Key_NumMinus,
		Key_NumMulti,
		Key_NumDivide,
		Key_NumDecimal,

		Key_F1,
		Key_F2,
		Key_F3,
		Key_F4,
		Key_F5,
		Key_F6,
		Key_F7,
		Key_F8,
		Key_F9,
		Key_F10,
		Key_F11,
		Key_F12,

		// Mouse Input
		Mouse_LeftButton,
		Mouse_RightButton,
		Mouse_MiddleButton,
		Mouse_Button4,
		Mouse_Button5,
		Mouse_Button6,
		Mouse_Button7,
		Mouse_Button8,
		Mouse_ScrollWheel,

		Mouse_XAxis,
		Mouse_YAxis,

		// Gamepad Input
		Gamepad_AButton,
		Gamepad_BButton,
		Gamepad_XButton,
		Gamepad_YButton,

		Gamepad_StartButton,
		Gamepad_SelectButton,

		Gamepad_DPadUp,
		Gamepad_DPadDown,
		Gamepad_DPadLeft,
		Gamepad_DPadRight,

		Gamepad_LeftShoulder,
		Gamepad_RightShoulder,
		Gamepad_LeftTrigger,
		Gamepad_RightTrigger,

		Gamepad_LeftStick_XAxis,
		Gamepad_LeftStick_YAxis,
		Gamepad_RightStick_XAxis,
		Gamepad_RightStick_YAxis,

		KM_Max = Mouse_Button8 + 1,
		GP_Max = Gamepad_RightStick_YAxis - Gamepad_AButton + 1,

		_INPUT_ENUM_MAX_ = 0x7FFFFFFF
	};
	// NOTE - These checks are one less than the total number of enums for each group tested
	static_assert(Key_Z - Key_A == 25, "Looping through the alpha keys won't work!");
	static_assert(Key_9 - Key_0 == 9, "Looping through the number keys won't work!");
	static_assert(Key_Num9 - Key_Num0 == 9, "Looping through the numpad keys won't work!");
	static_assert(Key_F12 - Key_F1 == 11, "Looping through the func keys won't work!");
	static_assert(KM_Max == Mouse_Button8 + 1);
	static_assert(GP_Max == Gamepad_RightStick_YAxis - Gamepad_AButton + 1);

	enum ModifierFlags
	{
		Shift = 0x1,
		Control = 0x2,
		Alt = 0x4,
		Command = 0x8,

		_INPUT_STATE_MAX_ = 0xFF
	};


	inline bool IsMouseButtonEvent(Buttons button) { return button <= Mouse_Button8 && button >= Mouse_LeftButton; }
};

