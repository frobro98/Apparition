// Copyright 2020, Nathan Blane

#include "CoreFlags.hpp"

WALL_WRN_PUSH
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <xinput.h>
#include <windowsx.h>
WALL_WRN_POP

#include "Platform/PlatformDefinitions.h"
#include "Musa.hpp"
#include "MusaAppWindows.hpp"
#include "Application/MusaApp.hpp"
#include "Window/Window.h"
#include "Debugging/MetricInterface.hpp"
#include "Input/Internal/InputInternal.hpp"
#include "Input/InputDefinitions.hpp"
#include "Input/Internal/ControllerInputUtilities.hpp"
#include "Math/MathFunctions.hpp"
#include "Containers/MemoryBuffer.hpp"
#include "Logging/LogFunctions.hpp"

DEFINE_LOG_CHANNEL(Windows);

DECLARE_METRIC_GROUP(WindowsInput);
METRIC_STAT(PumpMessages, WindowsInput);

constexpr u32 WindowsKeyInputCount = 0x20c;
static StaticArray<Input::Buttons, WindowsKeyInputCount> windowsInputs;

static void InitializeWindowsInputArray()
{
	for (auto& input : windowsInputs)
	{
		input = Input::_INPUT_ENUM_MAX_;
	}

	// Store 0-9 keys
	constexpr u32 key0 = 0x30;
	for (u32 i = 0; i < 10; ++i)
	{
		windowsInputs[key0 + i] = (Input::Buttons)(Input::Key_0 + i);
	}

	// Store A-Z keys
	constexpr u32 keyA = 0x41;
	for (u32 i = 0; i < 26; ++i)
	{
		windowsInputs[keyA + i] = (Input::Buttons)(Input::Key_A + i);
	}

	// Store Mouse Button inputs
	windowsInputs[VK_LBUTTON] = Input::Mouse_LeftButton;
	windowsInputs[VK_RBUTTON] = Input::Mouse_RightButton;
	windowsInputs[VK_MBUTTON] = Input::Mouse_MiddleButton;
	windowsInputs[VK_XBUTTON1] = Input::Mouse_Button4;
	windowsInputs[VK_XBUTTON2] = Input::Mouse_Button5;

	// Store Alt, Shift, Control, Caps, Tab, Esc
	windowsInputs[VK_LSHIFT] = Input::Key_LeftShift;
	windowsInputs[VK_RSHIFT] = Input::Key_RightShift;
	windowsInputs[VK_LCONTROL] = Input::Key_LeftControl;
	windowsInputs[VK_RCONTROL] = Input::Key_RightControl;
	windowsInputs[VK_LMENU] = Input::Key_LeftAlt;
	windowsInputs[VK_RMENU] = Input::Key_RightAlt;
	windowsInputs[VK_CAPITAL] = Input::Key_Capslock;
	windowsInputs[VK_SPACE] = Input::Key_Space;
	windowsInputs[VK_TAB] = Input::Key_Tab;
	windowsInputs[VK_ESCAPE] = Input::Key_Escape;

	// Store punctuation
	windowsInputs[186] = Input::Key_Semicolon;
	windowsInputs[187] = Input::Key_Equal;
	windowsInputs[188] = Input::Key_Comma;
	windowsInputs[189] = Input::Key_Minus;
	windowsInputs[190] = Input::Key_Period;
	windowsInputs[191] = Input::Key_ForwardSlash;
	windowsInputs[192] = Input::Key_Tilde;
	windowsInputs[219] = Input::Key_LeftBracket;
	windowsInputs[220] = Input::Key_Backslash;
	windowsInputs[221] = Input::Key_RightBracket;
	windowsInputs[222] = Input::Key_Apostrophe;

	// Store Function keys
	for (u32 i = 0; i < 12; ++i)
	{
		windowsInputs[VK_F1 + i] = (Input::Buttons)(Input::Key_F1 + i);
	}

	// Store Arrow keys
	windowsInputs[VK_LEFT] = Input::Key_ArrowLeft;
	windowsInputs[VK_RIGHT] = Input::Key_ArrowRight;
	windowsInputs[VK_UP] = Input::Key_ArrowUp;
	windowsInputs[VK_DOWN] = Input::Key_ArrowDown;

	// Store Input keys
	windowsInputs[VK_RETURN] = Input::Key_Enter;
	windowsInputs[VK_BACK] = Input::Key_Backspace;
	windowsInputs[VK_PRIOR] = Input::Key_PageUp;
	windowsInputs[VK_NEXT] = Input::Key_PageDown;
	windowsInputs[VK_INSERT] = Input::Key_Insert;
	windowsInputs[VK_DELETE] = Input::Key_Delete;
	windowsInputs[VK_HOME] = Input::Key_Home;
	windowsInputs[VK_END] = Input::Key_End;

	// Store numpad keys
	windowsInputs[VK_NUMLOCK] = Input::Key_NumLock;
	for (u32 i = 0; i < 10; ++i)
	{
		windowsInputs[VK_NUMPAD0 + i] = (Input::Buttons)(Input::Key_Num0 + i);
	}
	windowsInputs[VK_MULTIPLY] = Input::Key_NumMulti;
	windowsInputs[VK_ADD] = Input::Key_NumPlus;
	windowsInputs[VK_SUBTRACT] = Input::Key_NumMinus;
	windowsInputs[VK_DECIMAL] = Input::Key_NumDecimal;
	windowsInputs[VK_DIVIDE] = Input::Key_NumDivide;

	// Store Unique keys
	windowsInputs[VK_SNAPSHOT] = Input::Key_PrintScreen;
	windowsInputs[VK_SCROLL] = Input::Key_ScrollLock;
	windowsInputs[VK_PAUSE] = Input::Key_Pause;

}

