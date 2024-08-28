// Copyright 2020, Nathan Blane

#include "SceneRenderPipeline.h"
#include "LightingPipeline.hpp"

#include "Scene/GBufferTargets.hpp"
#include "Scene/SceneRenderTargets.hpp"

#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Texture/Texture2D/Texture.h"
#include "Mesh/MeshRenderInfo.hpp"
#include "Mesh/Mesh.h"
#include "Shader/Material.hpp"
#include "Shader/ShaderResource.hpp"

#include "Archiver/SkeletonHeader.h"

#include "Graphics/ResourceInitializationDescriptions.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/RenderContext.hpp"

#include "Containers/DynamicArray.hpp"
#include "Containers/Map.h"
#include "Lighting/Light.hpp"
#include "Math/MatrixFunctions.hpp"

#include "Debugging/MetricInterface.hpp"

#include "Scene/Scene.hpp"
#include "Scene/Viewport.hpp"
#include "Scene/ScreenView.hpp"

#include "BatchPrimitives.hpp"
#include "UserInterfacePipeline.hpp"
#include "LightingPipeline.hpp"
#include "Graphics/RenderContextUtilities.hpp"

#include "Debugging/MetricInterface.hpp"

// Render Pipeline Additions
#include "RenderPipeline/RenderPipelineConfig.hpp"

DECLARE_METRIC_GROUP(SceneRender);
METRIC_STAT(DeferredRender, SceneRender);
METRIC_STAT(NormalMapRender, SceneRender);
METRIC_STAT(GBufferRenderPass, SceneRender);
METRIC_STAT(RenderToScreen, SceneRender);
METRIC_STAT(TextDisplayRender, SceneRender);


// static void RenderWithNormalMap(RenderContext& renderer, const RenderObject& object, const View& view)
// {
// 	SCOPED_TIMED_BLOCK(NormalMapRender);
// 	// Set Transform Data
// 	renderer.SetUniformBuffer(*object.gpuRenderInfo->transformBuffer, 0);
// 
// 	// Set View information
// 	renderer.SetUniformBuffer(*view.viewBuffer, 1);
// 
// 	MaterialRenderInfo* matInfo = object.gpuRenderInfo->meshMaterial;
// 	if (matInfo->baseTexture != nullptr)
// 	{
// 		// Set Texture Data
// 		renderer.SetTexture(*matInfo->baseTexture, *SamplerDesc(), 2);
// 	}
// 
// 	// Set Material Data
// 	renderer.SetUniformBuffer(*matInfo->materialProperties, 3);
// 
// 	// Set Normal Map
// 	renderer.SetTexture(*matInfo->normalMap, *SamplerDesc(), 4);
// }

static void ConstructPipelineDescription(const FixedArray<ColorDescription, MaxColorTargetCount>& colorDescs, const DepthStencilDescription& depthDesc, GraphicsPipelineDescription& desc)
{
	desc.colorAttachments = colorDescs;
	desc.depthAttachment = depthDesc;
	desc.vertexInputs = GetVertexInput<Vertex>();
	desc.rasterizerDesc = RasterDesc();
	for (u32 i = 0; i < desc.colorAttachments.Size(); ++i)
	{
		desc.blendingDescs[i] = BlendDesc();
	}
	desc.depthStencilTestDesc = DepthTestDesc();
	desc.topology = PrimitiveTopology::TriangleList;
}

using RenderTargetList = FixedArray<const RenderTarget*, MaxColorTargetCount>;

// static void GBufferRenderPass(RenderContext& context, const GBuffer& gbuffer, const SceneRenderTargets& sceneTargets, /*const RenderObjectManager& renderManager,*/ const View& view)
// {
// 	DynamicArray<Color32> clearColors(4); // TODO - SHould be using FixedArray here instead of DynamicArray
// 	clearColors[0] = Color32(.7f, .7f, .8f);
// 	clearColors[1] = Color32(0, 0, 0);
// 	clearColors[2] = Color32(0, 0, 0);
// 	clearColors[3] = Color32(.7f, .7f, .8f);
// 
// 	// Begin GBuffer pass
// 	RenderTargetList colorTargets;
// 	colorTargets.Add(sceneTargets.sceneColorTexture.Get());
// 	colorTargets.Add(gbuffer.positionTexture.Get());
// 	colorTargets.Add(gbuffer.normalTexture.Get());
// 	colorTargets.Add(gbuffer.diffuseTexture.Get());
// 
// 	RenderTargetDescription gbufferDesc = CreateRenderTargetDescription(colorTargets, sceneTargets.depthTexture.Get(), RenderTargetAccess::Write);
// 	NativeRenderTargets targets = CreateNativeRenderTargets(colorTargets, sceneTargets.depthTexture.Get());
// 
// 	// TODO - This call happens right before every render target getting set, so it should be lumped into that behavior...
// 	TransitionTargetsToWrite(context, targets);
// 
// 	context.SetRenderTarget(gbufferDesc, targets, clearColors);
// 	{
// 		SCOPED_TIMED_BLOCK(GBufferRenderPass);
// 
// 		// TODO - This should be a pipeline helper function
// 		u32 viewWidth = (u32)view.description.viewport.width;
// 		u32 viewHeight = (u32)view.description.viewport.height;
// 		context.SetViewport(0, 0, viewWidth, viewHeight, 0, 1);
// 		context.SetScissor(0, 0, viewWidth, viewHeight);
// 
// 		const DynamicArray<RenderObject*>& renderObjects = renderManager.GetRenderObjects();
// 
// 		for (auto& info : renderObjects)
// 		{
// 			MaterialRenderDescription* matInfo = info->gpuRenderInfo->meshMaterial;
// 
// 			GraphicsPipelineDescription pipelineDesc;
// 			ConstructPipelineDescription(gbufferDesc, pipelineDesc);
// 			pipelineDesc.vertexShader = matInfo->shaders[ShaderStage::Vertex]->GetVertexShader();
// 			pipelineDesc.fragmentShader = matInfo->shaders[ShaderStage::Fragment]->GetFragmentShader();
// 			context.SetGraphicsPipeline(pipelineDesc);
// 
// 			MaterialResourceTable* resourceTable = matInfo->resources;
// 			for (const auto& binding : resourceTable->resourceBindings)
// 			{
// 				if (binding.type == ShaderResourceType::UniformBuffer)
// 				{
// 					const NativeUniformBuffer* ub = resourceTable->uniformBufferStorage[binding.resourceIndex];
// 					context.SetUniformBuffer(*ub, binding.bindIndex);
// 				}
// 				else if (binding.type == ShaderResourceType::TextureSampler)
// 				{
// 					const NativeTexture* texture = resourceTable->textureStorage[binding.resourceIndex];
// 					const NativeSampler* sampler = resourceTable->samplerStorage[binding.resourceIndex];
// 					context.SetTexture(*texture, *sampler, binding.bindIndex);
// 				}
// 			}
// 			context.SetVertexBuffer(*info->gpuRenderInfo->vertexBuffer);
// 			context.DrawIndexed(*info->gpuRenderInfo->indexBuffer, 1);
// 		}
// 	}
// 
// 	TransitionTargetsToRead(context, targets);
// }

