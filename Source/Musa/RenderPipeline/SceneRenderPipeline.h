// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"

class RenderContext;
class Scene;
class RenderObjectManager;
struct View;
struct GBuffer;
struct SceneRenderTargets;
struct RenderView;

namespace DeferredRender
{
//void RenderSceneDeferred(RenderContext& renderContext, const GBuffer& gbuffer, const SceneRenderTargets& sceneColorTexture, const RenderObjectManager& renderManager, const View& view);
void RenderSceneDeferred(RenderContext& renderContext, const DynamicArray<RenderView*>& views, const GBuffer& gbuffer, const SceneRenderTargets& sceneTextures);
}