WPARAM MapWparamLeftRightKeys(WPARAM wparam, LPARAM lparam)
{
	u32 scanCode = (lparam & 0x00ff0000) >> 16;
	bool extended = (lparam & 0x01000000) != 0;
	switch (wparam)
	{
		case VK_SHIFT:
			return MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
	case VK_CONTROL:
		return extended ? VK_RCONTROL : VK_LCONTROL;
	case VK_MENU:
		return extended ? VK_RMENU : VK_LMENU;
	default:
		return wparam;
	}
}

Input::Buttons ConvertWin32ToMusaInput(u32 vkCode)
{
	// TODO - this could be cached to be a static array of initialized vk codes
	return windowsInputs[vkCode];
}

inline Input::Buttons GetMouseType(UINT message, WPARAM wParam)
{
	switch (message)
	{
	case WM_LBUTTONDOWN: return Input::Mouse_LeftButton;
	case WM_RBUTTONDOWN: return Input::Mouse_RightButton;
	case WM_MBUTTONDOWN: return Input::Mouse_MiddleButton;
	case WM_LBUTTONUP: return Input::Mouse_LeftButton;
	case WM_RBUTTONUP: return Input::Mouse_RightButton;
	case WM_MBUTTONUP: return Input::Mouse_MiddleButton;
	case WM_MOUSEWHEEL: return Input::Mouse_ScrollWheel;

	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	{
		return (HIWORD(wParam) & XBUTTON1) ? Input::Mouse_Button4 : Input::Mouse_Button5;
	}

	default:
		return Input::_INPUT_ENUM_MAX_;
	}
}