/*static*/ void LightingRenderPass(RenderContext& context, const GBuffer& /*gbuffer*/, const SceneRenderTargets& sceneTargets, const Scene& /*scene*/, const View& /*view*/)
{
	// TODO - Should clear color be floating around or should it live with the render target?
	DynamicArray<Color32> clearColors = { Color32(.7f, .7f, .8f) }; // TODO - SHould be using FixedArray here instead of DynamicArray

	RenderTargetList sceneColorTarget;
	sceneColorTarget.Add(sceneTargets.sceneColorTexture);
	/*const */FixedArray<ColorDescription, MaxColorTargetCount> colorAttachments = CreateColorTargetDescriptions(sceneColorTarget, RenderTargetAccess::Read);
	const DepthStencilDescription depthAttachment = CreateDepthTargetDescription(sceneTargets.depthTexture, RenderTargetAccess::Read);
	//RenderTargetDescription targetDescription = CreateRenderTargetDescription(sceneColorTarget, sceneTargets.depthTexture, RenderTargetAccess::Read);
	NativeRenderTargets sceneRenderTargets = CreateNativeRenderTargets(sceneColorTarget, sceneTargets.depthTexture);

	// TODO - Fix whatever this is...
	colorAttachments[0].op = ColorTargetOperations::Load_Store;
	const NativeTexture* depth = sceneRenderTargets.depthTarget;
	sceneRenderTargets.depthTarget = nullptr;

	TransitionTargetsToWrite(context, sceneRenderTargets);

	sceneRenderTargets.depthTarget = depth;

	BeginRenderPassInfo beginInfo = {};
	beginInfo.colorAttachments = colorAttachments;
	beginInfo.depthAttachment = depthAttachment;
	beginInfo.clearColors.Add(Color32(.7f, .7f, .8f));
	beginInfo.targets = sceneRenderTargets;

	context.BeginRenderPass(beginInfo);

	//DeferredRender::RenderLights(context, scene, gbuffer, view);

	context.EndRenderPass();

	TransitionTargetsToRead(context, sceneRenderTargets);
}

