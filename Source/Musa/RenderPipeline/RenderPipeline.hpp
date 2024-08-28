// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"

struct RenderView;
class Viewport;
class ScreenView;
class RenderObjectManager;

namespace UI
{
class Context;
}

namespace Musa
{
class RenderPipeline
{
public:
	RenderPipeline(Viewport& vp);

	void Initialize();
	void Execute(ScreenView& screenView, UI::Context& uiContext);

	void HandOffRenderViews(DynamicArray<RenderView*>&& renderViews);

private:
	void ScaleViewportForViews();

private:
	Viewport* viewport;
	DynamicArray<RenderView*> renderViews;
};

RenderPipeline& GetRenderPipeline();
}
