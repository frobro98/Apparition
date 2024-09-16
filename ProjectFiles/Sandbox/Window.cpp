
#include "Window.h"

#include "Debugging/Assertion.hpp"

WALL_WRN_PUSH
#include <dwmapi.h>
#include <ShlObj.h>
WALL_WRN_POP

//----------------------------------------------------------------
// Basic Window callback
LRESULT CALLBACK WindowCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//MusaApp* application = (MusaApp*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	//if (application != nullptr)
	{
		/*ApplicationEventRouter& eventRouter = application->GetInputRouter();
		ApplicationInputMap& inputMap = application->GetInputMap();*/

		switch (message)
		{
		case WM_ACTIVATEAPP:
		{
			// TODO - Cursor doesn't correctly reset its clip when deactivated and reactivated. 

			// Deactivating the app allows for the mouse to move freely again. This is done because the app isn't "active"
			// Because of this, most of the input behavior won't happen, like setting the position to be the middle of the screen 
			// or whatever every frame. This is the main thing that's prevented the mouse from moving outside of the engine....

			/*bool activated = wParam;
			if (activated)
			{
				MUSA_DEBUG(Windows, "Windows App Activated");
			}
			else
			{
				MUSA_DEBUG(Windows, "Windows App Deactivated");
			}

			eventRouter.HandleWindowActivationChanged(activated);*/
		}break;

		case  WM_CLOSE:
		{
			//MUSA_DEBUG(Windows, "Window Close Event");

			//eventRouter.HandleWindowCloseEvent();
			return 0;
		}break;

		case WM_DESTROY:
		{
			//MUSA_DEBUG(Windows, "Window Destroy Event");
			return 0;
		}break;

		case WM_SIZE:
		{
			/*MUSA_DEBUG(Windows, "Window Resize Event");

			u32 width = LOWORD(lParam);
			u32 height = HIWORD(lParam);
			eventRouter.HandleWindowResizeEvent(IntVector2(width, height));*/

			return 0;
		}break;

		case WM_SETCURSOR:
		{
			// If the DefWindowProc processes this, it doesn't update the cursor for some reason
			return 1;
		}

		case WM_MOUSEWHEEL:
		{
			/*const SHORT scrollDelta = GET_WHEEL_DELTA_WPARAM(wParam);

			const IntVector2 mousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			eventRouter.HandleMouseScrollWheel(mousePos, (f32)(scrollDelta) / WHEEL_DELTA);*/
			return 1;
		}break;

		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_XBUTTONDOWN:
		{
			/*Input::Buttons mouseButton = GetMouseType(message, wParam);
			Input::DownState buttonState = inputMap.MouseDown(mouseButton);
			eventRouter.HandleMouseDown(mouseButton, buttonState);*/

			return 0;
		}break;

		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		case WM_XBUTTONUP:
		{
			/*Input::Buttons mouseButton = GetMouseType(message, wParam);
			Input::DownState buttonState = inputMap.MouseUp(mouseButton);
			eventRouter.HandleMouseUp(mouseButton, buttonState);*/

			return 0;
		}break;

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			/*wParam = MapWparamLeftRightKeys(wParam, lParam);
			u32 vkCode = LOWORD(wParam);

			bool repeated = (lParam & 0x40000000) != 0;

			Input::Buttons input = ConvertWin32ToMusaInput(vkCode);
			Assert(input != Input::_INPUT_ENUM_MAX_);

			if (input == Input::Key_Escape)
			{
				application->CloseWindow();
			}

			Input::DownState buttonState = inputMap.KeyDown(input);
			eventRouter.HandleKeyDown(input, buttonState, repeated);*/

			// NOTE - This is commented to allow processing of things like alt+f4
			//return 0;
		}break;

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			/*wParam = MapWparamLeftRightKeys(wParam, lParam);
			u32 vkCode = LOWORD(wParam);

			Input::Buttons input = ConvertWin32ToMusaInput(vkCode);
			Assert(input != Input::_INPUT_ENUM_MAX_);

			Input::DownState buttonState = inputMap.KeyUp(input);
			eventRouter.HandleKeyUp(input, buttonState);*/

			return 0;
		}break;

		case WM_CHAR:
		{
			// TODO - Support Unicode characters!
			//char c = (tchar)wParam;

			//// 30th bit containing the prev state of the character...
			//bool repeated = (lParam & 0x40000000) != 0;

			//eventRouter.HandleKeyChar(c, repeated);

			return 0;
		}break;

		// According to multiple sources, this kind of mouse movement message when the mouse is hidden because:
		//   1) It has more resolution for mouse movement
		//   2) It essentially only has delta information
		// Because of these reasons, I will be using this every time the mouse is hidden on Windows
		case WM_INPUT:
		{
			//UINT size;
			//::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

			//MemoryBuffer riData(size);
			//if (::GetRawInputData((HRAWINPUT)lParam, RID_INPUT, riData.GetData(), &size, sizeof(RAWINPUTHEADER)) == size)
			//{
			//	const RAWINPUT* rawInput = reinterpret_cast<RAWINPUT*>(riData.GetData());

			//	if (rawInput->header.dwType == RIM_TYPEMOUSE)
			//	{
			//		POINT screenSpaceCursor;
			//		::GetCursorPos(&screenSpaceCursor);
			//		// TODO - Make this a function on the Win32 side of things
			//		POINT transformedPos = screenSpaceCursor;
			//		::ScreenToClient(hwnd, &transformedPos);

			//		IntVector2 currentScreenSpacePosition(screenSpaceCursor.x, screenSpaceCursor.y);
			//		IntVector2 currentClientPosition(transformedPos.x, transformedPos.y);
			//		IntVector2 deltaPosition(rawInput->data.mouse.lLastX, rawInput->data.mouse.lLastY);

			//		inputMap.MouseMove(currentScreenSpacePosition, currentClientPosition);
			//		eventRouter.HandleRawMouseMove(currentScreenSpacePosition, currentClientPosition, deltaPosition);
			//	}
			//}

			return 1;
		}break;

		case WM_MOUSEMOVE:
		{
			//POINT screenSpaceCursor;
			//::GetCursorPos(&screenSpaceCursor);
			//// TODO - Make this a function on the Win32 side of things
			//POINT transformedPos = screenSpaceCursor;
			//::ScreenToClient(hwnd, &transformedPos);

			//IntVector2 currentScreenSpacePosition(screenSpaceCursor.x, screenSpaceCursor.y);
			//IntVector2 currentClientPosition(transformedPos.x, transformedPos.y);

			//inputMap.MouseMove(currentScreenSpacePosition, currentClientPosition);
			//eventRouter.HandleMouseMove(currentScreenSpacePosition, currentClientPosition);

			return 0;

		}break;

		default:
		{
			//MUSA_DEBUG(Windows, "Event {}", message);
		} break;
		}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
