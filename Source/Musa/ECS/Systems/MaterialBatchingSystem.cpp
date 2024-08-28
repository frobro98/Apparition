// Copyright 2020, Nathan Blane

#include "MaterialBatchingSystem.hpp"
//#include "ECS/World.hpp"
//
//#include "ECS/Components/MeshRenderComponent.hpp"
//#include "ECS/Components/WorldTransformComponent.hpp"
//#include "ECS/Components/RenderDataComponent.hpp"

#include "Mesh/Mesh.h"
#include "Shader/Material.hpp"
#include "Graphics/UniformBuffers.h"
#include "Graphics/GraphicsInterface.hpp"
#include "RenderPipeline/MaterialPrimitiveBatches.hpp"

// RenderComponent contains mesh and material data. This data needs to be pushed to the
// actual rendering system path. Currently, I can't do this, so I need to add some code
// in the render pipeline to actually accept the data from this ECS system...

// Steps that currently happen in the deferred render pipeline:
//	1) After update, push updated data to the corresponding RenderObject 
//	   within the RenderObjectManager. See RenderObjectManager::SequenciallyPull
//	2) Render starts
//	3) Gets the RenderContext, "screen" view, and the RenderObjectManager. These are used within
//	   the main render of the scene objects. The RenderObjectManager isn't used outside of rendering
//	   the scene
//	4) The context marks the beginning of the frame, which sets up the frame for rendering. It takes in
//	   the scene's viewport
//	5) The main render pass starts now. This goes and renders the scene to the GBuffer, then uses it to 
//	   render lighting to the scene color buffer
//	6) After the scene is rendered to the color buffer, the UI gets processed. Currently, this isn't a real
//	   implementation of UI in general, so this will hopefully change along with ECS being integrated within
//	   the rendering. The spirit will stay the same, which is rendering the UI to a single color texture that
//	   only gets rendered to when the UI changes.
//	7) Next is the composition of the back buffer. Both the UI color buffer and the scene color buffer are 
//	   rendered onto the back buffer in preperation for presentation
//	8) The context now marks the end of the frame, which means the frame attempts to present itself to the window

// Looking at the non-ECS way of doing things right now, what these rendering systems seek to do is to replace
// the RenderObjectManager::SequentiallyPull function and RenderObjects in general, add culling to the render,
// and clean up relationship between rendering and updating. The last part involves giving render items to the
// main render with all of the information that they would need, without touching the mesh or material (CPU data...)

// To execute this, currently, there would be 2 systems involved: Culling and Rendering. The CullingSystem would be 
// the first to update and it essentially removes the entities from consideration for the RenderingSystem. It does this
// by adding a "Culled" empty component

// We have access to all of the chunks here. We should transform the data within each component into 
// data that can be submitted to the render pipeline

// Either we submit each component to the render pipeline or submit packets of render data to the render pipeline to use
// If the former is developed, it will keep this system very simple. If the latter is developed, this system handles 
// most of the complexity of the render when it comes to sorting and batching. This is the ideal situation definitely

// It makes sense to keep mesh data and material data coupled, in a way, because a mesh, to be drawn, needs a material.
// This will prevent a bunch of context switching and keep things relatively fast. The likelihood of the same material
// being used by multiple objects, at least in 3D, is minimal I'd say. If there are multiple instances of a mesh and
// material, that just would use GPU instancing down the line anyways

// Need to sort transparency vs opaque, which essentially depends on the material itself. If the material allows for 
// transparency, then it goes in that bucket of RenderComponents, which eventually gets sorted back to front. The 
// opaque geometry would then get sorted by material, or more accurately, its properties, so as to not switch context 
// for each kind of material

//using namespace Musa;
//
//void MaterialBatchingSystem::Initialize()
//{
//	// TODO - Initialize material primitive group!
//
//	noRenderDataQuery = &GetQueryFor(
//		DescribeQuery()
//		.Require<WorldTransformComponent, MeshRenderComponent>()
//		.None<RenderDataComponent>()
//	);
//
//	updateRenderDataQuery = &GetQueryFor(
//		DescribeQuery()
//		.Require<WorldTransformComponent, MeshRenderComponent, RenderDataComponent>()
//	);
//
//	// TODO - Delete the RenderDataComponent if there are entities with no worldtransformcomponent or no meshrendercomponent
//}
//
//void MaterialBatchingSystem::Update()
//{
//	AddMissingRenderData();
//	UpdateRenderData();
//}
//
//void MaterialBatchingSystem::AddMissingRenderData()
//{
//	// Get chunks associated with the query and loop through them, processing them
//	DynamicArray<ChunkComponentAccessor> noRenderDataChunks = GetQueryChunks(*noRenderDataQuery);
//	for (auto& chunk : noRenderDataChunks)
//	{
//		const ChunkArray<Entity> entities = chunk.GetArrayOf<Entity>();
//		for (const auto& entity : entities)
//		{
//			NativeUniformBuffer* primBuff = GetGraphicsInterface().CreateUniformBuffer(sizeof(PrimUniformBuffer));
//			NativeUniformBuffer* matPropsBuff = GetGraphicsInterface().CreateUniformBuffer(sizeof(MaterialProperties));
//
//			GetWorld().AddComponentTo<RenderDataComponent>(entity, RenderDataComponent{ {},
//				MOVE(primBuff),
//				MOVE(matPropsBuff)
//				});
//		}
//	}
//}
//
//void MaterialBatchingSystem::UpdateRenderData()
//{
//	auto queriedChunks = GetQueryChunks(*updateRenderDataQuery);
//	ArrayView<Entity> entities = GetQueriedEntities(*updateRenderDataQuery);
//
//	for (auto& chunk : queriedChunks)
//	{
//		// TODO - This body could essentially be a function in its own right. The user doesn't need to know about an array of chunks, potentially...
//		auto renderDataComps = chunk.GetArrayOf<RenderDataComponent>();
//		Assert(renderDataComps.IsValid());
//
//		auto renderComps = chunk.GetArrayOf<const MeshRenderComponent>();
//		Assert(renderComps.IsValid());
//
//		if (chunk.HasChanged<MeshRenderComponent>(GetVersion()))
//		{
//			for (u32 i = 0; i < renderDataComps.size; ++i)
//			{
//				RenderDataComponent& component = renderDataComps[i];
//				MaterialProperties props;
//				props.diffuse = renderComps[i].material->GetColor();
//				GetGraphicsInterface().PushBufferData(*component.materialPropsUniformBuffer, &props);
//			}
//		}
//		if (chunk.HasChanged<WorldTransformComponent>(GetVersion()))
//		{
//			auto transformComps = chunk.GetArrayOf<const WorldTransformComponent>();
//			Assert(transformComps.IsValid());
//
//			for (u32 i = 0; i < renderDataComps.size; ++i)
//			{
//				RenderDataComponent& component = renderDataComps[i];
//				PrimUniformBuffer prim;
//				prim.model = transformComps[i].transform;
//				GetGraphicsInterface().PushBufferData(*component.primUniformBuffer, &prim);
//			}
//		}
//	}
//}
