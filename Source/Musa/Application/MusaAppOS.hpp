// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/UniquePtr.hpp"
#include "Math/Rect.hpp"

class ApplicationInputMap;
class ApplicationEventRouter;
class Window;
struct IntVector2;

// TODO - Consider making this just an interface
class MusaAppOS
{
public:
	MusaAppOS() = default;
	virtual ~MusaAppOS() = default;

	virtual Window* CreateGameWindow(u32 xPos, u32 yPos, u32 width, u32 height) = 0;
	
	virtual void SetRawMouseInput(bool enabled, const Window& window) = 0;

	virtual void ShowCursor(bool showCursor) = 0;
	virtual void SetMousePosition(const IntVector2& mousePos) = 0;
	virtual IntVector2 GetMousePosition() const = 0;
	virtual IntVector2 TransformPositionToWindow(const Window& window, const IntVector2& pos) const = 0;
	virtual void LockCursorToRect(const Recti& rect) = 0;
	virtual void UnlockCursorFromRect() = 0;

	virtual void ProcessNativeGamepad(ApplicationInputMap& inputMap, ApplicationEventRouter& inputDispatcher) = 0;

	virtual void ProcessInputEvents(ApplicationInputMap& inputMap) = 0;
};