namespace DeferredRender
{
// void RenderSceneDeferred(RenderContext& /*renderContext*/, const GBuffer& /*gbuffer*/, const SceneRenderTargets& /*sceneTextures*/, const View& /*view*/)
// {
// 	SCOPED_TIMED_BLOCK(DeferredRender);
// 
// 	// Render to gbuffer
// 	GBufferRenderPass(renderContext, gbuffer, sceneTextures, renderManager, view);
// 
// 	// Do any special rendering for the scene here as well
// 
// 	// Render lighting
// 	LightingRenderPass(renderContext, gbuffer, sceneTextures, scene, view);
// 
// 	// Post Processing on Scene Color(?????)
// }

void RenderSceneDeferred(RenderContext& renderContext, const DynamicArray<RenderView*>& views, const GBuffer& gbuffer, const SceneRenderTargets& sceneTargets)
{
	SCOPED_TIMED_BLOCK(DeferredRender);

	FixedArray<Color32, MaxColorTargetCount> clearColors(4); // TODO - SHould be using FixedArray here instead of DynamicArray
	clearColors[0] = Color32(.7f, .7f, .8f);
	clearColors[1] = Color32(0, 0, 0);
	clearColors[2] = Color32(0, 0, 0);
	clearColors[3] = Color32(.7f, .7f, .8f);

	// Begin GBuffer pass
	RenderTargetList colorTargets;
	colorTargets.Add(sceneTargets.sceneColorTexture);
	colorTargets.Add(gbuffer.positionTexture);
	colorTargets.Add(gbuffer.normalTexture);
	colorTargets.Add(gbuffer.diffuseTexture);

	//RenderTargetDescription gbufferDesc = CreateRenderTargetDescription(colorTargets, sceneTargets.depthTexture, RenderTargetAccess::Write);
	const FixedArray<ColorDescription, MaxColorTargetCount> colorAttachments = CreateColorTargetDescriptions(colorTargets, RenderTargetAccess::Write);
	const DepthStencilDescription depthAttachment = CreateDepthTargetDescription(sceneTargets.depthTexture, RenderTargetAccess::Write);
	NativeRenderTargets targets = CreateNativeRenderTargets(colorTargets, sceneTargets.depthTexture);

	// TODO - This call happens right before every render target getting set, so it should be lumped into that behavior...
	TransitionTargetsToWrite(renderContext, targets);

	BeginRenderPassInfo beginInfo = {};
	beginInfo.colorAttachments = colorAttachments;
	// TODO - Make sure that this is after some sort of depth prepass
	beginInfo.depthAttachment = depthAttachment;
	beginInfo.clearColors = clearColors;
	beginInfo.targets = targets;

	renderContext.BeginRenderPass(beginInfo);
	{
		SCOPED_TIMED_BLOCK(GBufferRenderPass);

		for (const auto& view : views)
		{
			u32 viewWidth = (u32)view->scaledViewport.width;
			u32 viewHeight = (u32)view->scaledViewport.height;
			renderContext.SetViewport(0, 0, viewWidth, viewHeight, 0, 1);
			renderContext.SetScissor(0, 0, viewWidth, viewHeight);

			const DynamicArray<MaterialPrimitiveBatch>& primitiveBatches = view->opaqueMeshBatches.GetBatches();
			// NOTE - Opaque objects
			for(const auto& batch : primitiveBatches)
			{
				for (const auto& primitive : batch)
				{
					const PipelineConfiguration& pipelineConfig = primitive.pipelineConfig;

					GraphicsPipelineDescription pipelineDesc;
					ConstructPipelineDescription(colorAttachments, depthAttachment, pipelineDesc);
					pipelineDesc.vertexShader = pipelineConfig.shaders[ShaderStage::Vertex]->GetVertexShader();
					pipelineDesc.fragmentShader = pipelineConfig.shaders[ShaderStage::Fragment]->GetFragmentShader();
					renderContext.SetGraphicsPipeline(pipelineDesc);


					// TODO - This resource table consists of the "Prim" and "View" resources that aren't able to be set
					// by the users of the Material interface. Remove these from the table!
					renderContext.SetUniformBuffer(*primitive.primitiveBuffer, 0);
					renderContext.SetUniformBuffer(*view->viewUniformBuffer, 1);
					renderContext.SetUniformBuffer(*primitive.materialProperties, 2);
					const MaterialResourceTable* resourceTable = primitive.materialResources;
					for (u32 i = 3; i < resourceTable->resourceBindings.Size(); ++i)
					{
						MaterialResourceDesc resDesc = resourceTable->resourceBindings[i];
						if (resDesc.type == ShaderResourceType::UniformBuffer)
						{
							const NativeUniformBuffer* ub = resourceTable->uniformBufferStorage[resDesc.resourceIndex];
							renderContext.SetUniformBuffer(*ub, resDesc.bindIndex);
						}
						else if (resDesc.type == ShaderResourceType::TextureSampler)
						{
							const NativeTexture* texture = resourceTable->textureStorage[resDesc.resourceIndex];
							const NativeSampler* sampler = resourceTable->samplerStorage[resDesc.resourceIndex];
							renderContext.SetTexture(*texture, *sampler, resDesc.bindIndex);
						}
					}

					renderContext.SetVertexBuffer(*primitive.vertexBuffer);
					u32 numApiPrimitives = (u32)primitive.indexBuffer->size / sizeof(u32);
					renderContext.DrawPrimitiveIndexed(*primitive.indexBuffer, numApiPrimitives, 1, 0, 0, 0);
				}
			}
		}
	}
	renderContext.EndRenderPass();

	TransitionTargetsToRead(renderContext, targets);
}
}

// struct ShadowMapTextures
// {
// 	NativeTexture* depthTarget = nullptr;
// 	NativeTexture* depthTextureResource = nullptr;
// };
// static ShadowMapTextures shadowMap;
// 
// constexpr uint32 ShadowMapWidth = 1024;
// constexpr uint32 ShadowMapHeight = 1024;
//VulkanShader* shadowVertShader = nullptr;

// static void InitializeShadowMap()
// {
// 	VkImageSubresourceRange range = {};
// 	range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
// 	range.layerCount = 1;
// 	range.levelCount = 1;
// 
// 	VulkanCommandBuffer* cmdBuffer = GetGraphicsInterface().GetGraphicsDevice()->GetCmdBufferManager().GetActiveGraphicsBuffer();
// 
// 	// Setup initial texture
// 	VulkanImage* image = GetGraphicsInterface().GetGraphicsDevice()->GetMemoryManager().AllocateImage(
// 		ShadowMapWidth, ShadowMapHeight, 
// 		VK_FORMAT_D32_SFLOAT_S8_UINT, 1, VK_IMAGE_TILING_OPTIMAL, 
// 		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
// 		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
// 	image->aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
// 
// 	ImageLayoutTransition(*cmdBuffer, range, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, { image });
// 	shadowMap.depthTarget = new VulkanTexture(*image);
// 
// 	// Setup read texture
// 	image = GetGraphicsInterface().GetGraphicsDevice()->GetMemoryManager().AllocateImage(
// 		ShadowMapWidth, ShadowMapHeight,
// 		VK_FORMAT_D32_SFLOAT_S8_UINT, 1, VK_IMAGE_TILING_OPTIMAL,
// 
// 		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
// 		VK_IMAGE_USAGE_TRANSFER_DST_BIT |
// 		VK_IMAGE_USAGE_SAMPLED_BIT,
// 		
// 		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
// 	image->aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
// 
// 	ImageLayoutTransition(*cmdBuffer, range, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, {image});
// 
// 	shadowMap.depthTextureResource = new VulkanTexture(*image);
// 
// 	VulkanDescriptorSetLayout* descriptorSetLayout = GetDescriptorLayoutManager().CreateSetLayout();
// 	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
// 	descriptorSetLayout->BindLayout();
// 
// // 	shadowVertShader = GetShaderManager().CreateShader("shadowDepth.vs.spv", ShaderStage::Vertex);
// // 	shadowVertShader->SetDescriptorInformation(*descriptorSetLayout);
// }

