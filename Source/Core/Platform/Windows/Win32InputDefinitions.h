// Copyright 2020, Nathan Blane

#pragma once

enum class KeyInput
{
	Key_Backspace = 8,
	Key_Tab = 9,
	Key_Enter = 13,
	Key_Shift = 16,
	Key_Control = 17,
	Key_Alt = 18,
	Key_Pause = 19,
	Key_CapsLock = 20,
	Key_Escape = 27,
	Key_Space = 32,
	Key_PageUp = 33,
	Key_PageDown = 34,
	Key_End = 35,
	Key_Home = 36,
	Key_ArrowLeft = 37,
	Key_ArrowUp = 38,
	Key_ArrowRight = 39,
	Key_ArrowDown = 40,
	Key_PrintScreen = 44,
	Key_Insert = 45,
	Key_Delete = 46,
	
	/*Number keys*/
	Key_0 = 48,
	Key_1 = 49,
	Key_2 = 50,
	Key_3 = 51,
	Key_4 = 52,
	Key_5 = 53,
	Key_6 = 54,
	Key_7 = 55,
	Key_8 = 56,
	Key_9 = 57,
	
	/*Letter Keys*/
	Key_A = 65,
	Key_B = 66,
	Key_C = 67,
	Key_D = 68,
	Key_E = 69,
	Key_F = 70,
	Key_G = 71,
	Key_H = 72,
	Key_I = 73,
	Key_J = 74,
	Key_K = 75,
	Key_L = 76,
	Key_M = 77,
	Key_N = 78,
	Key_O = 79,
	Key_P = 80,
	Key_Q = 81,
	Key_R = 82,
	Key_S = 83,
	Key_T = 84,
	Key_U = 85,
	Key_V = 86,
	Key_W = 87,
	Key_X = 88,
	Key_Y = 89,
	Key_Z = 90,

	/* NumPad */
	Key_Num0 = 96,
	Key_Num1 = 97,
	Key_Num2 = 98,
	Key_Num3 = 99,
	Key_Num4 = 100,
	Key_Num5 = 101,
	Key_Num6 = 102,
	Key_Num7 = 103,
	Key_Num8 = 104,
	Key_Num9 = 105,
	Key_NumMultiply = 106,
	Key_NumAdd = 107,
	Key_NumSubtract = 109,
	Key_NumDecimal = 110,
	Key_NumDivide = 111,
// 	Key_NumEnter = 335,
// 	Key_NumEqual = 336,

	Key_F1 = 112,
	Key_F2 = 113,
	Key_F3 = 114,
	Key_F4 = 115,
	Key_F5 = 116,
	Key_F6 = 117,
	Key_F7 = 118,
	Key_F8 = 119,
	Key_F9 = 120,
	Key_F10 = 121,
	Key_F11 = 122,
	Key_F12 = 123,

	/* Func keys */
	Key_NumLock = 144,
	Key_ScrollLock = 145,

	Key_LeftShift = 160,
	Key_RightShift = 161,
	Key_LeftControl = 162,
	Key_RightControl = 163,
	Key_LeftAlt = 164,
	Key_RightAlt = 165,

	Key_Semicolon = 186,  /* ; */
	Key_Equal = 187,  /* = */
	Key_Comma = 188,  /* , */
	Key_Minus = 189,  /* - */
	Key_Period = 190,  /* . */
	Key_ForwardSlash = 191,  /* / */
	Key_Tilde = 192,  /* ` */
	Key_LeftBracket = 219,  /* [ */
	Key_BackSlash = 220,  /* \ */
	Key_RightBracket = 221,  /* ] */
	Key_Apostrophe = 222,  /* ' */
	

	Key_Max = 222
};

enum class KeyboardState
{
	State_Shift = 0x0001,
	State_Control = 0x0002,
	State_Alt = 0x0004
};

enum class MouseInput
{
	Mouse_LeftButton = 1,
	Mouse_RightButton = 2,
	Mouse_MiddleButton = 4,
	Mouse_Button4,
	Mouse_Button5,
	Mouse_Button6,
	Mouse_Button7,
	Mouse_Button8
};

enum KeyStateFlags
{
	KeyState_Pressed = 0,
	KeyState_Released,
	KeyState_Hold,
	KeyState_Max,
	KeyState_MK_4_BYTES = 0x70000000
};

static_assert(
	((KeyStateFlags::KeyState_Hold) - (KeyStateFlags::KeyState_Pressed)) == (KeyStateFlags::KeyState_Max) - 1,
	"Total amount of KeyStateFlags are incorrect!"
);