// Basic Window callback
LRESULT CALLBACK WindowCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MusaApp* application = (MusaApp*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if(application != nullptr)
	{
		ApplicationEventRouter& eventRouter = application->GetInputRouter();
		ApplicationInputMap& inputMap = application->GetInputMap();

		switch (message)
		{
			case WM_ACTIVATEAPP:
			{
				// TODO - Cursor doesn't correctly reset its clip when deactivated and reactivated. 

				// Deactivating the app allows for the mouse to move freely again. This is done because the app isn't "active"
				// Because of this, most of the input behavior won't happen, like setting the position to be the middle of the screen 
				// or whatever every frame. This is the main thing that's prevented the mouse from moving outside of the engine....

				bool activated = wParam;
				if (activated)
				{
					MUSA_DEBUG(Windows, "Windows App Activated");
				}
				else
				{
					MUSA_DEBUG(Windows, "Windows App Deactivated");
				}

				eventRouter.HandleWindowActivationChanged(activated);
			}break;

			case  WM_CLOSE:
			{
				MUSA_DEBUG(Windows, "Window Close Event");

				eventRouter.HandleWindowCloseEvent();
				return 0;
			}break;

			case WM_DESTROY:
			{
				MUSA_DEBUG(Windows, "Window Destroy Event");
				return 0;
			}break;

			case WM_SIZE:
			{
				MUSA_DEBUG(Windows, "Window Resize Event");

				u32 width = LOWORD(lParam);
				u32 height = HIWORD(lParam);
				eventRouter.HandleWindowResizeEvent(IntVector2(width, height));

				return 0;
			}break;

			case WM_SETCURSOR:
			{
				// If the DefWindowProc processes this, it doesn't update the cursor for some reason
				return 1;
			}

			case WM_MOUSEWHEEL:
			{
				const SHORT scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);

				const IntVector2 mousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				eventRouter.HandleMouseScrollWheel(mousePos, (f32)(scrollDelta) / WHEEL_DELTA);
				return 1;
			}break;

			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_XBUTTONDOWN:
			{
				Input::Buttons mouseButton = GetMouseType(message, wParam);
				Input::DownState buttonState = inputMap.MouseDown(mouseButton);
				eventRouter.HandleMouseDown(mouseButton, buttonState);

				return 0;
			}break;

			case WM_LBUTTONUP:
			case WM_RBUTTONUP:
			case WM_MBUTTONUP:
			case WM_XBUTTONUP:
			{
				Input::Buttons mouseButton = GetMouseType(message, wParam);
				Input::DownState buttonState = inputMap.MouseUp(mouseButton);
				eventRouter.HandleMouseUp(mouseButton, buttonState);

				return 0;
			}break;

			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				wParam = MapWparamLeftRightKeys(wParam, lParam);
				u32 vkCode = LOWORD(wParam);

				bool repeated = (lParam & 0x40000000) != 0;

				Input::Buttons input = ConvertWin32ToMusaInput(vkCode);
				Assert(input != Input::_INPUT_ENUM_MAX_);

				if (input == Input::Key_Escape)
				{
					application->CloseWindow();
				}

				Input::DownState buttonState = inputMap.KeyDown(input);
				eventRouter.HandleKeyDown(input, buttonState, repeated);

				// NOTE - This is commented to allow processing of things like alt+f4
				//return 0;
			}break;

			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				wParam = MapWparamLeftRightKeys(wParam, lParam);
				u32 vkCode = LOWORD(wParam);
				
				Input::Buttons input = ConvertWin32ToMusaInput(vkCode);
				Assert(input != Input::_INPUT_ENUM_MAX_);

				Input::DownState buttonState = inputMap.KeyUp(input);
				eventRouter.HandleKeyUp(input, buttonState);

				return 0;
			}break;

			case WM_CHAR:
			{
				// TODO - Support Unicode characters!
				char c = (tchar)wParam;

				// 30th bit containing the prev state of the character...
				bool repeated = (lParam & 0x40000000) != 0;
				
				eventRouter.HandleKeyChar(c, repeated);

				return 0;
			}break;

			// According to multiple sources, this kind of mouse movement message when the mouse is hidden because:
			//   1) It has more resolution for mouse movement
			//   2) It essentially only has delta information
			// Because of these reasons, I will be using this every time the mouse is hidden on Windows
			case WM_INPUT:
			{
				UINT size;
				::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

				MemoryBuffer riData(size);
				if (::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, riData.GetData(), &size, sizeof(RAWINPUTHEADER)) == size)
				{
					const RAWINPUT* rawInput = reinterpret_cast<RAWINPUT*>(riData.GetData());

					if (rawInput->header.dwType == RIM_TYPEMOUSE)
					{
						POINT screenSpaceCursor;
						::GetCursorPos(&screenSpaceCursor);
						// TODO - Make this a function on the Win32 side of things
						POINT transformedPos = screenSpaceCursor;
						::ScreenToClient(hwnd, &transformedPos);

						IntVector2 currentScreenSpacePosition(screenSpaceCursor.x, screenSpaceCursor.y);
						IntVector2 currentClientPosition(transformedPos.x, transformedPos.y);
						IntVector2 deltaPosition(rawInput->data.mouse.lLastX, rawInput->data.mouse.lLastY);

						inputMap.MouseMove(currentScreenSpacePosition, currentClientPosition);
						eventRouter.HandleRawMouseMove(currentScreenSpacePosition, currentClientPosition, deltaPosition);
					}
				}

				return 1;
			}break;

			case WM_MOUSEMOVE:
			{
				POINT screenSpaceCursor;
				::GetCursorPos(&screenSpaceCursor);
				// TODO - Make this a function on the Win32 side of things
				POINT transformedPos = screenSpaceCursor;
				::ScreenToClient(hwnd, &transformedPos);

				IntVector2 currentScreenSpacePosition(screenSpaceCursor.x, screenSpaceCursor.y);
				IntVector2 currentClientPosition(transformedPos.x, transformedPos.y);

				inputMap.MouseMove(currentScreenSpacePosition, currentClientPosition);
				eventRouter.HandleMouseMove(currentScreenSpacePosition, currentClientPosition);

				return 0;
				
			}break;

			default:
			{
				MUSA_DEBUG(Windows, "Event {}", message);
			} break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

