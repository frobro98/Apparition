#include "Game/Game.h"
#include "GameObject/GameObjectManager.h"
#include "Shader/Material.h"
#include "Renderer/RenderingEngine.h"
#include "Texture2D/Texture.h"
#include "Texture2D/TextureManager.h"
#include "Model/Model.h"
#include "Model/ModelFactory.h"
#include "Model/MeshManager.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Input/Input.hpp"
#include "DebugOutput.h"
#include "Archiver/ArchiveQuery.h"
#include "Model/Mesh.h"
// #include "Animation/Skeleton/SkeletonManager.h"
// #include "Animation/Skeleton/Skeleton.h"
// #include "Animation/AnimationClipFactory.h"
#include "GameObject/AnimationGameObject.h"
#include "Archiver/SkeletonHeader.h"

#include "Scene/GameWorld.hpp"
#include "Scene/Scene.hpp"
#include "Lighting/PointLight.hpp"
#include "Lighting/SpotLight.hpp"

#include "DirectoryLocations.h"
#include "String/String.h"

#include "GameObject/DemoGameObjects/CameraObject.h"
#include "GameObject/DemoGameObjects/OrbitingObject.hpp"
#include "GameObject/DemoGameObjects/MoveBetweenObject.hpp"
#include "GameObject/DemoGameObjects/OrbitOtherObject.hpp"
#include "GameObject/DemoGameObjects/SpinningObject.hpp"
#include "GameObject/DemoGameObjects/ScaleChangingObject.hpp"

//////////////////////////////////////////////////////////////////////////
// ECS
//////////////////////////////////////////////////////////////////////////

#include "ECS/Entity.hpp"
#include "ECS/Components/TransformComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/RenderComponent.hpp"

#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/TransformSystem.hpp"
#include "ECS/Systems/RenderSystem.hpp"

#include "ECS/SystemsManager.hpp"


//////////////////////////////////////////////////////////////////////////
// TODO - This needs to be removed!
#include "Graphics/Vulkan/VulkanDescriptorSet.h"
#include "Graphics/Vulkan/VulkanShaderManager.h"
#include "Graphics/Vulkan/VulkanShader.h"
#include "Graphics/Vulkan/VulkanDescriptorLayoutManager.h"

using namespace Musa;



void InitializeMainCamera()
{
	CameraObject* cam = new CameraObject("Main Camera");
	GetGameObjectManager().Add(cam);
	cam->SetupGodCam();

	GetCameraManager().SetActiveCamera("Main Camera");
}

void Game::Init()
{
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	IntRect viewport = { 0, 0, width, height };

	Camera* camera = new Camera;
	camera->SetViewport(viewport);
	camera->SetPerspective(45.f, aspect, .1f, 10000.f);
	camera->SetOrientationAndPosition(Vector(0/*1000*/, 0/*1000*/, 0), Vector(0, 0.f, 155.f), Vector(0, 1, 0));

	// TODO - Make this more of a factory than a camera set up then add sort of thing
	GetCameraManager().AddCamera(camera, "Main Camera");
	GetCameraManager().SetActiveCamera("Main Camera");



// 
// 	TransformSystem* transSys = CreateSystem<TransformSystem>();
// 	CameraSystem* cameraSystem = CreateSystem<CameraSystem>();
// 	RenderSystem* renderingSystem = CreateSystem<RenderSystem>();
// 
// 	GetSystemsManager().AddSystem(*transSys);
// 	GetSystemsManager().AddSystemDependentOn(*cameraSystem, *transSys);
// 	GetSystemsManager().AddSystemDependentOn(*renderingSystem, *cameraSystem);
}

