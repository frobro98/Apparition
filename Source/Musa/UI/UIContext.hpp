// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "UI/UIPath.hpp"

class Window;
class WidgetBatchElements;

namespace UI
{

struct Widget;

class Context
{
public:
	// Either needs to take in the input handler or whoever "adds" UI must also add it to the input handling
	Context(u32 screenDimensionX, u32 screenDimensionY);

	inline void SetNewDimensions(u32 screenX, u32 screenY)
	{
		screenWidth = screenX;
		screenHeight = screenY;
	}

	inline void SetWindow(Window& win) { window = &win; }
	inline Window* GetWindow() const { return window; }

	// Adds widget tree to the context
	void AddWidget(Widget& widget);
	void RemoveWidget(Widget& widget);

	// Updates all of the UI within the tree. Probably pushes all of the info to the render side of things as well...
	void Update();

	void PrepareUIForRender(WidgetBatchElements& widgetElements);

	// Returns a ui tree path to the widget under
	Path GetUIPathAt(u32 screenPosX, u32 screenPosY) const;

private:
	// Tree of widgets
	DynamicArray<Widget*> widgets;
	Window* window = nullptr;
	u32 screenWidth;
	u32 screenHeight;
};

}
