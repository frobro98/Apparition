// Copyright 2020, Nathan Blane

#include "Scene.hpp"
#include "ScreenView.hpp"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Camera/ViewFrustum.hpp"
#include "Thread/JobSystem/JobUtilities.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Scene/Viewport.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "RenderPipeline/SceneRenderPipeline.h"
#include "Graphics/RenderContext.hpp"
#include "Graphics/ResourceInitializationDescriptions.hpp"
#include "RenderPipeline/BatchPrimitives.hpp"

#include "Debugging/MetricInterface.hpp"

DECLARE_METRIC_GROUP(SceneFrame);
METRIC_STAT(BeginRenderFrame, SceneFrame);
METRIC_STAT(EndRenderFrame, SceneFrame);


// static void BuildGBufferDescription(RenderTargetAttachment& gbufferDesc, ImageFormat format)
// {
// 	gbufferDesc.format = format;
// 	gbufferDesc.load = LoadOperation::Clear;
// 	gbufferDesc.store = StoreOperation::Store;
// 	gbufferDesc.stencilLoad = LoadOperation::DontCare;
// 	gbufferDesc.stencilStore = StoreOperation::DontCare;
// 	gbufferDesc.sampleCount = 1;
// }
// 
// static void BuildGBufferDepth(RenderTargetAttachment& depthDesc)
// {
// 	depthDesc.format = ImageFormat::DS_32f_8u;
// 	depthDesc.load = LoadOperation::Clear;
// 	depthDesc.store = StoreOperation::Store;
// 	depthDesc.stencilLoad = LoadOperation::DontCare;
// 	depthDesc.stencilStore = StoreOperation::DontCare;
// 	depthDesc.sampleCount = 1;
// }