void Game::LoadContent()
{
// 	TransformComponent* transformComp = CreateComponent<TransformComponent>();
// 	CameraComponent* cameraComp = CreateComponent<CameraComponent>();
// 	RenderComponent* renderComp = CreateComponent<RenderComponent>();

	Entity* entity = CreateEntity("Test entity");
	AttachComponentsTo<TransformComponent, CameraComponent, RenderComponent>(*entity);
	CameraComponent* comp = Musa::GetComponentFrom<CameraComponent>(*entity);
	TransformComponent* trans = Musa::GetComponentFrom<TransformComponent>(*entity);
	
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	IntRect viewport = { 0, 0, width, height };

	comp->aspectRatio = aspect;
	comp->viewport = viewport;
	comp->fov = 45.f;
	comp->nearPlane = .1f;
	comp->farPlane = 10000.f;
	comp->lookAt = Vector();
	trans->SetPosition(Vector(0, 0, 155));


	LoadTextures();
	ProcessPackages();
	

	MeshManager::LoadPrimitive(Primitive::Box);
	MeshManager::LoadPrimitive(Primitive::Plane);
	MeshManager::LoadPrimitive(Primitive::Sphere);
	MeshManager::LoadPrimitive(Primitive::Pyramid);

	SingleThreadedStressTest();
}

void Game::UpdateGame(float /*time*/)
{
}

void Game::UnloadContent()
{
}

void Game::DeInit()
{
}

void Game::SingleThreadedStressTest()
{
	InitializeMainCamera();

	Scene& scene = world->GetScene();

	InitializeShaders();

	Mesh* box = MeshManager::FindMesh(Mesh::BoxName);
	Mesh* sphere = MeshManager::FindMesh(Mesh::SphereName);
	Mesh* plane = MeshManager::FindMesh(Mesh::PlaneName);
	Mesh* frigate = MeshManager::FindMesh("SpaceShip");

	Material* defaultMat = new Material(*normalMapVertShader, *normalMapFragShader, "brickwall", Color32::Black());
	defaultMat->SetNormalMap(GetTextureManager().FindTexture("brickwall_normal"));

	Material* sphereMat = new Material(*defaultVertShader, *defaultFragShader, nullptr, Color32::Blue());
	sphereMat->SetShadingModel(ShadingModel::Unlit);

	Material* colorMat = new Material(*defaultVertShader, *defaultFragShader, nullptr, Color32::Gray());
	colorMat->SetShadingModel(ShadingModel::Unlit);

	Material* litMat = new Material(*lightVertShader, *lightFragShader, nullptr, Color32::Black());
	litMat->SetShadingModel(ShadingModel::Lit);

	GameObject* boxObject = GetGameObjectManager().CreateAndAdd<MoveBetweenObject>(Vector(50, 0, 0), Vector(500, 0, 0));
	boxObject->SetModel(ModelFactory::CreateModel(box, defaultMat));
	boxObject->SetScale(60, 60, 60);

	GameObject* sphereObject = GetGameObjectManager().CreateAndAdd<OrbitingObject>(Vector::UpAxis, Vector(-100, 0, 0));
	sphereObject->SetModel(ModelFactory::CreateModel(sphere, sphereMat));
	sphereObject->SetPos(-50, 0, 0);
	sphereObject->SetScale(30, 30, 30);

	Vector objAxis = Vector(1, 1, 0).GetNormalized();
	sphereObject = GetGameObjectManager().CreateAndAdd<OrbitOtherObject>(*boxObject, objAxis);
	sphereObject->SetModel(ModelFactory::CreateModel(sphere, sphereMat));
	sphereObject->SetPos(15, 50, 0);
	sphereObject->SetScale(10, 10, 10);

	objAxis = Vector(-1, 1, 0).GetNormalized();
	sphereObject = GetGameObjectManager().CreateAndAdd<OrbitOtherObject>(*boxObject, objAxis);
	sphereObject->SetModel(ModelFactory::CreateModel(sphere, sphereMat));
	sphereObject->SetPos(75, 50, 0);
	sphereObject->SetScale(10, 10, 10);

	sphereObject = GetGameObjectManager().CreateAndAdd<SpinningObject>();
	sphereObject->SetModel(ModelFactory::CreateModel(sphere, sphereMat));
	sphereObject->SetPos(0, 100, 0);
	sphereObject->SetScale(15, 15, 15);

	boxObject = GetGameObjectManager().CreateAndAdd<ScaleChangingObject>(.1f, 5.f);
	boxObject->SetModel(ModelFactory::CreateModel(frigate, defaultMat));
	boxObject->SetPos(0, 175, -100);

	GameObject* floorObject = GetGameObjectManager().CreateAndAdd<GameObject>();
	floorObject->SetModel(ModelFactory::CreateModel(plane, litMat));
	floorObject->SetPos(0, -30, 0);
	floorObject->SetRotation(-90, 0, 0);
	floorObject->SetScale(500, 500, 1);

	Light* light = GetGameObjectManager().CreateAndAdd<SpotLight>();
	light->SetPos(Vector(100, 100, 100)); 
	light->SetRotation(-40.f, 45.f, 0.f);

	constexpr uint32 dimensions = 19;
	constexpr float startingPos = -500;
	constexpr float deltaX = 20;
	constexpr float deltaMove = 1.f / dimensions;

	float posZ = startingPos;
	for (uint32 i = 0; i < dimensions; ++i)
	{
		float posX = startingPos;
		float moveDelta = 0;
		for (uint32 j = 0; j < dimensions; ++j)
		{
			Vector a(posX, 0, posZ);
			Vector b(posX, 150, posZ);
			GameObject* movingObject = GetGameObjectManager().CreateAndAdd<MoveBetweenObject>(a, b, moveDelta);
			movingObject->SetModel(ModelFactory::CreateModel(box, sphereMat));
			movingObject->SetScale(5, 5, 5);

			posX += deltaX;
			moveDelta += deltaMove;
		}

		posZ += deltaX;
	}

	scene.AddLightToScene(*light);
}