// RenderTargetDescription GetShadowMapTargetDescription()
// {
// 	RenderTargetDescription targetDesc = {};
// 	targetDesc.targetExtents = { static_cast<float32>(ShadowMapWidth), static_cast<float32>(ShadowMapHeight) };
// 
// 	RenderTargetAttachment& depthDesc = targetDesc.depthAttachment;
// 	depthDesc.format = ImageFormat::DS_32f_8u;
// 	depthDesc.load = LoadOperation::Clear;
// 	depthDesc.store = StoreOperation::Store;
// 	depthDesc.stencilLoad = LoadOperation::DontCare;
// 	depthDesc.stencilStore = StoreOperation::DontCare;
// 	depthDesc.sampleCount = 1;
// 
// 	return targetDesc;
// }

/*
// TODO - This should essentially be only the essential objects in the scene
// i.e. the objects within the view frustum and the light "frustum"
void RenderSceneForShadowMap(Renderer& renderer, Light* light, Scene& scene)
{
	NOT_USED Matrix proj = scene.GetScreenView().view.transforms.projectionMatrix;
	LightDescription lightDesc = light->GetLightDescription();
	TransformationUniformBuffer transformations;
	transformations.view = Math::ConstructViewMatrix(light->GetPosition(), Vector::Zero , Vector::UpAxis);
	//transformations.view = Matrix(TRANS, lightDesc.position).GetInverse();
	transformations.projection = Math::ConstructPerspectiveMatrix(90.f, (float)ShadowMapWidth / (float)ShadowMapHeight, .1f, 1000.);

	GraphicsPipelineDescription init = {};
	ConstructPipelineDescription(GetShadowMapTargetDescription(), init);
	//init.vertexShader = shadowVertShader;

	const DynamicArray<RenderObject*>& renderObjects = GetRenderObjectManager().GetRenderObjects();

	for (const auto& info : renderObjects)
	{
		// Contains all of the persistent data that will be given to the gpu

		//transformations.model = info->transform;

		renderer.SetGraphicsPipeline(init);

		UniformBuffer* transformBuffer = lightUniformPool.GetBufferFromPool(cmdBuffer);
		transformBuffer->UpdateUniforms(&transformations);

		// Set Transform Data
		renderingState.SetUniformBuffer(transformBuffer->GetBuffer(), 0);

		info->gpuRenderInfo->vertexBuffer->Bind(&cmdBuffer);
		info->gpuRenderInfo->indexBuffer->Bind(&cmdBuffer);
		uint32 indexCount = info->gpuRenderInfo->indexBuffer->GetNumberOfIndicies();

		renderingState.Bind(cmdBuffer);

		cmdBuffer.DrawIndexed(indexCount, 1, 0, 0, 0);

	}

	cmdBuffer.EndRenderPass();
}
//*/

// void SceneRendering::RenderShadowPass(VulkanCommandBuffer& cmdBuffer, Scene& scene)
// {
// 	RenderTargetTextures renderTarget = {};
// 	renderTarget.targetCount = 0;
// 	renderTarget.depthTarget = shadowMap.depthTarget;
// 	renderingState.SetFramebufferTarget(cmdBuffer, GetShadowMapTargetDescription(), renderTarget, {});
// 
// 	VkViewport view = {};
// 	view.minDepth = 0;
// 	view.maxDepth = 1;
// 	view.x = 0;
// 	view.y = 0;
// 	view.width = ShadowMapWidth;
// 	view.height = ShadowMapHeight;
// 	cmdBuffer.SetViewport(0, 1, &view);
// 
// 	VkRect2D scissor = {};
// 	scissor.extent = { ShadowMapWidth, ShadowMapHeight };
// 	scissor.offset = { 0, 0 };
// 	cmdBuffer.SetScissor(0, 1, &scissor);
// 
// 	// TODO - This doesn't work with the current render pass state. There needs to be a way to have pipelines stick around, but reset the descriptor set!
// // 	GraphicsPipelineDescription desc = {};
// // 	ConstructPipelineDescription(renderingState, desc);
// // 	desc.vertexShader = shadowVertShader;
// // 
// // 	renderingState.SetGraphicsPipeline(cmdBuffer, desc);
// 
// 	RenderSceneForShadowMap(cmdBuffer, renderingState, scene.GetLights()[0], scene);
// 
// 	// Transition depth images
// 	VkImageSubresourceRange range = {};
// 	range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
// 	range.baseArrayLayer = 0;
// 	range.layerCount = 1;
// 	range.baseMipLevel = 0;
// 	range.levelCount = 1;
// 
// 	VulkanImage* depthImage = shadowMap.depthTarget->image;
// 	VkImageLayout targetOldLayout = depthImage->layout;
// 	VkImageLayout targetNewLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
// 
// 	ImageLayoutTransition(cmdBuffer, range, targetNewLayout, *depthImage);
// 
// 	depthImage = shadowMap.depthTextureResource->image;
// 	VkImageLayout resourceNewLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
// 	ImageLayoutTransition(cmdBuffer, range, resourceNewLayout, *depthImage);
// 
// 	CopyImage(cmdBuffer, *shadowMap.depthTarget->image, *shadowMap.depthTextureResource->image);
// 
// 	ImageLayoutTransition(cmdBuffer, range, targetOldLayout, *shadowMap.depthTarget->image);
// 	ImageLayoutTransition(cmdBuffer, range, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL, *shadowMap.depthTextureResource->image);
// }


