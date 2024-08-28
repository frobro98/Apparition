// Copyright 2020, Nathan Blane

#include "LightingPipeline.hpp"
#include "Scene/GBufferTargets.hpp"
#include "Scene/SceneRenderTargets.hpp"
#include "Graphics/RenderContext.hpp"
#include "Graphics/RenderPassAttachments.hpp"
#include "Graphics/ResourceInitializationDescriptions.hpp"
#include "Scene/ScreenView.hpp"
#include "Scene/Scene.hpp"
#include "Lighting/Light.hpp"
#include "Math/MatrixFunctions.hpp"
#include "Shader/Shader.hpp"
#include "Shader/ShaderResource.hpp"

// constexpr u32 ShadowMapWidth = 1024;
// constexpr u32 ShadowMapHeight = 1024;
// 
// static void SetupLightRender(RenderContext& renderer, const GBuffer& gbuffer)
// {
// 	ShaderID deferredVertexID = Shader::FindOrLoadShaderFile("DeferredViewRender.mvs");
// 	ShaderID deferredFragmentID = Shader::FindOrLoadShaderFile("DeferredBlinn.mfs");
// 
// 	ShaderResource* vertShader = Shader::FindAssociatedShaderResource(deferredVertexID);
// 	ShaderResource* fragShader = Shader::FindAssociatedShaderResource(deferredFragmentID);
// 
// 	GraphicsPipelineDescription pipelineDesc = {};
// 	renderer.InitializeWithRenderState(pipelineDesc);
// 	pipelineDesc.vertexInputs = {};
// 	pipelineDesc.rasterizerDesc = { 1.25f, 1.75f, FillMode::Full, CullingMode::Front };
// 	pipelineDesc.blendingDescs[0] = BlendDesc<ColorMask::RGB, BlendOperation::Add, BlendFactor::One, BlendFactor::One, BlendOperation::Add, BlendFactor::One, BlendFactor::One>();
// 	pipelineDesc.depthStencilTestDesc = DepthTestDesc<CompareOperation::LessThanOrEqual, false>();
// 	pipelineDesc.topology = PrimitiveTopology::TriangleList;
// 	pipelineDesc.vertexShader = vertShader->GetVertexShader();
// 	pipelineDesc.fragmentShader = fragShader->GetFragmentShader();
// 	renderer.SetGraphicsPipeline(pipelineDesc);
// 
// 	// TODO - Render Targets should also a sampler associated with them when setting them up potentially...
// 	renderer.SetTexture(*gbuffer.positionTexture->nativeTarget, *SamplerDesc(), 0);
// 	renderer.SetTexture(*gbuffer.normalTexture->nativeTarget, *SamplerDesc(), 1);
// 	renderer.SetTexture(*gbuffer.diffuseTexture->nativeTarget, *SamplerDesc(), 2);
// }
// 
// static void RenderLight(RenderContext& renderer, Light& light, const View& view)
// {	
// 	LightDescription lightDesc = light.GetLightDescription();
// 	LightProperties lightProps = {};
// 	lightProps.direction = -light.GetDirection();
// 	lightProps.position = lightDesc.position;
// 	//lightProps.lightViewTransform = Math::ConstructViewMatrix(light.GetPosition(), Vector4::Zero, Vector4::UpAxis);
// 	lightProps.lightProjection = Math::ConstructPerspectiveMatrix(90.f, (float)ShadowMapWidth / (float)ShadowMapHeight, .1f, 1000.);
// 
// 	GetGraphicsInterface().PushBufferData(light.GetLightBuffer(), &lightProps);
// 
// 	renderer.SetUniformBuffer(*view.viewBuffer, 3);
// 	renderer.SetUniformBuffer(light.GetLightBuffer(), 4);
// 	renderer.Draw(3, 1);
// }

