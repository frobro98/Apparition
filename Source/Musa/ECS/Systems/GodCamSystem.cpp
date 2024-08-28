// Copyright 2020, Nathan Blane

#include "GodCamSystem.hpp"
//#include "ECS/World.hpp"
//
//#include "ECS/Components/GameInputComponent.hpp"
//#include "ECS/Components/CameraComponent.hpp"
//#include "ECS/Components/TranslationComponent.hpp"
//#include "ECS/Components/RotationComponent.hpp"
//
//#include "Engine/FrameData.hpp"
//#include "Math/MatrixFunctions.hpp"
//
//#include "Input/Input.hpp"
//
//DEFINE_LOG_CHANNEL(GodCam);
//
//using namespace Musa;
//
//void GodCamSystem::Initialize()
//{
//	missingTransRotCamQuery = &GetQueryFor(
//		DescribeQuery()
//		.Require<CameraComponent, GameInputComponent>()
//		.None<TranslationComponent, RotationComponent>()
//	);
//
//	missingTransCamQuery = &GetQueryFor(
//		DescribeQuery()
//		.Require<CameraComponent, GameInputComponent, RotationComponent>()
//		.None<TranslationComponent>()
//	);
//
//	missingRotCamQuery = &GetQueryFor(
//		DescribeQuery()
//		.Require<CameraComponent, GameInputComponent, TranslationComponent>()
//		.None<RotationComponent>()
//	);
//
//	camsWithInputQuery = &GetQueryFor(
//		DescribeQuery()
//		.Require<
//		CameraComponent, 
//		GameInputComponent, 
//		TranslationComponent, 
//		RotationComponent
//		>()
//	);
//}
//
//void GodCamSystem::Update()
//{
//	AddMissingTransformComponents();
//	ConvertInputToCameraMovement();
//}
//
//void GodCamSystem::AddMissingTransformComponents()
//{
//	// Add both translation and rotation
//	auto noTransRotChunks = GetQueryChunks(*missingTransRotCamQuery);
//	for (auto& noTRChunk : noTransRotChunks)
//	{
//		auto entities = noTRChunk.GetArrayOf<Entity>();
//		for (auto entity : entities)
//		{
//			// TODO - Add multiple components through one call
//			GetWorld().AddComponentTo<TranslationComponent>(entity);
//			// TODO - New components are currently not constructed, which is a problem
//			GetWorld().SetComponentDataOn(entity, TranslationComponent{ {}, Vector3() });
//
//			GetWorld().AddComponentTo<RotationComponent>(entity);
//			// TODO - New components are currently not constructed, which is a problem
//			GetWorld().SetComponentDataOn(entity, RotationComponent{ {}, Quat() });
//		}
//	}
//
//	// Add translation
//	auto noTransChunks = GetQueryChunks(*missingTransCamQuery);
//	for (auto& noTChunk : noTransChunks)
//	{
//		auto entities = noTChunk.GetArrayOf<Entity>();
//		for (auto entity : entities)
//		{
//			GetWorld().AddComponentTo<TranslationComponent>(entity);
//			// TODO - New components are currently not constructed, which is a problem
//			GetWorld().SetComponentDataOn(entity, TranslationComponent{ {}, Vector3() });
//		}
//	}
//
//	// Add rotation
//	auto noRotChunks = GetQueryChunks(*missingRotCamQuery);
//	for (auto& noRChunk : noRotChunks)
//	{
//		auto entities = noRChunk.GetArrayOf<Entity>();
//		for (auto entity : entities)
//		{
//			GetWorld().AddComponentTo<RotationComponent>(entity);
//			// TODO - New components are currently not constructed, which is a problem
//			GetWorld().SetComponentDataOn(entity, RotationComponent{ {}, Quat() });
//		}
//	}
//}
//
//void GodCamSystem::ConvertInputToCameraMovement()
//{
//	auto camsWithInputChunks = GetQueryChunks(*camsWithInputQuery);
//	const f32 tickSec = (f32)Frame::GetTickTimeSeconds();
//
//	for (auto& camChunk : camsWithInputChunks)
//	{
//		if (camChunk.HasChanged<GameInputComponent>(GetVersion()))
//		{
//			ChunkArray<const GameInputComponent> inputs = camChunk.GetArrayOf<const GameInputComponent>();
//			Assert(inputs.IsValid());
//
//			auto translations = camChunk.GetArrayOf<TranslationComponent>();
//			Assert(translations.IsValid());
//			auto rotations = camChunk.GetArrayOf<RotationComponent>();
//			Assert(rotations.IsValid());
//
//			for (u32 i = 0; i < camChunk.GetEntityCount(); ++i)
//			{
//				const GameInputComponent& input = inputs[i];
//				TranslationComponent& translation = translations[i];
//				RotationComponent& rotation = rotations[i];
//				Matrix4 view = Math::ConstructViewMatrix(Vector4(translation.translation, 1.f), rotation.rotation);
//				Vector4 forward(view.m2(), view.m6(), view.m10(), 0);
//				Vector4 right(view.m0(), view.m4(), view.m8(), 0);
//				Vector4 up(view.m1(), view.m5(), view.m9(), 0);
//
//				f32 forwardDelta = 0.f;
//				f32 rightDelta = 0.f;
//				f32 upDelta = 0.f;
//				f32 mouseX = 0.f;
//				f32 mouseY = 0.f;
//				constexpr f32 speed = 1.f;
//				constexpr f32 lookSensitivity = 10.f;
//
//				for (const auto& state : input.inputStates)
//				{
//					if (state.input == Input::Mouse_XAxis)
//					{
//						mouseX += state.value * lookSensitivity;
//						//MUSA_DEBUG(GodCam, "Mouse X Value: {}", mouseX);
//					}
//					else if (state.input == Input::Mouse_YAxis)
//					{
//						mouseY += state.value * lookSensitivity;
//						//MUSA_DEBUG(GodCam, "Mouse Y Value: {}", mouseY);
//					}
//// 					else if (state.input == Input::Key_W)
//// 					{
//// 						forwardDelta += state.value * speed;
//// 					}
//// 					else if (state.input == Input::Key_A)
//// 					{
//// 						rightDelta -= state.value * speed;
//// 					}
//// 					else if (state.input == Input::Key_E)
//// 					{
//// 						upDelta += state.value * speed;
//// 					}
//
//					if (Input::IsDown(Input::Key_W))
//					{
//						forwardDelta += speed * tickSec;
//					}
//					if (Input::IsDown(Input::Key_S))
//					{
//						forwardDelta -= speed * tickSec;
//					}
//
//					if (Input::IsDown(Input::Key_A))
//					{
//						rightDelta -= speed * tickSec;
//					}
//					if (Input::IsDown(Input::Key_E))
//					{
//						upDelta += speed * tickSec;
//					}
//				}
//
//				const f32 tick = Frame::GetTickTimeSeconds();
//
//				// Rotation application
//				if(mouseX != 0.f || mouseY != 0.f)
//				{
//					constexpr f32 lookDelta = .05f;
//					const f32 lookSpeed = lookDelta * tick;
//
//					Quat quatX(ROT_AXIS_ANGLE, right, -mouseY * lookSpeed);
//					Quat quatY(ROT_Y, -mouseX * lookSpeed);
//					Quat lookRotation = quatX * quatY;
//					rotation.rotation *= lookRotation;
//
//					forward *= lookRotation;
//					forward.Normalize();
//					right *= lookRotation;
//					right.Normalize();
//					up *= lookRotation;
//					up.Normalize();
//				}
//
//				// Translation application
//				//constexpr f32 speedDelta = .005f;
//				//const f32 moveSpeed = speedDelta * tick;
//				Vector4 trans = Vector4(translation.translation);
//				trans -= forward * forwardDelta;
//				trans += right * rightDelta;
//				trans += up * upDelta;
//				translation.translation = Vector3(trans);
//			}
//		}
//	}
//}
