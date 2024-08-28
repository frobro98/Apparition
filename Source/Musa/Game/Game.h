#pragma once

#include "Engine.h"

class VulkanShader;

class Game final : public Engine
{
public:
	Game() = default;
	~Game() = default;

	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	virtual void Init() override;
	virtual void LoadContent() override;
	virtual void UpdateGame(float time) override;
	virtual void UnloadContent() override;
	virtual void DeInit() override;

private:

	// Scene setup
	void SingleThreadedStressTest();
	void ModelTestScene();
	void AnimationTestScene();

	void ProcessPackages();
	void LoadTextures();
	void InitializeShaders();

private:
	VulkanShader* defaultVertShader = nullptr;
	VulkanShader* defaultFragShader = nullptr;

	VulkanShader* normalMapVertShader = nullptr;
	VulkanShader* normalMapFragShader = nullptr;

	VulkanShader* lightVertShader = nullptr;
	VulkanShader* lightFragShader = nullptr;
};