void Game::ModelTestScene()
{
	// TODO - This will be leaked because nothing currently deletes this, I'm fairly certain
	VulkanDescriptorSetLayout* descriptorSetLayout = GetDescriptorLayoutManager().CreateSetLayout();
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 2);
	//descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 3);
	descriptorSetLayout->BindLayout();

	VulkanShader* vertShader = GetShaderManager().CreateShader("default.vs.spv", ShaderStage::Vertex);
	vertShader->SetDescriptorInformation(*descriptorSetLayout);
	VulkanShader* fragShader = GetShaderManager().CreateShader("default.fs.spv", ShaderStage::Vertex);
	fragShader->SetDescriptorInformation(*descriptorSetLayout);

	Mesh* plane = MeshManager::FindMesh(Mesh::PlaneName);
	//Mesh* spaceship = MeshManager::FindMesh("SpaceShip");
	Material* spaceshipMat =    new Material(*vertShader, *fragShader, "space_frigate_t0", Color32(1, 1, 1, 1));
	Material* spaceshipBC1Mat = new Material(*vertShader, *fragShader, "space_frigate_bc1", Color32(1, 1, 1, 1));
	Material* spaceshipBC3Mat = new Material(*vertShader, *fragShader, "space_frigate_bc3", Color32(1, 1, 1, 1));
	Material* spaceshipBC7Mat = new Material(*vertShader, *fragShader, "space_frigate_bc7", Color32(1, 1, 1, 1));

	GameObject* ship = GetGameObjectManager().CreateAndAdd<GameObject>();
	ship->SetModel(ModelFactory::CreateModel(plane, spaceshipMat));
	ship->SetPos(-105, 0, 0);
	ship->SetScale(50, 50, 50);

	ship = GetGameObjectManager().CreateAndAdd<GameObject>();
	ship->SetModel(ModelFactory::CreateModel(plane, spaceshipBC1Mat));
	ship->SetPos(-35, 0, 0);
	ship->SetScale(50, 50, 50);

	ship = GetGameObjectManager().CreateAndAdd<GameObject>();
	ship->SetModel(ModelFactory::CreateModel(plane, spaceshipBC3Mat));
	ship->SetPos(35, 0, 0);
	ship->SetScale(50, 50, 50);

	ship = GetGameObjectManager().CreateAndAdd<GameObject>();
	ship->SetModel(ModelFactory::CreateModel(plane, spaceshipBC7Mat));
	ship->SetPos(105, 0.f, 0);
	ship->SetScale(50, 50, 50);

}

