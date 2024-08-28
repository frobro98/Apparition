#pragma once

#include "Platform.h"
#include "Graphics.h"
#include "Time/Timer.h"
#include "Time/Time.h"
#include "Time/EngineTick.h"

class Window;
class GameWorld;
struct Job;

class Engine
{
public:
	Engine() = default;
	virtual ~Engine() = default;

	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void Run();
protected:
	virtual void Init() = 0;
	virtual void LoadContent() = 0;
	virtual void UpdateGame(float deltaTime) = 0;
	virtual void UnloadContent() = 0;
	virtual void DeInit() = 0;

private:
	void UpdateEngine(float tick);
	void PrepareStateForRender();
	void Render();

	void InitializeECS();
	void InitializeWorld();
	void InitializeWindow();
	void InitializeTime();
	void InitializeJobSystem();

	void CalculateFrameTime() const;

	void EngineLoop();

// TODO - Some of this data will be moved because it shouldn't live in engine anyways.
private:
	EngineTick tickTimer;

protected:
	Timer globalTimer;
	Timer internalTimer;
	Window* window;
	GameWorld* world;

	Job* updateJob = nullptr;
	Job* cullJob = nullptr;
	Job* pushStateJob = nullptr;
	Job* renderJob = nullptr;

	int32 width = 800;
	int32 height = 600;
	
	int32 pad[1] = { 0 };
};

