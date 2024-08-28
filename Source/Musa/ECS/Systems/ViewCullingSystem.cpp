// Copyright 2020, Nathan Blane

#include "ViewCullingSystem.hpp"
#include "Math/MatrixFunctions.hpp"
#include "RenderPipeline/RenderPipelineConfig.hpp"
#include "RenderPipeline/RenderPipeline.hpp"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/UniformBuffers.h"
#include "Mesh/Mesh.h"
#include "Shader/Material.hpp"

//#include "ECS/Components/CameraComponent.hpp"
//#include "ECS/Components/ViewTransformComponent.hpp"
//#include "ECS/Components/RenderDataComponent.hpp"
//#include "ECS/Components/TranslationComponent.hpp"
//#include "ECS/Components/MeshRenderComponent.hpp"
//
//using namespace Musa;
//
//void ViewCullingSystem::Initialize()
//{
//	viewsIntoScene = &GetQueryFor(
//		DescribeQuery()
//		.Require<CameraComponent, ViewTransformComponent>()
//	);
//
//	renderPrimsToCull = &GetQueryFor(
//		DescribeQuery()
//		.Require<MeshRenderComponent, RenderDataComponent>()
//	);
//}
//
//void ViewCullingSystem::Update()
//{
//	DynamicArray<RenderView*> views;
//	CollectAllViewsIntoWorld(views);
//	CullPrimitivesPerView(views);
//
//	// TODO - Now all of the views have been populated. Now they need to be accessible by the pipeline itself.
//	// Put them in a common place so that they can be passed into the pipeline render
//	Musa::GetRenderPipeline().HandOffRenderViews(MOVE(views));
//}
//
//void ViewCullingSystem::CollectAllViewsIntoWorld(DynamicArray<RenderView*>& views) const
//{
//	auto chunks = GetQueryChunks(*viewsIntoScene);
//	for (auto& chunk : chunks)
//	{
//		auto cameras = chunk.GetArrayOf<const CameraComponent>();
//		auto transforms = chunk.GetArrayOf<const ViewTransformComponent>();
//		auto translations = chunk.GetArrayOf<const TranslationComponent>();
//
//		Assert(cameras.IsValid());
//		Assert(transforms.IsValid());
//
//		bool translationsValid = translations.IsValid();
//		u32 entityCount = chunk.GetEntityCount();
//		for (u32 i = 0; i < entityCount; ++i)
//		{
//			const CameraComponent& camera = cameras[i];
//			const ViewTransformComponent& transform = transforms[i];
//
//			RenderView* view = new RenderView;
//
//			view->clearColor = NormalizeColor(camera.clearColor);
//			view->normViewport = camera.viewport;
//
//			Matrix4 projectionMat;
//			Matrix4 invProjection;
//			if (camera.projection == ProjectionType::Perspective)
//			{
//				projectionMat = Math::ConstructPerspectiveMatrix(
//					camera.fov, camera.aspectRatio,
//					camera.nearPlane, camera.farPlane
//				);
//				invProjection = Math::ConstructFastInversePerspectiveMatrix(projectionMat);
//			}
//			else
//			{
//				projectionMat = Math::ConstructOrthographicMatrix(
//					(f32)camera.viewport.width, (f32)camera.viewport.height,
//					camera.nearPlane, camera.farPlane
//				);
//				invProjection = Math::ConstructFastInverseOrthographicMatrix(
//					(f32)camera.viewport.width, (f32)camera.viewport.height,
//					camera.nearPlane, camera.farPlane);
//			}
//
//			ViewTransformations& viewTransforms = view->transforms;
//			viewTransforms.worldToView = transform.transform;
//			viewTransforms.viewToWorld = transform.invTransform;
//			viewTransforms.viewToClip = projectionMat;
//			viewTransforms.clipToView = invProjection;
//			viewTransforms.worldToClip = transform.transform * projectionMat;
//			viewTransforms.clipToWorld = invProjection * transform.invTransform;
//
//			if (translationsValid)
//			{
//				view->position = Vector4(translations[i].translation, 1.f);
//			}
//			else
//			{
//				view->position = Vector4::Zero;
//			}
//			view->direction = transform.forward;
//
//			ViewUniformBuffer desc = {};
//			desc.viewTransform = viewTransforms.worldToView;
//			desc.projectionTransform = viewTransforms.viewToClip;
//			desc.viewPosition = view->position;
//			view->viewUniformBuffer = GetGraphicsInterface().CreateUniformBuffer(sizeof(ViewUniformBuffer));
//			GetGraphicsInterface().PushBufferData(*view->viewUniformBuffer, &desc);
//
//			views.Add(view);
//		}
//	}
//}
//
//void ViewCullingSystem::CullPrimitivesPerView(DynamicArray<RenderView*>& views) const
//{
//	auto chunks = GetQueryChunks(*renderPrimsToCull);
//	for (auto& view : views)
//	{
//		for (auto& chunk : chunks)
//		{
//			auto renderDataComps = chunk.GetArrayOf<const RenderDataComponent>();
//			auto meshRenderComps = chunk.GetArrayOf<const MeshRenderComponent>();
//
//			Assert(renderDataComps.IsValid());
//			Assert(meshRenderComps.IsValid());
//			
//			// Go through all mesh render components and add opaque mesh batches and
//			// transparent mesh batches that are within the view to the view
//
//			// TODO - Cull meshes per view
//
//			u32 entityCount = chunk.GetEntityCount();
//			MaterialPrimitiveBatch batch(entityCount);
//			for (u32 i = 0; i < entityCount; ++i)
//			{
//				const MeshRenderComponent& mesh = meshRenderComps[i];
//				const RenderDataComponent& renderData = renderDataComps[i];
//				const MaterialRenderDescription& renderDesc = mesh.material->GetRenderDescription();
//
//				MaterialPrimitive primitive = {};
//				primitive.vertexBuffer = &mesh.mesh->GetVertexBuffer();
//				primitive.indexBuffer = &mesh.mesh->GetIndexBuffer();
//				primitive.primitiveBuffer = renderData.primUniformBuffer;
//				primitive.materialProperties = renderData.materialPropsUniformBuffer;
//				primitive.materialResources = renderDesc.resources;
//
//				PipelineConfiguration& pipelineConfig = primitive.pipelineConfig;
//				Memcpy(pipelineConfig.shaders, sizeof(pipelineConfig.shaders), renderDesc.shaders, sizeof(renderDesc.shaders));
//				pipelineConfig.shadingModel = renderDesc.shadingModel;
//				pipelineConfig.fillMode = renderDesc.fillMode;
//				pipelineConfig.cullMode = renderDesc.cullMode;
//				pipelineConfig.blendMode = renderDesc.blendMode;
//
//				batch[i] = primitive;
//			}
//
//			view->opaqueMeshBatches.AddMaterialBatch(batch);
//		}
//	}
//}

// void ViewCullingSystem::CullLightsPerView(DynamicArray<RenderView*>& views) const
// {
// 	for (auto& view : views)
// 	{
// 
// 	}
// }