/*
void Game::AnimationTestScene()
{
	uint8* chunkData = nullptr;
	uint32 chunkSize, hash;
	Skeleton* humanoidSkel = nullptr;
	Skeleton* teddySkel = nullptr;

	String assetPath = EngineAssetPath();

	// Loading the Humanoid
	assetPath += "Models/humanoid_anim_nocompress.pak";
	if (ProcessPackage(*assetPath, Chunk::SKEL_TYPE, "humanoid_Skel", chunkData, chunkSize, hash))
	{
		humanoidSkel = SkeletonManager::CreateSkeleton(chunkData, hash);
	}
	delete[] chunkData;

	if (ProcessPackage(*assetPath, Chunk::ANIM_TYPE, "shot_anim", chunkData, chunkSize, hash))
	{
		Assert(humanoidSkel != nullptr);
		AnimationClip* clip = AnimationClipFactory::CreateAnimationClip(chunkData);
		humanoidSkel->AddAnimation(clip);
	}
	delete[] chunkData;

	if (ProcessPackage(*assetPath, Chunk::ANIM_TYPE, "punch_anim", chunkData, chunkSize, hash))
	{
		Assert(humanoidSkel != nullptr);
		AnimationClip* clip = AnimationClipFactory::CreateAnimationClip(chunkData);
		humanoidSkel->AddAnimation(clip);
	}
	delete[] chunkData;

	if (ProcessPackage(*assetPath, Chunk::ANIM_TYPE, "run_anim", chunkData, chunkSize, hash))
	{
		Assert(humanoidSkel != nullptr);
		AnimationClip* clip = AnimationClipFactory::CreateAnimationClip(chunkData);
		humanoidSkel->AddAnimation(clip);
	}
	delete[] chunkData;

	assetPath = EngineAssetPath();
	assetPath += "Models/humanoid.pak";
	if (ProcessPackage(*assetPath, Chunk::VERTS_TYPE, "humanoid", chunkData, chunkSize, hash))
	{
		uint8* weightData;
		uint32 weightChunkSize, weightHash;
		if (ProcessPackage(*assetPath, Chunk::WEIGHTS_TYPE, "humanoid", weightData, weightChunkSize, weightHash))
		{
			MeshManager::LoadMeshFromPak(chunkData, weightData, "Humanoid");
			delete[] weightData;
		}
		else
		{
			MeshManager::LoadMeshFromPak(chunkData, "Humanoid");
		}
	}
	delete[] chunkData;

	//*/

	// Loading the teddy
	/*
	assetPath = EngineAssetPath();
	assetPath += "Models/teddy_TGA_anim.pak";
	if (ProcessPackage(*assetPath, Chunk::SKEL_TYPE, "teddy_TGA_Skel", chunkData, chunkSize, hash))
	{
		teddySkel = SkeletonManager::CreateSkeleton(chunkData, hash);
	}
	delete[] chunkData;

	if (ProcessPackage(*assetPath, Chunk::ANIM_TYPE, "idle_anim", chunkData, chunkSize, hash))
	{
		Assert(teddySkel != nullptr);
		AnimationClip* clip = AnimationClipFactory::CreateAnimationClip(chunkData);
		teddySkel->AddAnimation(clip);
	}
	delete[] chunkData;

	if (ProcessPackage(*assetPath, Chunk::ANIM_TYPE, "walk_anim", chunkData, chunkSize, hash))
	{
		Assert(teddySkel != nullptr);
		AnimationClip* clip = AnimationClipFactory::CreateAnimationClip(chunkData);
		teddySkel->AddAnimation(clip);
	}
	delete[] chunkData;

	assetPath = EngineAssetPath();
	assetPath += "Models/teddy_TGA.pak";
	if (ProcessPackage(*assetPath, Chunk::VERTS_TYPE, "teddy_TGA", chunkData, chunkSize, hash))
	{
		uint8* weightData;
		uint32 weightChunkSize, weightHash;
		if (ProcessPackage(*assetPath, Chunk::WEIGHTS_TYPE, "teddy_TGA", weightData, weightChunkSize, weightHash))
		{
			MeshManager::LoadMeshFromPak(chunkData, weightData, "Teddy");
			delete[] weightData;
		}
		else
		{
			MeshManager::LoadMeshFromPak(chunkData, "Teddy");
		}
	}
	delete[] chunkData;


	AnimationGameObject* go = new AnimationGameObject;
	go->SetModel(ModelFactory::CreateModel(MeshManager::FindMesh("Humanoid"), new Material("gpuColorSkinning.vs.spv", "gpuColorSkinning.fs.spv", nullptr, Color32{ 1, 1, 1, 1 })));
	GetGameObjectManager().Add(go);

	Quat rot(ROT_XYZ, -Math::PiOver2, 0, 0);
	go->SetWorld(rot);
	go->SetSkeleton(*humanoidSkel);
	go->SetupInputs();
	go->SetDebug(false);

	go = new AnimationGameObject;
	go->SetModel(ModelFactory::CreateModel(MeshManager::FindMesh("Teddy"), new Material("gpuColorSkinning.vs.spv", "gpuColorSkinning.fs.spv", nullptr, Color32{ 1, 1, 1, 1 })));
	GetGameObjectManager().Add(go);

	go->SetSkeleton(*teddySkel);
	go->SetupInputs();
	go->SetDebug(false);
	go->SetCurrentClip(0);
}
//*/

