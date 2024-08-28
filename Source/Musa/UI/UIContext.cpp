// Copyright 2020, Nathan Blane

#include "UIContext.hpp"
#include "Input/Input.hpp"
#include "UIWidget.hpp"
#include "Window/Window.h"
#include "Input/ApplicationInputMap.hpp"

namespace UI
{
Context::Context(u32 screenDimensionX, u32 screenDimensionY)
	: screenWidth(screenDimensionX),
	screenHeight(screenDimensionY)
{
}
void Context::AddWidget(Widget& widget)
{
	widgets.AddUnique(&widget);
	//WindowInputHandler& inputHandler = window->GetInputHandler();
	//inputHandler.AddInput(widget);
}

void Context::RemoveWidget(Widget & widget)
{
	for (u32 i = 0; i < widgets.Size(); ++i)
	{
		if (widgets[i] == &widget)
		{
			widgets.Remove(i);
			break;
		}
	}
}

void Context::Update()
{
	for (auto& element : widgets)
	{
		element->Update(Vector2::Zero, Vector2::One);
	}
}

void Context::PrepareUIForRender(WidgetBatchElements& widgetElements)
{
	for (auto& element : widgets)
	{
		element->PrepareForRender(widgetElements);
	}
}

Path Context::GetUIPathAt(u32 screenPosX, u32 screenPosY) const
{
	UNUSED(screenPosY, screenPosX);
	return Path();
}
}