// constexpr uint32 minNumObjectsPerJob = 20;
// 
// uint32 SceneRendering::DetermineJobCount() const
// {
// 	const auto& renderObject = GetRenderObjectManager().GetRenderObjects();
// 	return renderObject.Size() / minNumObjectsPerJob;
// }
// 
// void SceneRendering::SecondaryGBufferPass_Job(VulkanCommandBuffer& secondaryCmdBuffer, const View& view, VkFramebuffer framebufferHandle, VkRenderPass renderpassHandle, uint32 startRenderIndex, uint32 endRenderIndex) const
// {
// 	Assert(!secondaryCmdBuffer.HasStarted());
// 	VkCommandBufferInheritanceInfo inheritInfo = {};
// 	inheritInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
// 	inheritInfo.framebuffer = framebufferHandle;
// 	inheritInfo.renderPass = renderpassHandle;
// 	inheritInfo.subpass = 0;
// 	secondaryCmdBuffer.Begin(VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, &inheritInfo);
// 
// 	SetViewportAndScissor(secondaryCmdBuffer, view);
// 
// 	const DynamicArray<RenderObject*>& renderObjects = GetRenderObjectManager().GetRenderObjects();
// 	VulkanRenderPassState jobRenderState;
// 
// 	for (uint32 i = startRenderIndex; i < endRenderIndex; ++i)
// 	{
// 		RenderObject* renderObject = renderObjects[i];
// 
// 		MaterialRenderInfo* matInfo = renderObject->gpuRenderInfo->meshMaterial;
// 
// 		// TODO - Get descriptor sets and update them based on information in the rendering entity
// 		GraphicsPipelineDescription pipelineDesc;
// 		ConstructPipelineDescription(renderingState, pipelineDesc);
// 		pipelineDesc.vertexShader = matInfo->vertexShader;
// 		pipelineDesc.fragmentShader = matInfo->fragmentShader;
// 		jobRenderState.SetGraphicsPipeline(secondaryCmdBuffer, pipelineDesc);
// 
// 		if (matInfo->normalMap != nullptr)
// 		{
// 			RenderWithNormalMap(jobRenderState, *renderObject);
// 		}
// 		else
// 		{
// 			RenderNormally(jobRenderState, *renderObject);
// 		}
// 
// 
// 		renderObject->gpuRenderInfo->vertexBuffer->Bind(&secondaryCmdBuffer);
// 		renderObject->gpuRenderInfo->indexBuffer->Bind(&secondaryCmdBuffer);
// 		uint32 indexCount = renderObject->gpuRenderInfo->indexBuffer->GetNumberOfIndicies();
// 
// 		jobRenderState.Bind(secondaryCmdBuffer);
// 
// 		secondaryCmdBuffer.DrawIndexed(indexCount, 1, 0, 0, 0);
// 	}
// 
// 	secondaryCmdBuffer.End();
// }

