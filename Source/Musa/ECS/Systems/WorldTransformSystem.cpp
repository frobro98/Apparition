// Copyright 2020, Nathan Blane

#include "WorldTransformSystem.hpp"
//#include "ECS/Query.hpp"
//#include "ECS/World.hpp"
//
//#include "ECS/Components/TranslationComponent.hpp"
//#include "ECS/Components/RotationComponent.hpp"
//#include "ECS/Components/ScaleComponent.hpp"
//#include "ECS/Components/WorldTransformComponent.hpp"
//
//using namespace Musa;
//
//void WorldTransformSystem::Initialize()
//{
//	noWorldEntities = &GetQueryFor(
//		DescribeQuery()
//		.OneOrMore<TranslationComponent, RotationComponent, ScaleComponent>()
//		.None<WorldTransformComponent>()
//	);
//
//	worldEntities = &GetQueryFor<WorldTransformComponent>();
//}
//
//void WorldTransformSystem::Update()
//{
//	// TODO - Should be its own system!
//	UpdateEntitiesWithNoTransform();
//	UpdateEntitiesWithTransform();
//}
//
//void WorldTransformSystem::UpdateEntitiesWithNoTransform()
//{
//	// TODO - This should be its own system! Then there isn't any duplication of code for adding WorldComponent and updating it
//	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(*noWorldEntities);
//	for (auto& chunk : chunks)
//	{
//		ChunkArray<Entity> entities = chunk.GetArrayOf<Entity>();
//		for(const auto& entity : entities)
//		{
//			GetWorld().AddComponentTo<WorldTransformComponent>(entity);
//		}
//	}
//}
//
//void WorldTransformSystem::UpdateEntitiesWithTransform()
//{
//	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(*worldEntities);
//	for (auto& chunk : chunks)
//	{
//		bool chunkChanged = chunk.HasChanged<TranslationComponent>(GetVersion()) ||
//			chunk.HasChanged<RotationComponent>(GetVersion()) ||
//			chunk.HasChanged<ScaleComponent>(GetVersion());
//
//		if (chunkChanged)
//		{
//			ChunkArray<const TranslationComponent> translations = chunk.GetArrayOf<const TranslationComponent>();
//			ChunkArray<const RotationComponent> rotations = chunk.GetArrayOf<const RotationComponent>();
//			ChunkArray<const ScaleComponent> scales = chunk.GetArrayOf<const ScaleComponent>();
//			ChunkArray<WorldTransformComponent> transforms = chunk.GetArrayOf<WorldTransformComponent>();
//
//			u32 entityCount = chunk.GetEntityCount();
//			bool transExists = translations.IsValid();
//			bool rotExists = rotations.IsValid();
//			bool scaleExists = scales.IsValid();
//
//			if (!scaleExists)
//			{
//				if (transExists && rotExists)
//				{
//					for (u32 i = 0; i < entityCount; ++i)
//					{
//						transforms[i].transform = Matrix4(rotations[i].rotation) *
//							Matrix4(TRANS, Vector4(translations[i].translation));
//					}
//				}
//				else if (transExists && !rotExists)
//				{
//					for (u32 i = 0; i < entityCount; ++i)
//					{
//						transforms[i].transform = Matrix4(TRANS, Vector4(translations[i].translation));
//					}
//				}
//				else if (!transExists && rotExists)
//				{
//					for (u32 i = 0; i < entityCount; ++i)
//					{
//						transforms[i].transform = Matrix4(rotations[i].rotation);
//					}
//				}
//			}
//			else // Scale exists
//			{
//				if (transExists && rotExists)
//				{
//					for (u32 i = 0; i < entityCount; ++i)
//					{
//						transforms[i].transform = Matrix4(SCALE, Vector4(scales[i].scale)) *
//							Matrix4(rotations[i].rotation) *
//							Matrix4(TRANS, Vector4(translations[i].translation));
//					}
//				}
//				else if (transExists && !rotExists)
//				{
//					for (u32 i = 0; i < entityCount; ++i)
//					{
//						transforms[i].transform = Matrix4(SCALE, Vector4(scales[i].scale)) *
//							Matrix4(TRANS, Vector4(translations[i].translation));
//					}
//				}
//				else if (!transExists && rotExists)
//				{
//					for (u32 i = 0; i < entityCount; ++i)
//					{
//						transforms[i].transform = Matrix4(SCALE, Vector4(scales[i].scale)) *
//							Matrix4(rotations[i].rotation);
//					}
//				}
//				else // Just scale
//				{
//					for (u32 i = 0; i < entityCount; ++i)
//					{
//						transforms[i].transform = Matrix4(SCALE, Vector4(scales[i].scale));
//					}
//				}
//			}
//		}
//	}
//}
