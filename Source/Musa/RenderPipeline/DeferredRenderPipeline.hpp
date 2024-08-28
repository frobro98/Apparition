// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"

struct View;
struct RenderView;
struct FrameRenderTargets;
class RenderContext;
struct RenderTarget;

namespace DeferredRender
{
void Render(RenderContext& renderContext, const DynamicArray<RenderView*>& views, const FrameRenderTargets& frameTargets);
void ComposeBackbuffer(RenderContext& context, const RenderTarget& sceneColor, const RenderTarget& uiColor, const View& view);
}


