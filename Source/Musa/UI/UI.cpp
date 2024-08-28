// Copyright 2020, Nathan Blane

#include "UI.hpp"
#include "UIInternal.hpp"
#include "UIContext.hpp"

namespace UI
{
namespace Internal
{
static Context* context = nullptr;

void SetMusaUI(Context& uiContext)
{
	context = &uiContext;
}
}

void AddWidgetToView(Widget& widget)
{
	Internal::context->AddWidget(widget);
}

void RemoveWidgetFromView(Widget& widget)
{
	Internal::context->RemoveWidget(widget);
}

}