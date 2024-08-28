// Copyright 2020, Nathan Blane

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Platform/PlatformDefinitions.h"
#include "Debugging/Assertion.hpp"
#include "Window.h"
#include "Input/InputDefinitions.hpp"
#include "Input/Internal/InputInternal.hpp"


Window::Window(void* instance, MusaApp& app, u32 xPos, u32 yPos, u32 w, u32 h)
	: hwnd(nullptr),
	application(app),
	position(xPos, yPos),
	width(w),
	height(h),
	isShown(false)
{


	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD exStyle = 0;
	hwnd = CreateWindowEx(exStyle,
		// TODO - Make the window class name some shared variable that the WindowsApp controls and everything else knows about or something...
		TEXT("MusaEngine"), TEXT("Musa Engine Window"),
		style,
		xPos, yPos,
		width, height,
		nullptr, nullptr, (HINSTANCE)instance, nullptr);

	if (!hwnd)
	{
		Assert(false);
	}

	SetWindowLongPtr((HWND)hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&application));


	RECT windowRect;
	GetWindowRect((HWND)hwnd, &windowRect);

	Show();
//	ShowCursor(FALSE);

// 	int32 originX = windowRect.left;
// 	int32 originY = windowRect.top;
// 	RECT cursorRect = {};
// 	cursorRect.left = originX;
// 	cursorRect.top = originY;
// 	cursorRect.right = width;
// 	cursorRect.bottom = height;
// 	ClipCursor(&cursorRect);
// 
// 	int32 centerX = originX + (width / 2);
// 	int32 centerY = originY + (height / 2);
// 	SetCursorPos(centerX, centerY);

	isActive = true;

	// 	if (newWindowMode == WindowMode::Fullscreen)
	// 	{
	// 		SetWindowMode(windowMode);
	// 	}
	// 	else
	// 	{
	// 		windowMode = WindowMode::Windowed;
	// 	}
}

Window::~Window()
{
	Assert(hwnd != nullptr);
	DestroyWindow((HWND)hwnd);
}

void Window::Close()
{
	if (isActive)
	{
		isActive = false;
	}
}

void Window::Resize(u32 w, u32 h)
{
	width = w;
	height = h;
}

void Window::Show()
{
	if (!isShown)
	{
		ShowWindow((HWND)hwnd, SWP_SHOWWINDOW);
		isShown = true;
	}
}

void Window::Hide()
{
	if (isShown)
	{
		ShowWindow((HWND)hwnd, SW_HIDE);
		isShown = false;
	}
}

void Window::SetWindowMode(WindowMode newWindowMode)
{
	if (windowMode != newWindowMode)
	{
		windowMode = newWindowMode;

		LONG currentStyle = GetWindowLong((HWND)hwnd, GWL_STYLE);
		DWORD windowedStyle = WS_OVERLAPPEDWINDOW;
		DWORD fullScreenStyle = WS_POPUP;

		if (newWindowMode == WindowMode::Fullscreen)
		{
			currentStyle &= ~windowedStyle;
			currentStyle |= fullScreenStyle;

			//GetWindowPlacement((HWND)hwnd, &previousPlacement);

			SetWindowLong((HWND)hwnd, GWL_STYLE, currentStyle);
			//UINT flags = SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOCOPYBITS;

			SetWindowPos((HWND)hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

			HMONITOR monitor = MonitorFromWindow((HWND)hwnd, MONITOR_DEFAULTTOPRIMARY /*MONITOR_DEFAULTTONDEAREST (BorderedFullscreen)*/);
			MONITORINFO monInfo = {};
			monInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(monitor, &monInfo);

			width = monInfo.rcMonitor.right - monInfo.rcMonitor.left;
			height = monInfo.rcMonitor.bottom - monInfo.rcMonitor.top;
			LONG x = monInfo.rcMonitor.left;
			LONG y = monInfo.rcMonitor.top;

			SetWindowPos((HWND)hwnd, nullptr, x, y, width, height, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING);

			//ShowWindow(hWnd, SW_RESTORE);
		}
		else
		{
			currentStyle &= ~fullScreenStyle;
			currentStyle |= windowedStyle;
			SetWindowLong((HWND)hwnd, GWL_STYLE, currentStyle);
			SetWindowPos((HWND)hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
			//SetWindowPlacement((HWND)hwnd, &previousPlacement);
		}
	}
}

void Window::SetAsActiveWindow()
{
}

bool Window::IsActive() const
{
	return isActive;
}

void* Window::GetWindowHandle() const
{
	return hwnd;
}