//----------------------------------------------------------------

Window* CreateSandboxWindow(void* instance, i32 xPos, i32 yPos, i32 width, i32 height)
{
	static bool windowClassInit = false;
	if (!windowClassInit)
	{
		//instance = (HINSTANCE)GetModuleHandle(nullptr);

		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = WindowCallback;
		windowClass.hInstance = (HINSTANCE)instance;
		// TODO - Store this somewhere that can be accessed by the window that's created
		windowClass.lpszClassName = TEXT("Sandbox");
		windowClass.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClassEx(&windowClass))
		{
			Assert(false);
		}

		//InitializeWindowsInputArray();

		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
	}


	Window* window = new Window;

	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD exStyle = 0;
	window->windowHandle = CreateWindowEx(exStyle,
		// TODO - Make the window class name some shared variable that the WindowsApp controls and everything else knows about or something...
		TEXT("Sandbox"), TEXT("Apparition Sandbox Window"),
		style,
		xPos, yPos,
		width, height,
		nullptr, nullptr, (HINSTANCE)instance, nullptr);

	if (!window->windowHandle)
	{
		Assert(false);
	}

	window->posX = xPos;
	window->posY = yPos;
	window->width = width;
	window->height = height;
	//SetWindowLongPtr((HWND)hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&application));


	RECT windowRect;
	GetWindowRect((HWND)window->windowHandle, &windowRect);


	return window;
}
