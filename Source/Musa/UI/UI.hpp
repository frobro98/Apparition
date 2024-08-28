// Copyright 2020, Nathan Blane

#pragma once

#include <utility>

namespace UI
{
struct Widget;

void AddWidgetToView(Widget& widget);
void RemoveWidgetFromView(Widget& widget);

template<class WidgetType, typename ...Args>
WidgetType* MakeWidget(Args&&... args)
{
	return new WidgetType(FORWARD(Args, args)...);
}

}