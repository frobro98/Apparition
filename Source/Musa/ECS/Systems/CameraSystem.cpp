// Copyright 2020, Nathan Blane

#include "CameraSystem.hpp"
#include "Math/MatrixFunctions.hpp"
//#include "ECS/World.hpp"
//
//#include "ECS/Components/CameraComponent.hpp"
//#include "ECS/Components/ViewTransformComponent.hpp"
//#include "ECS/Components/TranslationComponent.hpp"
//#include "ECS/Components/RotationComponent.hpp"
//
//using namespace Musa;
//
//void CameraSystem::Initialize()
//{
//	noTransRotComps = &GetQueryFor(
//		DescribeQuery()
//		.Require<CameraComponent>()
//		.None<TranslationComponent, RotationComponent>()
//	);
//
//	noViewTransforms = &GetQueryFor(
//		DescribeQuery()
//		.Require<CameraComponent>()
//		.None<ViewTransformComponent>()
//	);
//
//	viewTransformsExist = &GetQueryFor(
//		DescribeQuery()
//		.Require<ViewTransformComponent>()
//		.OneOrMore<TranslationComponent, RotationComponent>()
//	);
//}
//
//void CameraSystem::Update()
//{
//	AddMissingTransformComponents();
//	AddMissingViewTransforms();
//	UpdateViewTransforms();
//}
//
//void CameraSystem::AddMissingTransformComponents()
//{
//	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(*noTransRotComps);
//	for (auto& chunk : chunks)
//	{
//		ChunkArray<Entity> entities = chunk.GetArrayOf<Entity>();
//
//		for (const auto& entity : entities)
//		{
//			// TODO - Make a function that adds multiple components to an entity
//			GetWorld().AddComponentTo<TranslationComponent>(entity);
//			GetWorld().AddComponentTo<RotationComponent>(entity);
//		}
//	}
//}
//
//void CameraSystem::AddMissingViewTransforms()
//{
//	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(*noViewTransforms);
//	for (auto& chunk : chunks)
//	{
//		ChunkArray<Entity> entities = chunk.GetArrayOf<Entity>();
//
//		for (const auto& entity : entities)
//		{
//			GetWorld().AddComponentTo<ViewTransformComponent>(entity);
//		}
//	}
//}
//
//void CameraSystem::UpdateViewTransforms()
//{
//	auto chunks = GetQueryChunks(*viewTransformsExist);
//	for (auto& chunk : chunks)
//	{
//		bool hasChanged = chunk.HasChanged<TranslationComponent>(GetVersion()) ||
//						chunk.HasChanged<RotationComponent>(GetVersion());
//
//		if (hasChanged)
//		{
//			ChunkArray<const TranslationComponent> translations = chunk.GetArrayOf<const TranslationComponent>();
//			ChunkArray<const RotationComponent> rotations = chunk.GetArrayOf<const RotationComponent>();
//			ChunkArray<ViewTransformComponent> transforms = chunk.GetArrayOf<ViewTransformComponent>();
//			u32 entityCount = chunk.GetEntityCount();
//
//			bool translationExists = translations.IsValid();
//			bool rotationExists = rotations.IsValid();
//
//			if (translationExists && rotationExists)
//			{
//				for (u32 i = 0; i < entityCount; ++i)
//				{
//					const TranslationComponent& translation = translations[i];
//					const RotationComponent& rotation = rotations[i];
//					ViewTransformComponent& transform = transforms[i];
//
//					Matrix4 view = Math::ConstructViewMatrix(Vector4(translation.translation, 1), rotation.rotation);
//					transform.transform = view;
//					transform.invTransform = Math::ConstructFastInverseViewMatrix(Vector4(translation.translation, 1), rotation.rotation);
//
//					transform.right = Vector4(view.m0(), view.m4(), view.m8(), 0);
//					transform.up = Vector4(view.m1(), view.m5(), view.m9(), 0);
//					transform.forward = Vector4(view.m2(), view.m6(), view.m10(), 0);
//				}
//			}
//			else if (translationExists) // No rotation
//			{
//				for (u32 i = 0; i < entityCount; ++i)
//				{
//					const TranslationComponent& translation = translations[i];
//					ViewTransformComponent& transform = transforms[i];
//
//					Matrix4 view = Math::ConstructViewMatrix(Vector4(translation.translation, 1), Quat::Identity);
//					transform.transform = view;
//					transform.invTransform = Math::ConstructFastInverseViewMatrix(Vector4(translation.translation, 1), Quat::Identity);
//
//					transform.right = Vector4::RightAxis;
//					transform.up = Vector4::UpAxis;
//					transform.forward = Vector4::ForwardAxis;
//				}
//			}
//			else if (rotationExists) // No translation
//			{
//				for (u32 i = 0; i < entityCount; ++i)
//				{
//					const RotationComponent& rotation = rotations[i];
//					ViewTransformComponent& transform = transforms[i];
//
//					Matrix4 view = Math::ConstructViewMatrix(Vector4::Zero, rotation.rotation);
//					transform.transform = view;
//					transform.invTransform = Math::ConstructFastInverseViewMatrix(Vector4::Zero, rotation.rotation);
//
//					transform.right = Vector4(view.m0(), view.m4(), view.m8(), 0);
//					transform.up = Vector4(view.m1(), view.m5(), view.m9(), 0);
//					transform.forward = Vector4(view.m2(), view.m6(), view.m10(), 0);
//				}
//			}
//		}
//	}
//}