void Game::LoadTextures()
{
	{
		Path texturePath(EngineTexturePath() + "default.tex");
		GetTextureManager().LoadTextureFromFile(texturePath, TextureManager::DefaultTexture);
	}

	{
		Path texturePath(EngineTexturePath() + "brickwall.tex");
		GetTextureManager().LoadTextureFromFile(texturePath, "brickwall");
	}

	{
		Path texturePath(EngineTexturePath() + "brickwall_norm.tex");
		GetTextureManager().LoadTextureFromFile(texturePath, "brickwall_norm");
	}
}

void Game::InitializeShaders()
{
	// TODO - This will be leaked because nothing currently deletes this, I'm fairly certain
	VulkanDescriptorSetLayout* descriptorSetLayout = GetDescriptorLayoutManager().CreateSetLayout();
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 2);
	//descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 3);
	descriptorSetLayout->BindLayout();

	defaultVertShader = GetShaderManager().CreateShader("textureGBuffer.vs.spv", ShaderStage::Vertex);
	defaultVertShader->SetDescriptorInformation(*descriptorSetLayout);
	defaultFragShader = GetShaderManager().CreateShader("textureGBuffer.fs.spv", ShaderStage::Fragment);
	defaultFragShader->SetDescriptorInformation(*descriptorSetLayout);

	descriptorSetLayout = GetDescriptorLayoutManager().CreateSetLayout();
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 2);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 3);
	descriptorSetLayout->BindLayout();

	normalMapVertShader = GetShaderManager().CreateShader("gbufferNormalMap.vs.spv", ShaderStage::Vertex);
	normalMapVertShader->SetDescriptorInformation(*descriptorSetLayout);
	normalMapFragShader = GetShaderManager().CreateShader("gbufferNormalMap.fs.spv", ShaderStage::Fragment);
	normalMapFragShader->SetDescriptorInformation(*descriptorSetLayout);

	descriptorSetLayout = GetDescriptorLayoutManager().CreateSetLayout();
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 0);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1);
	descriptorSetLayout->AddDescriptorBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 2);
	descriptorSetLayout->BindLayout();

	lightVertShader = GetShaderManager().CreateShader("gbuffer.vs.spv", ShaderStage::Vertex);
	lightVertShader->SetDescriptorInformation(*descriptorSetLayout);
	lightFragShader = GetShaderManager().CreateShader("gbuffer.fs.spv", ShaderStage::Fragment);
	lightFragShader->SetDescriptorInformation(*descriptorSetLayout);
}

void Game::ProcessPackages()
{
	String assetPath = EngineAssetPath();
	assetPath += "Models/astroboy.pak";

	uint8* chunkData = nullptr;
	uint32 chunkSize, hash;
	if (ProcessPackage(*assetPath, Chunk::VERTS_TYPE, "astroboy", chunkData, chunkSize, hash))
	{
		MeshManager::LoadMeshFromPak(chunkData, "AstroBoy");
	}
	delete[] chunkData;

	if (ProcessPackage(*assetPath, Chunk::TEXTURE_TYPE, "astroboy_t0", chunkData, chunkSize, hash))
	{
		DynamicArray<uint8> textureData(chunkData, chunkSize);
		GetTextureManager().LoadTexture(textureData, "astroboy_t0");
	}
	delete[] chunkData;


	assetPath = EngineAssetPath();
	assetPath += "Models/space_frigate.pak";
	if (ProcessPackage(*assetPath, Chunk::VERTS_TYPE, "space_frigate", chunkData, chunkSize, hash))
	{
		MeshManager::LoadMeshFromPak(chunkData, "SpaceShip");
	}
	delete[] chunkData;

}