static const Input::Buttons ControllerButtons[] = {
	Input::Gamepad_DPadUp,
	Input::Gamepad_DPadDown,
	Input::Gamepad_DPadLeft,
	Input::Gamepad_DPadRight,
	Input::Gamepad_StartButton,
	Input::Gamepad_SelectButton,
	Input::Gamepad_LeftShoulder,
	Input::Gamepad_RightShoulder,
	Input::Gamepad_LeftTrigger,
	Input::Gamepad_RightTrigger,
	Input::Gamepad_AButton,
	Input::Gamepad_BButton,
	Input::Gamepad_XButton,
	Input::Gamepad_YButton,
};

void ProcessAnalogControllerInputs(ApplicationEventRouter& eventDispatcher, u32 controllerIndex, const XINPUT_GAMEPAD& xinputGamepad, Input::GamepadState& state)
{
	// Deadzone checking
	if (//state.leftStick.x != xinputGamepad.sThumbLX ||
		Math::Abs(xinputGamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		state.leftStick.x = NormalizeStickValue(xinputGamepad.sThumbLX);
		eventDispatcher.HandleControllerAnalogChange(controllerIndex, Input::Gamepad_LeftStick_XAxis, state.leftStick.x);
	}

	if (//state.leftStick.y != xinputGamepad.sThumbLY ||
		Math::Abs(xinputGamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		state.leftStick.y = NormalizeStickValue(xinputGamepad.sThumbLY);
		eventDispatcher.HandleControllerAnalogChange(controllerIndex, Input::Gamepad_LeftStick_YAxis, state.leftStick.y);
	}

	if (//state.rightStick.x != xinputGamepad.sThumbRX ||
		Math::Abs(xinputGamepad.sThumbRX) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state.rightStick.x = NormalizeStickValue(xinputGamepad.sThumbRX);
		eventDispatcher.HandleControllerAnalogChange(controllerIndex, Input::Gamepad_RightStick_XAxis, state.rightStick.x);
	}

	if (//state.rightStick.y != xinputGamepad.sThumbRY ||
		Math::Abs(xinputGamepad.sThumbRY) > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{
		state.rightStick.y = NormalizeStickValue(xinputGamepad.sThumbRY);
		eventDispatcher.HandleControllerAnalogChange(controllerIndex, Input::Gamepad_RightStick_YAxis, state.rightStick.y);
	}

	// Triggers for axis analog
	if (Math::Abs(xinputGamepad.bLeftTrigger) > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		state.leftTrigger = NormalizeTriggerValue(xinputGamepad.bLeftTrigger);
		eventDispatcher.HandleControllerAnalogChange(controllerIndex, Input::Gamepad_LeftTrigger, state.leftTrigger);
	}
	if (Math::Abs(xinputGamepad.bRightTrigger) > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	{
		state.rightTrigger = NormalizeTriggerValue(xinputGamepad.bRightTrigger);
		eventDispatcher.HandleControllerAnalogChange(controllerIndex, Input::Gamepad_RightTrigger, state.rightTrigger);
	}
}

void ProcessControllerButtons(ApplicationEventRouter& eventDispatcher, u32 controllerIndex, const XINPUT_GAMEPAD& xinputGamepad, Input::GamepadState& state)
{
	StaticArray<bool, Input::GP_Max> currentButtonState;
	currentButtonState[0] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0;
	currentButtonState[1] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0;
	currentButtonState[2] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) != 0;
	currentButtonState[3] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) != 0;
	currentButtonState[4] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_START) != 0;
	currentButtonState[5] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_BACK) != 0;
	currentButtonState[6] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0;
	currentButtonState[7] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0;
	currentButtonState[8] = (xinputGamepad.bLeftTrigger > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	currentButtonState[9] = (xinputGamepad.bRightTrigger > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	currentButtonState[10] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_A) != 0;
	currentButtonState[11] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_B) != 0;
	currentButtonState[12] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
	currentButtonState[13] = (xinputGamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;

	for (u32 i = 0; i < Input::GP_Max; ++i)
	{
		state.buttonStates[i].endedDown = currentButtonState[i];
		if (state.buttonStates[i].endedDown != state.buttonStates[i].previouslyDown)
		{
			if (state.buttonStates[i].endedDown)
			{
				eventDispatcher.HandleControllerButtonDown(controllerIndex, ControllerButtons[i], state.buttonStates[i]);
			}
			else
			{
				eventDispatcher.HandleControllerButtonUp(controllerIndex, ControllerButtons[i], state.buttonStates[i]);
			}
		}
		else if(state.buttonStates[i].endedDown)
		{
			eventDispatcher.HandleControllerButtonDown(controllerIndex, ControllerButtons[i], state.buttonStates[i]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// MusaAppWindows
//////////////////////////////////////////////////////////////////////////

MusaAppWindows::MusaAppWindows()
{
	MUSA_INFO(Windows, "Initializing Windows Application");

	instance = (HINSTANCE)GetModuleHandle(nullptr);

	WNDCLASSEX windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = WindowCallback;
	windowClass.hInstance = instance;
	// TODO - Store this somewhere that can be accessed by the window that's created
	windowClass.lpszClassName = TEXT("MusaEngine");
	windowClass.style = CS_HREDRAW | CS_VREDRAW;

	if (!RegisterClassEx(&windowClass))
	{
		Assert(false);
	}

	InitializeWindowsInputArray();

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

Window* MusaAppWindows::CreateGameWindow(u32 xPos, u32 yPos, u32 width, u32 height)
{
	MUSA_DEBUG(Windows, "Create Win32 Window (x: {} y: {} w: {} h: {}",
		xPos, yPos, width, height);

	Assert(gApp);
	return new Window(instance, *gApp, xPos, yPos, width, height);
}

void MusaAppWindows::SetRawMouseInput(bool enabled, const Window& window)
{
	DWORD riFlags = enabled ? 0 : RIDEV_REMOVE;
	HWND wnd = enabled ? reinterpret_cast<HWND>(window.GetWindowHandle()) : nullptr;

	constexpr u32 riMouseUsage = 2;
	RAWINPUTDEVICE riDev = {};
	riDev.dwFlags = riFlags;
	riDev.hwndTarget = wnd;
	riDev.usUsage = riMouseUsage;
	riDev.usUsagePage = 1;

	NOT_USED BOOL registered = ::RegisterRawInputDevices(&riDev, 1, sizeof(RAWINPUTDEVICE));
	Assert(registered);
}

void MusaAppWindows::ShowCursor(bool showCursor)
{
	MUSA_DEBUG(Windows, "ShowCursor: {}", showCursor);

	::ShowCursor(showCursor);
}

void MusaAppWindows::SetMousePosition(const IntVector2& mousePos)
{
	::SetCursorPos(mousePos.x, mousePos.y);
}

IntVector2 MusaAppWindows::GetMousePosition() const
{
	POINT cursorPos;
	::GetCursorPos(&cursorPos);

	return IntVector2(cursorPos.x, cursorPos.y);
}

IntVector2 MusaAppWindows::TransformPositionToWindow(const Window& window, const IntVector2& pos) const
{
	// TODO - Make this a function on the Win32 side of things
	POINT cursorPos{ pos.x, pos.y };
	::ScreenToClient((HWND)window.GetWindowHandle(), &cursorPos);
	
	return IntVector2(cursorPos.x, cursorPos.y);
}

void MusaAppWindows::LockCursorToRect(const Recti& rect)
{
	MUSA_DEBUG(Windows, "Locked Cursor to Rect x: {} y: {} w: {} h: {}",
		rect.x, rect.y, rect.width, rect.height);

	RECT r = {};
	r.left = rect.x;
	r.top = rect.y;
	r.right = rect.x + rect.width;
	r.bottom = rect.y + rect.height;
	BOOL clipped = ::ClipCursor(&r);
	MUSA_INFO(Windows, "Locked cursor to rect? {}",  clipped ? "true" : "false");
}

void MusaAppWindows::UnlockCursorFromRect()
{
	MUSA_DEBUG(Windows, "Unlocked Cursor");
	::ClipCursor(nullptr);
}

void MusaAppWindows::ProcessNativeGamepad(ApplicationInputMap& inputMap, ApplicationEventRouter& inputDispatcher)
{
	// NOTE - I could check if controllers are actually connected? Don't really know what that gets me though

	for (u32 i = 0; i < Input::MaxSupportedControllers; ++i)
	{
		XINPUT_STATE state = {};
		activeControllers[i] = XInputGetState(i, &state) == ERROR_SUCCESS;
	}

	for (u32 i = 0; i < Input::MaxSupportedControllers; ++i)
	{
		// TODO - Probably slow because it's a system call multiple times (above and here...)
		XINPUT_STATE state = {};
		if (XInputGetState(i, &state) == ERROR_SUCCESS)
		{
			Input::GamepadState& musaGamepad = controllers[i];
			const XINPUT_GAMEPAD& gamepad = state.Gamepad;

			// Stick processing
			ProcessAnalogControllerInputs(inputDispatcher, i, gamepad, musaGamepad);

			// Button processing
			ProcessControllerButtons(inputDispatcher, i, gamepad, musaGamepad);
		}
	}

	inputMap.SetGamepadInformation(controllers, activeControllers);
}

void MusaAppWindows::ProcessInputEvents(ApplicationInputMap& inputMap)
{
	inputMap.PrepInputForFrame();

	SCOPED_TIMED_BLOCK(PumpMessages);
	MSG Message;
	while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
}

