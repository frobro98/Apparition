// Copyright 2020, Nathan Blane

#include "Viewport.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/RenderContextUtilities.hpp"

Viewport::Viewport(void* windowHandle, u32 width, u32 height)
	: viewWidth(width),
	viewHeight(height)
{
	graphicsViewport = GetGraphicsInterface().CreateViewport(windowHandle, viewWidth, viewHeight);

	InitializeRenderTargets();
}

Viewport::~Viewport()
{
	GBuffer& gbuffer = renderTargets.gbuffer;
	DestroyRenderTarget(gbuffer.positionTexture);
	DestroyRenderTarget(gbuffer.normalTexture);
	DestroyRenderTarget(gbuffer.diffuseTexture);
	
	SceneRenderTargets& sceneTargets = renderTargets.sceneTargets;
	DestroyRenderTarget(sceneTargets.sceneColorTexture);
	DestroyRenderTarget(sceneTargets.depthTexture);

	DestroyRenderTarget(renderTargets.userInterfaceTarget);

	GetGraphicsInterface().DestroyViewport(graphicsViewport);
}

void Viewport::Resize(u32 newWidth, u32 newHeight)
{
	GetGraphicsInterface().ResizeViewport(*graphicsViewport, newWidth, newHeight);
	viewWidth = newWidth;
	viewHeight = newHeight;
}

void Viewport::InitializeRenderTargets()
{
	GBuffer& gbuffer = renderTargets.gbuffer;
	gbuffer.positionTexture = CreateRenderTarget(
		ImageFormat::RGBA_16f, viewWidth, viewHeight,
		LoadOperation::Clear, StoreOperation::Store,
		LoadOperation::DontCare, StoreOperation::DontCare,
		TextureUsage::RenderTarget
	);
	gbuffer.normalTexture = CreateRenderTarget(
		ImageFormat::RGBA_16f,
		viewWidth, viewHeight,
		LoadOperation::Clear, StoreOperation::Store,
		LoadOperation::DontCare, StoreOperation::DontCare,
		TextureUsage::RenderTarget
	);
	gbuffer.diffuseTexture = CreateRenderTarget(
		ImageFormat::RGBA_8norm,
		viewWidth, viewHeight,
		LoadOperation::Clear, StoreOperation::Store,
		LoadOperation::DontCare, StoreOperation::DontCare,
		TextureUsage::RenderTarget
	);

	SceneRenderTargets& sceneTargets = renderTargets.sceneTargets;
	sceneTargets.sceneColorTexture = CreateRenderTarget(
		ImageFormat::RGBA_8norm,
		viewWidth, viewHeight,
		LoadOperation::Clear, StoreOperation::Store,
		LoadOperation::DontCare, StoreOperation::DontCare,
		TextureUsage::RenderTarget
	);
	sceneTargets.depthTexture = CreateRenderTarget(
		ImageFormat::DS_32f_8u,
		viewWidth, viewHeight,
		LoadOperation::Clear, StoreOperation::Store,
		LoadOperation::DontCare, StoreOperation::DontCare,
		TextureUsage::DepthStencilTarget
	);

	renderTargets.userInterfaceTarget = CreateRenderTarget(
		ImageFormat::RGBA_8norm,
		viewWidth, viewHeight,
		LoadOperation::Clear, StoreOperation::Store,
		LoadOperation::DontCare, StoreOperation::DontCare,
		TextureUsage::RenderTarget
	);
}
