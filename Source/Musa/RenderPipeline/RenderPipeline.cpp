// Copyright 2020, Nathan Blane

#include "RenderPipeline.hpp"
#include "RenderPipeline/DeferredRenderPipeline.hpp"
#include "RenderPipeline/UserInterfacePipeline.hpp"
#include "RenderPipeline/RenderPipelineConfig.hpp"

#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/RenderContext.hpp"
#include "Debugging/MetricInterface.hpp"
#include "Scene/Viewport.hpp"
#include "Scene/ScreenView.hpp"

DECLARE_METRIC_GROUP(RenderPipeline)

METRIC_STAT(BeginRenderFrame, RenderPipeline)
METRIC_STAT(EndRenderFrame, RenderPipeline)

namespace Musa
{
RenderPipeline::RenderPipeline(Viewport& vp)
	: viewport(&vp)
{
}
void RenderPipeline::Initialize()
{
}

// NOTE - The ScreenView and the RenderObjectManager classes existing in here, and in general, is temporary
void Musa::RenderPipeline::Execute(ScreenView& screenView, UI::Context& uiContext)
{
	RenderContext& context = *GetGraphicsInterface().GetRenderContext();

	BEGIN_TIMED_BLOCK(BeginRenderFrame);
	context.BeginRenderFrame(viewport->GetNativeViewport());
	END_TIMED_BLOCK(BeginRenderFrame);

	DeferredRender::Render(context, renderViews, viewport->GetRenderTargets());
	DeferredRender::RenderUI(context, uiContext, *viewport->GetRenderTargets().userInterfaceTarget, screenView.view);

	DeferredRender::ComposeBackbuffer(
		context,
		*viewport->GetRenderTargets().sceneTargets.sceneColorTexture,
		*viewport->GetRenderTargets().userInterfaceTarget,
		screenView.view
	);

	BEGIN_TIMED_BLOCK(EndRenderFrame);
	context.EndRenderFrame(viewport->GetNativeViewport());
	END_TIMED_BLOCK(EndRenderFrame);

	for (auto& view : renderViews)
	{
		delete view;
	}
}
void RenderPipeline::HandOffRenderViews(DynamicArray<RenderView *>&& renderViews_)
{
	renderViews = MOVE(renderViews_);

	ScaleViewportForViews();
}
void Musa::RenderPipeline::ScaleViewportForViews()
{
	for (auto& view : renderViews)
	{
		// TODO - Doesn't support viewport origin scaling
		view->scaledViewport = Recti{
			0, 0,
			(i32)(view->normViewport.width * viewport->GetWidth()),
			(i32)(view->normViewport.height * viewport->GetHeight())
		};
	}
}
}
