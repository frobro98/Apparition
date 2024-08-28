// Copyright 2020, Nathan Blane

#include "OSInputSystem.hpp"
//#include "ECS/World.hpp"
//#include "ECS/Components/OSInputsComponent.hpp"
//#include "Input/ApplicationInputMap.hpp"
//
//using namespace Musa;
//
//OSInputSystem::OSInputSystem(ApplicationInputMap& inputHandler_)
//	: inputHandler(inputHandler_)
//{
//}
//
//void OSInputSystem::Initialize()
//{
//	GetWorld().CreateEntity<OSInputsComponent>();
//	globalInput = &GetQueryFor(DescribeQuery().Require<OSInputsComponent>());
//}
//
//void OSInputSystem::Update()
//{
//	auto chunks = GetQueryChunks(*globalInput);
//	Assert(chunks.Size() == 1);
//	ChunkComponentAccessor& chunk = chunks[0];
//	auto inputComponents = chunk.GetArrayOf<OSInputsComponent>();
//	Assert(inputComponents.IsValid());
//	Assert(inputComponents.size == 1);
//
//	OSInputsComponent input = inputComponents[0];
//
//	inputHandler.CopyKeyboardMouseInput(input.inputMap);
//	input.currentMousePosition = inputHandler.GetMousePosition();
//	input.modFlags = inputHandler.GetModifierFlags();
//	inputHandler.CopyGamepadStates(input.gamepads, input.activeGamepads);
//}