/* TODO - Compute doesn't work. Will need to make it work later
void SceneRendering::CreateComputePipelines()
{
	if (firstPassPipeline == nullptr)
	{
		CreateFirstPassPipeline();
		CreateBlendingPassPipeline();
		CreateProcessingPathPipeline();
		CreateFinalPassPipeline();
	}
}

void SceneRendering::CreateFirstPassPipeline()
{
	DynamicArray<uint8> compCode = LoadSPVShader("animFrameInterpolation.cs.spv");
	VulkanShader* compShader = GetShaderManager().CreateShader("animFrameInterpolation.cs.spv", ShaderStage::Compute);

	VulkanDescriptorSetLayout* firstPassLayout = new VulkanDescriptorSetLayout(*GetGraphicsInterface().GetGraphicsDevice());
	firstPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	firstPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	firstPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 2);
	firstPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 3);
	firstPassLayout->BindLayout();

	VulkanPipelineLayout* layout = new VulkanPipelineLayout(*GetGraphicsInterface().GetGraphicsDevice(), { firstPassLayout });

	firstPassPipeline = new VulkanComputePipeline(*GetGraphicsInterface().GetGraphicsDevice());
	firstPassPipeline->Initialize(layout, compShader);
}

void SceneRendering::CreateBlendingPassPipeline()
{
	VulkanShader* compShader = GetShaderManager().CreateShader("animationBlendClips.cs.spv", ShaderStage::Compute);

	VulkanDescriptorSetLayout* blendPassLayout = new VulkanDescriptorSetLayout(*GetGraphicsInterface().GetGraphicsDevice());
	blendPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	blendPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	blendPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 2);
	blendPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 3);

	blendPassLayout->BindLayout();

	VulkanPipelineLayout* layout = new VulkanPipelineLayout(*GetGraphicsInterface().GetGraphicsDevice(), { blendPassLayout });

	blendingPassPipeline = new VulkanComputePipeline(*GetGraphicsInterface().GetGraphicsDevice());
	blendingPassPipeline->Initialize(layout, compShader);
}

void SceneRendering::CreateProcessingPathPipeline()
{
	VulkanShader* compShader = GetShaderManager().CreateShader("processLocalTransforms.cs.spv", ShaderStage::Compute);

	VulkanDescriptorSetLayout* processPassLayout = new VulkanDescriptorSetLayout(*GetGraphicsInterface().GetGraphicsDevice());
	processPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	processPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	processPassLayout->BindLayout();

	VulkanPipelineLayout* layout = new VulkanPipelineLayout(*GetGraphicsInterface().GetGraphicsDevice(), { processPassLayout });

	processLocalsPassPipeline = new VulkanComputePipeline(*GetGraphicsInterface().GetGraphicsDevice());
	processLocalsPassPipeline->Initialize(layout, compShader);
}

void SceneRendering::CreateFinalPassPipeline()
{
	VulkanShader* compShader = GetShaderManager().CreateShader("animHierarchyCalculation.cs.spv", ShaderStage::Compute);

	VulkanDescriptorSetLayout* finalPassLayout = new VulkanDescriptorSetLayout(*GetGraphicsInterface().GetGraphicsDevice());
	finalPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 0);
	finalPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 1);
	finalPassLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT, 2);
	finalPassLayout->BindLayout();

	VulkanPipelineLayout* layout = new VulkanPipelineLayout(*GetGraphicsInterface().GetGraphicsDevice(), { finalPassLayout });

	finalPassPipeline = new VulkanComputePipeline(*GetGraphicsInterface().GetGraphicsDevice());
	finalPassPipeline->Initialize(layout, compShader);
}

ComputeEntity& SceneRendering::BuildComputeCommandBuffer(Model& model)
{
	ComputeEntity* entity = ComputeEntityManager::CreateOrGetEntity(*GetGraphicsInterface().GetGraphicsDevice(), model);

	CreateComputePipelines();

	// Entity update buffers
	KeyFrameRange frameRange = model.GetSkeleton()->GetFrameRange();
	entity->GetFirstFrameStorage().UpdateStorage(frameRange.frame0->boneFrameData);
	entity->GetSecondFrameStorage().UpdateStorage(frameRange.frame1->boneFrameData);

	KeyframeConstanceUniformBuffer constants;
	constants.boneCount = model.GetSkeleton()->GetBoneCount();
	constants.deltaTime = frameRange.deltaTime;
	entity->GetKeyframeConstantsBuffer().UpdateUniforms(&constants);

	if (model.GetSkeleton()->IsBlending())
	{
		KeyFrameRange blendRange = model.GetSkeleton()->GetBlendRange();
		entity->GetFirstBlendFrameStorage().UpdateStorage(blendRange.frame0->boneFrameData);
		entity->GetSecondBlendFrameStorage().UpdateStorage(blendRange.frame1->boneFrameData);
		constants.deltaTime = blendRange.deltaTime;
		entity->GetBlendConstantsBuffer().UpdateUniforms(&constants);

		constants.deltaTime = model.GetSkeleton()->GetBlendDelta();
		entity->GetBlendResultingConstantsBuffer().UpdateUniforms(&constants);
	}

	static bool initialized = false;
	if (!initialized)
	{
		// Plus 1 for the default identity matrix
		Matrix* boneIden = new Matrix[constants.boneCount + 1];
		for (uint32 i = 0; i < constants.boneCount + 1; ++i)
		{
			boneIden[i].Set(IDENTITY);
		}

		entity->GetBoneMatrixStorage().UpdateStorage(boneIden);
		delete[] boneIden;

		//initialized = true;
	}

	const BoneHierarchyTable& table = model.GetSkeleton()->GetHierarchyTable();
	entity->GetHierarchyTableBuffer().UpdateStorage(&table);

	// Printing out debug info
	{
// 		
// 		static bool doMatrixPrint = false;
// 
// 		FrameData* frameData = reinterpret_cast<FrameData*>(entity->GetInterpolatedTransformsStorage().GetMappedData());
// 		Matrix* linkList = reinterpret_cast<Matrix*>(entity->GetBoneMatrixStorage().GetMappedData());
// 		//Matrix* matList = reinterpret_cast<Matrix*>(entity->GetTransformedHierarchy().GetMappedData());
// 		if (!doMatrixPrint)
// 		{
// 			Debug::Print("\n\n");
// 			BoneHierarchyTable* mappedTable = reinterpret_cast<BoneHierarchyTable*>(entity->GetHierarchyTableBuffer().GetMappedData());
// 			for (uint32 i = 0; i < constants.boneCount; ++i)
// 			{
// 				Debug::Printf("Bone %u: ", i);
// 				for (uint32 j = 0; j < BoneHierarchyTable::BoneTableElement::MaxElementLength; ++j)
// 				{
// 					Debug::Printf("Parent %d,", mappedTable->table[i].boneParentHierarchy[j]);
// 				}
// 				Debug::Print("\n");
// 			}
// 			Debug::Print("\n\n");
// 		}
// 
// 		
// 		static int count = 0;
// 		if (doMatrixPrint && count < 5)
// 		{
// 			Debug::Print("Bone Local Matrix data\n");
// 			for (uint32 i = 0; i < constants.boneCount + 1; ++i)
// 			{
// 				Matrix* mat = &linkList[i];
// 				Debug::Printf("Bone %u's local matrix: \n", i);
// 				Debug::Printf("\t m0(%f),  m1(%f),  m2(%f),  m3(%f),\n", (*mat)[m0], (*mat)[m1], (*mat)[m2], (*mat)[m3]);
// 				Debug::Printf("\t m4(%f),  m5(%f),  m2(%f),  m7(%f),\n", (*mat)[m4], (*mat)[m5], (*mat)[m6], (*mat)[m7]);
// 				Debug::Printf("\t m8(%f),  m9(%f), m10(%f), m11(%f),\n", (*mat)[m8], (*mat)[m9], (*mat)[m10], (*mat)[m11]);
// 				Debug::Printf("\tm12(%f), m13(%f), m14(%f), m15(%f)\n\n", (*mat)[m12], (*mat)[m13], (*mat)[m14], (*mat)[m15]);
// 			}
// 			Debug::Print("\n\n");
// 
// 			Debug::Print("Frame Local Bone Transforms\n");
// 			for(uint32 i = 0; i < constants.boneCount; ++i)
// 			{
// 				FrameData* data = &frameData[i];
// 				Debug::Printf("Bone %u's interpolated data\n", i);
// 				Debug::Printf("Translation: x(%f), y(%f), z(%f)\n", data->translation[x], data->translation[y], data->translation[z]);
// 				Debug::Printf("Rotation: x(%f), y(%f), z(%f), w(%f)\n", data->rotation[x], data->rotation[y], data->rotation[z], data->rotation[w]);
// 				Debug::Printf("Scale: x(%f), y(%f), z(%f)\n", data->scale[x], data->scale[y], data->scale[z]);
// 				Debug::Print("\n");
// 			}
// 
// // 			Debug::Print("Transformed Matrix data\n");
// // 			for (uint32 i = 0; i < constants.boneCount; ++i)
// // 			{
// // 				Matrix* mat = &matList[i];
// // 				Debug::Printf("Bone %u's transformed matrix: \n", i);
// // 				Debug::Printf("\t m0(%f),  m1(%f),  m2(%f),  m3(%f)\n,", (*mat)[m0], (*mat)[m1], (*mat)[m2], (*mat)[m3]);
// // 				Debug::Printf("\t m4(%f),  m5(%f),  m2(%f),  m7(%f)\n,", (*mat)[m4], (*mat)[m5], (*mat)[m6], (*mat)[m7]);
// // 				Debug::Printf("\t m8(%f),  m9(%f), m10(%f), m11(%f)\n,", (*mat)[m8], (*mat)[m9], (*mat)[m10], (*mat)[m11]);
// // 				Debug::Printf("\tm12(%f), m13(%f), m14(%f), m15(%f)\n\n", (*mat)[m12], (*mat)[m13], (*mat)[m14], (*mat)[m15]);
// // 			}
// // 			Debug::Print("\n\n");
// 
// 			count++;
// 		}
// 		doMatrixPrint = true;
	}

	if (entity->GetFirstPassDescriptorSet() == nullptr)
	{
		VulkanDescriptorSet* ds = firstPassPipeline->GetUnusedDescriptorSet();
		ds->SetStorageBufferInfo(entity->GetFirstFrameStorage().GetBuffer(), 0);
		ds->SetStorageBufferInfo(entity->GetSecondFrameStorage().GetBuffer(), 1);
		ds->SetUniformBufferInfo(entity->GetKeyframeConstantsBuffer().GetBuffer(), 2);
		ds->SetStorageBufferInfo(entity->GetInterpolatedTransformsStorage().GetBuffer(), 3);
		ds->UpdateDescriptorSet();
		entity->SetFirstPassDescriptorSet(*ds);

		ds = firstPassPipeline->GetUnusedDescriptorSet();
		ds->SetStorageBufferInfo(entity->GetFirstFrameStorage().GetBuffer(), 0);
		ds->SetStorageBufferInfo(entity->GetSecondFrameStorage().GetBuffer(), 1);
		ds->SetUniformBufferInfo(entity->GetKeyframeConstantsBuffer().GetBuffer(), 2);
		ds->SetStorageBufferInfo(entity->GetFirstPassBlendResultStorage().GetBuffer(), 3);
		ds->UpdateDescriptorSet();
		entity->SetFirstPassBlendDescriptor(*ds);

		ds = firstPassPipeline->GetUnusedDescriptorSet();
		ds->SetStorageBufferInfo(entity->GetFirstBlendFrameStorage().GetBuffer(), 0);
		ds->SetStorageBufferInfo(entity->GetSecondBlendFrameStorage().GetBuffer(), 1);
		ds->SetUniformBufferInfo(entity->GetBlendConstantsBuffer().GetBuffer(), 2);
		ds->SetStorageBufferInfo(entity->GetBlendTransformsBuffer().GetBuffer(), 3);
		ds->UpdateDescriptorSet();
		entity->SetBlendingDescriptorSet(*ds);

		ds = blendingPassPipeline->GetUnusedDescriptorSet();
		ds->SetStorageBufferInfo(entity->GetFirstPassBlendResultStorage().GetBuffer(), 0);
		ds->SetStorageBufferInfo(entity->GetBlendTransformsBuffer().GetBuffer(), 1);
		ds->SetUniformBufferInfo(entity->GetBlendResultingConstantsBuffer().GetBuffer(), 2);
		ds->SetStorageBufferInfo(entity->GetInterpolatedTransformsStorage().GetBuffer(), 3);
		ds->UpdateDescriptorSet();
		entity->SetBlendingResultDescriptorSet(*ds);

		ds = processLocalsPassPipeline->GetUnusedDescriptorSet();
		ds->SetStorageBufferInfo(entity->GetInterpolatedTransformsStorage().GetBuffer(), 0);
		ds->SetStorageBufferInfo(entity->GetBoneMatrixStorage().GetBuffer(), 1);
		ds->UpdateDescriptorSet();
		entity->SetProcessingPassDescriptorSet(*ds);

		ds = finalPassPipeline->GetUnusedDescriptorSet();
		ds->SetStorageBufferInfo(entity->GetBoneMatrixStorage().GetBuffer(), 0);
		ds->SetStorageBufferInfo(entity->GetHierarchyTableBuffer().GetBuffer(), 1);
		ds->SetStorageBufferInfo(entity->GetTransformedHierarchy().GetBuffer(), 2);
		ds->UpdateDescriptorSet();
		entity->SetFinalPassDescriptorSet(*ds);
	}

	VulkanCommandBuffer* computeBuffer = GetGraphicsInterface().GetGraphicsDevice()->GetCmdBufferManager().GetActiveGraphicsBuffer();
	if (!computeBuffer->HasStarted())
	{
		computeBuffer->Begin();
	}

	VkBufferMemoryBarrier memBarrier = {};
	memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	memBarrier.buffer = entity->GetTransformedHierarchy().GetBuffer().handle;
	memBarrier.size = entity->GetTransformedHierarchy().GetBuffer().memory->size;
	memBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	memBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	// Compute and graphics queue may have different queue families
	// For the barrier to work across different queues, we need to set their family indices
	memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	computeBuffer->BufferMemoryBarrier(
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		1, &memBarrier
	);
	
	constexpr uint32 WorkGroupCount = 10;

	


	if (!model.GetSkeleton()->IsBlending())
	{
		firstPassPipeline->Bind(computeBuffer);
		firstPassPipeline->BindDescriptorSet(computeBuffer, entity->GetFirstPassDescriptorSet());

		computeBuffer->Dispatch(model.GetSkeleton()->GetBoneCount() / WorkGroupCount, 1, 1);

		memBarrier = {};
		memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		memBarrier.buffer = entity->GetInterpolatedTransformsStorage().GetBuffer().handle;
		memBarrier.size = entity->GetInterpolatedTransformsStorage().GetBuffer().memory->size;
		memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		computeBuffer->BufferMemoryBarrier(
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			1, &memBarrier
		);
	}
	else
	{
// 		{
// 			FrameData* frameData = reinterpret_cast<FrameData*>(entity->GetBlendTransformsBuffer().GetMappedData());
// 			Debug::Print("Blending Locals data\n");
// 			for (uint32 i = 0; i < constants.boneCount + 1; ++i)
// 			{
// 				FrameData* data = &frameData[i];
// 				Debug::Printf("Bone %u's interpolated data\n", i);
// 				Debug::Printf("Translation: x(%f), y(%f), z(%f)\n", data->translation.x, data->translation.y, data->translation.z);
// 				Debug::Printf("Rotation: x(%f), y(%f), z(%f), w(%f)\n", data->rotation.x, data->rotation.y, data->rotation.z, data->rotation.w);
// 				Debug::Printf("Scale: x(%f), y(%f), z(%f)\n", data->scale.x, data->scale.y, data->scale.z);
// 				Debug::Print("\n");
// 			}
// 			Debug::Print("\n\n");
// 		}

		firstPassPipeline->Bind(computeBuffer);
		firstPassPipeline->BindDescriptorSet(computeBuffer, entity->GetFirstPassBlendDescriptorSet());

		computeBuffer->Dispatch(model.GetSkeleton()->GetBoneCount() / WorkGroupCount, 1, 1);

		// I think this is unnecessary
		firstPassPipeline->Bind(computeBuffer);
		firstPassPipeline->BindDescriptorSet(computeBuffer, entity->GetBlendingDescriptorSet());

		computeBuffer->Dispatch(model.GetSkeleton()->GetBoneCount() / WorkGroupCount, 1, 1);

		memBarrier = {};
		memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		memBarrier.buffer = entity->GetFirstPassBlendResultStorage().GetBuffer().handle;
		memBarrier.size = entity->GetFirstPassBlendResultStorage().GetBuffer().memory->size;
		memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		computeBuffer->BufferMemoryBarrier(
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			1, &memBarrier
		);

		memBarrier = {};
		memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		memBarrier.buffer = entity->GetBlendTransformsBuffer().GetBuffer().handle;
		memBarrier.size = entity->GetBlendTransformsBuffer().GetBuffer().memory->size;
		memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		computeBuffer->BufferMemoryBarrier(
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			1, &memBarrier
		);

		blendingPassPipeline->Bind(computeBuffer);
		blendingPassPipeline->BindDescriptorSet(computeBuffer, entity->GetBlendingResultDescriptorSet());

		computeBuffer->Dispatch(model.GetSkeleton()->GetBoneCount() / WorkGroupCount, 1, 1);

		memBarrier = {};
		memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		memBarrier.buffer = entity->GetInterpolatedTransformsStorage().GetBuffer().handle;
		memBarrier.size = entity->GetInterpolatedTransformsStorage().GetBuffer().memory->size;
		memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		computeBuffer->BufferMemoryBarrier(
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			1, &memBarrier
		);

	}


	memBarrier = {};
	memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	memBarrier.buffer = entity->GetInterpolatedTransformsStorage().GetBuffer().handle;
	memBarrier.size = entity->GetInterpolatedTransformsStorage().GetBuffer().memory->size;
	memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	computeBuffer->BufferMemoryBarrier(
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		1, &memBarrier
	);


	processLocalsPassPipeline->Bind(computeBuffer);
	processLocalsPassPipeline->BindDescriptorSet(computeBuffer, entity->GetProcessingPassDescriptorSet());

	computeBuffer->Dispatch(model.GetSkeleton()->GetBoneCount() / WorkGroupCount, 1, 1);

	memBarrier = {};
	memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	memBarrier.buffer = entity->GetBoneMatrixStorage().GetBuffer().handle;
	memBarrier.size = entity->GetBoneMatrixStorage().GetBuffer().memory->size;
	memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	computeBuffer->BufferMemoryBarrier(
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0,
		1, &memBarrier
	);


	//If blending??

	finalPassPipeline->Bind(computeBuffer);
	finalPassPipeline->BindDescriptorSet(computeBuffer, entity->GetFinalPassDescriptorSet());

	computeBuffer->Dispatch(model.GetSkeleton()->GetBoneCount() / WorkGroupCount, 1, 1);

	memBarrier = {};
	memBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	memBarrier.buffer = entity->GetTransformedHierarchy().GetBuffer().handle;
	memBarrier.size = entity->GetTransformedHierarchy().GetBuffer().memory->size;
	memBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
	memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	// Compute and graphics queue may have different queue families
	// For the barrier to work across different queues, we need to set their family indices
	memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	computeBuffer->BufferMemoryBarrier(
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
		0,
		1, &memBarrier
	);

	return *entity;
}
//*/

