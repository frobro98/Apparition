#include "Engine.h"
#include "Math/MathEngine.h"
#include "Camera/CameraManager.h"
#include "Camera/Camera.h"

#include "GameObject/GameObjectManager.h"
#include "GameObject/GameObject.h"
#include "Model/Model.h"
#include "Texture2D/TextureManager.h"
#include "Texture2D/Texture.h"
#include "Input/Input.hpp"
#include "Windowing/Window.h"
//#include "Animation/AnimationTimerManager.h"
#include "Scene/GameWorld.hpp"

#include "Renderer/RenderingEngine.h"
#include "Graphics/GraphicsInterface.hpp"

#include "ECS/SystemsManager.hpp"

#include "Thread/JobSystem/JobSystem.hpp"
#include "Thread/JobSystem/JobThread.hpp"
#include "Thread/JobSystem/JobUtilities.hpp"
#include "Thread/JobSystem/Job.hpp"

using namespace Musa;

// TODO - Remove anything referencing vulkan...
#include "Graphics/Vulkan/VulkanShader.h"
#include "Graphics/Vulkan/VulkanShaderManager.h"
#include "Graphics/Vulkan/VulkanDescriptorSet.h"
#include "Graphics/Vulkan/VulkanDevice.h"
#include "Graphics/Vulkan/VulkanPipeline.h"

void Engine::Run()
{
	InitializeJobSystem();

	InitializeTime();

	InitializeWindow();

	GetGraphicsInterface().InitializeGraphics(window->GetWindowHandle(), width, height);

	InitializeWorld();

	InitializeECS();

	Init();

	LoadContent();

	EngineLoop();

	GetJobSystem().Deinitialize();

	UnloadContent();

	DeInit();

	delete window;
}

void Engine::EngineLoop()
{
	while (window->IsActive())
	{
		tickTimer.Lap();
		Time elapsedTime = internalTimer.Toc();
		float tick = tickTimer.GetMilliseconds();

		CalculateFrameTime();

		// TODO - This calls functions that process input on game objects right away instead of within the job system
		// This must be fixed
		GetInputManager().Update();

		SetActiveJobParent(*updateJob);

		//AnimationTimerManager::Update(elapsedTime);

		// TODO - Need to have some sort of frame tick
		UpdateEngine(tick);

		GetJobSystem().WaitOnJob(*updateJob);

		internalTimer.Tic();
		tickTimer.Start();

		SetActiveJobParent(*pushStateJob);

		PrepareStateForRender();

		GetJobSystem().WaitOnJob(*pushStateJob);

		SetActiveJobParent(*renderJob);

		Render();

		GetJobSystem().WaitOnJob(*renderJob);

		GetJobSystem().EndOfFrame();
	}
}

float prevMouseX, prevMouseY = 0.f;
float mouseX, mouseY = 0.f;

void Engine::UpdateEngine(float tick)
{
	//GetSystemsManager().ProcessDirtyEntities();
	//GetSystemsManager().TickSystems(tick);
	
	PushConcurrentJobUnder(
		GetActiveJobParent(),
		[&, tick]
		{
			world->TickWorld(tick);
			UpdateGame(tick);
		}
	);
}

void Engine::PrepareStateForRender()
{
	PushConcurrentJobUnder(
		GetActiveJobParent(),
		[&]
		{
			world->PushToRenderState();
		}
	);
}

void Engine::Render()
{
	PushConcurrentJobUnder(
		GetActiveJobParent(),
		[&]
		{
			world->RenderWorld();
		}
	);
}

void Engine::InitializeECS()
{
	GetSystemsManager().Initialize();
}

void Engine::InitializeWorld()
{
	// TODO - This probably will need to be a lot more than just creating a damn World class
	world = new GameWorld();
	world->Initialize(*window);

	GetGameObjectManager().Initialize(*world);
}


void Engine::InitializeTime()
{
	EngineTick::Init();

	tickTimer.Start();
	globalTimer.Tic();
	internalTimer.Tic();
}

void Engine::InitializeJobSystem()
{
	JobSystem& jobSystem = GetJobSystem();

	jobSystem.Initialize();

	updateJob = jobSystem.CreateEmptyJob();
	cullJob = jobSystem.CreateEmptyJob();
	pushStateJob = jobSystem.CreateEmptyJob();
	renderJob = jobSystem.CreateEmptyJob();
	
	jobSystem.StartSystem();
}

void Engine::CalculateFrameTime() const
{
	static float sec = 0.f;
	sec += tickTimer.GetSeconds();
	
	if (sec > 1.f)
	{
		float fps = 1.f / tickTimer.GetSeconds();
		printf("FPS: %f\n", fps);
		//SetWindowText(mhMainWnd, outs.str().c_str());
		sec = 0;
	}
}

void Engine::InitializeWindow()
{
	// TODO - Move this console set up somewhere else
	FILE* stdOut = nullptr;
	{
		AllocConsole();
		freopen_s(&stdOut, "CONOUT$", "w", stdout);
	}

	width = 1080;
	height = 720;

	
	window = new Window(width, height);
	window->Initialize();
	window->SetAsActiveWindow();

	GetInputManager().Initialize(*window);
}