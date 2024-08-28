// Copyright 2020, Nathan Blane

#include "MusaEngine.hpp"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Mesh/MeshManager.h"
#include "Input/Input.hpp"
#include "Texture/Texture2D/TextureManager.h"

#include "Math/MathFunctions.hpp"
#include "Scene/Scene.hpp"

#include "Lighting/SpotLight.hpp"
#include "Shader/Material.hpp"
#include "Model/ModelFactory.h"

#include "Engine/FrameData.hpp"
#include "Engine/Internal/FrameDataInternal.hpp"

#include "Debugging/MetricInterface.hpp"
#include "Debugging/ProfilerStatistics.hpp"

#include "RenderPipeline/UserInterfacePipeline.hpp"
#include "RenderPipeline/DeferredRenderPipeline.hpp"
#include "Font/FontCache.hpp"
#include "Application/Musa.hpp"
#include "Shader/Shader.hpp"
#include "Input/InputContext.hpp"

#include "Mesh/Mesh.h"

// ECS ---------------------
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/MeshRenderComponent.hpp"
#include "ECS/Components/TranslationComponent.hpp"
#include "ECS/Components/RotationComponent.hpp"
#include "ECS/Components/InputContextComponent.hpp"
#include "ECS/Components/CameraComponent.hpp"
#include "ECS/Components/ButtonEventComponent.hpp"
#include "ECS/Components/MouseMoveEventComponent.hpp"
#include "ECS/Components/AnalogChangeEventComponent.hpp"

#include "ECS/Systems/OSInputSystem.hpp"
#include "ECS/Systems/GameInputSystem.hpp"
#include "ECS/Systems/WorldTransformSystem.hpp"
#include "ECS/Systems/CameraSystem.hpp"
#include "ECS/Systems/MaterialBatchingSystem.hpp"
#include "ECS/Systems/ViewCullingSystem.hpp"

// BEGIN GAME SYSTEMS
#include "ECS/Systems/GodCamSystem.hpp"
// END GAME SYSTEMS
// -------------------------

// NOTE - This is for deserializing PAK files.
// TODO - The way pak files are deserialized, or even structured, needs to either change or be reexamined
#include "Archiver/FileDeserializer.hpp"
#include "Archiver/PackageHeader.h"
#include "Archiver/ChunkHeader.h"
#include "Archiver/AnimationHeader.h"
#include "Model/ModelFileHeader.h"
#include "Animation/AnimationClipFactory.h"
#include "Animation/Clip.h"
#include "Animation/Skeleton/SkeletonManager.h"
#include "Animation/Skeleton/Skeleton.h"
#include "File/DirectoryLocations.hpp"

#include "Memory/MemoryCore.hpp"

//GLOBAL_OPERATOR_NEW_DELETE_OVERLOADS

DECLARE_METRIC_GROUP(Engine);
DECLARE_METRIC_GROUP(FrameRender);

METRIC_STAT(UpdateAndRender, Engine);
METRIC_STAT(Update, Engine);
METRIC_STAT(Render, Engine);


static Musa::RenderPipeline* pipeline = nullptr;

Musa::RenderPipeline& Musa::GetRenderPipeline()
{
	Assert(pipeline);
	return *pipeline;
}

using namespace Musa;

MusaEngine::MusaEngine(UI::Context& context, const ApplicationEventRouter& inputDispatcher_, ApplicationInputMap* inInputMap)
	: inputDispatcher(inputDispatcher_),
	uiContext(&context),
	inputMap(inInputMap)
{
}

void MusaEngine::StartupEngine(Window& window)
{
	SetupWindowContext(window);
	InitializeSceneView();
}

void MusaEngine::ShutdownEngine()
{
	viewport.Reset();
}

void MusaEngine::SetupWindowContext(Window& window)
{
	screenView = MakeUnique<ScreenView>(window.GetWidth(), window.GetHeight());
	viewport = MakeUnique<Viewport>(window.GetWindowHandle(), window.GetWidth(), window.GetHeight());
	renderPipeline = MakeUnique<RenderPipeline>(*viewport);
	renderPipeline->Initialize();

	pipeline = renderPipeline.Get();
}

void MusaEngine::InitializeSceneView()
{
	// TODO - This should be available here, so the width and height should be removed
	const i32 width = viewport->GetWidth();
	const i32 height = viewport->GetHeight();

	const f32 aspect = (f32)width / (f32)height;
	const Recti viewportDim = { 0, 0, width, height };

	Camera* mainCamera = new Camera;
	mainCamera->SetViewport(viewportDim);
	mainCamera->SetPerspective(60.f, aspect, .1f, 10000.f);
	mainCamera->SetOrientationAndPosition(Vector4(0, 0, 0), Vector4(0, 0, 10.f), Vector4(0, 1, 0));

	GetCameraManager().AddCamera(mainCamera, "Main Camera");
	GetCameraManager().SetActiveCamera("Main Camera");

	gApp->SetRawMouseInput(true);
}

/*
static InputContext* CreateDefaultInputContext()
{
	InputContext& mainContext = MakeInputContext("Keyboard Context");
	//InputContext gamepadContext = MakeInputContext("Gamepad Context");

	RangedInput mouseXInput;
	mouseXInput.range = {
		-10, 10, -1, 1
	};
	mouseXInput.input = {
		"Mouse X",
		Input::Mouse_XAxis
	};
	mainContext.inputRanges.Add(mouseXInput);

	RangedInput mouseYInput;
	mouseYInput.range = {
		-10, 10, -1, 1
	};
	mouseYInput.input = {
		"Mouse Y",
		Input::Mouse_YAxis
	};
	mainContext.inputRanges.Add(mouseYInput);

	SingleInput input = {
		"Move Left",
		Input::Key_A
	};
	mainContext.inputStates.Add(input);

	input = {
		"Move Right",
		Input::Key_D
	};
	mainContext.inputStates.Add(input);

	input = {
		"Move Forward",
		Input::Key_W
	};
	mainContext.inputStates.Add(input);

	input = {
		"Move Backward",
		Input::Key_S
	};
	mainContext.inputStates.Add(input);

	input = {
	"Move Up",
	Input::Key_E
	};
	mainContext.inputStates.Add(input);

	input = {
		"Move Down",
		Input::Key_Q
	};
	mainContext.inputStates.Add(input);

	input = {
		"Space",
		Input::Key_Space
	};
	mainContext.inputActions.Add(input);

	return &mainContext;


	// Controller support
	
	RangedInput leftXInput;
	leftXInput.range = {
		-100, 100, -1, 1
	};
	leftXInput.input = {
		"Leftstick X",
		Input::Gamepad_LeftStick_XAxis
	};
	gamepadContext.inputRanges.Add(leftXInput);

	RangedInput leftYInput;
	leftYInput.range = {
		-100, 100, -1, 1
	};
	leftYInput.input = {
		"Leftstick Y",
		Input::Gamepad_LeftStick_YAxis
	};
	gamepadContext.inputRanges.Add(leftYInput);

	RangedInput rightXInput;
	rightXInput.range = {
		-100, 100, -1, 1
	};
	rightXInput.input = {
		"Rightstick X",
		Input::Gamepad_RightStick_XAxis
	};
	gamepadContext.inputRanges.Add(rightXInput);

	RangedInput rightYInput;
	rightYInput.range = {
		-100, 100, -1, 1
	};
	rightYInput.input = {
		"Rightstick Y",
		Input::Gamepad_RightStick_YAxis
	};
	gamepadContext.inputRanges.Add(rightYInput);

	RangedInput leftTriggerInput = {};
	leftTriggerInput.range = {
		0, 1, 0, 1
	};
	leftTriggerInput.input = {
		"Left Trigger",
		Input::Gamepad_LeftTrigger
	};
	gamepadContext.inputRanges.Add(leftTriggerInput);

	RangedInput rightTriggerInput = {};
	rightTriggerInput.range = {
		0, 1, 0, 1
	};
	rightTriggerInput.input = {
		"Right Trigger",
		Input::Gamepad_RightTrigger
	};
	gamepadContext.inputRanges.Add(rightTriggerInput);

	input = {
	"Gamepad A",
	Input::Gamepad_AButton
	};
	gamepadContext.inputActions.Add(input);
	

// 	gameInput.AddInputContext(mainContext);
// 	gameInput.AddInputContext(gamepadContext);
// 	gameInput.PushInputContext("Keyboard Context");
// 	gameInput.PushInputContext("Gamepad Context");
}
//*/

// TODO - This SHOULD NOT exist in the engine file. It should exist in the file reading system probably...
static void LoadPakFile(const Path& pakPath)
{
	FileDeserializer pakFile(pakPath);
	PackageHeader pakHeader;
	Deserialize(pakFile, pakHeader);
	
	for (u32 i = 0; i < pakHeader.numChunks; ++i)
	{
		::ChunkHeader chunkHeader;
		Deserialize(pakFile, chunkHeader);

		switch (chunkHeader.type)
		{
			case Chunk::VERTS_TYPE:
			{
				// TODO - This is sort of gross...there has to be a way to reduce the memory allocation. Allocating from the deserialization
				// and then one from the mesh manager
				// TODO - Deserialize the header here
				u8* modelData = new u8[chunkHeader.chunkSize];
				pakFile.DeserializeData(modelData, chunkHeader.chunkSize);
				
				GetMeshManager().LoadMeshFromPak(modelData, chunkHeader.chunkName);
			}break;
			case Chunk::TEXTURE_TYPE:
			{
				// TODO - Need to allow the Serialization/Deserialization functions to work with this
				MemoryBuffer texData(chunkHeader.chunkSize);
				pakFile.DeserializeData(texData.GetData(), chunkHeader.chunkSize);

				GetTextureManager().LoadTexture(texData, chunkHeader.chunkName);
			}break;
			case Chunk::SKEL_TYPE:
			{
				MemoryBuffer skelData(chunkHeader.chunkSize);
				pakFile.DeserializeData(skelData.GetData(), skelData.Size());

				NOT_USED Skeleton* skeleton = SkeletonManager::CreateSkeleton(skelData.GetData(), chunkHeader.hashNum);
			}
			case Chunk::WEIGHTS_TYPE:
			{
				// TODO - Weights need to be stored with the mesh, at this point, but this sort of thing needs to be revisited...
				MemoryBuffer weights(chunkHeader.chunkSize);
				pakFile.DeserializeData(weights.GetData(), weights.Size());

			}
			case Chunk::ANIM_TYPE:
			{
				MemoryBuffer animData(chunkHeader.chunkSize);
				pakFile.DeserializeData(animData.GetData(), chunkHeader.chunkSize);
				
				AnimationClip* clip = AnimationClipFactory::CreateAnimationClip(animData.GetData());
				Skeleton* skeleton = SkeletonManager::FindSkeleton(clip->GetSkeletonHash());
				skeleton->AddAnimation(clip);
			}break;
			default:
				break;
		}

	}
}

// TODO - This will eventually be moved. Mostly for quick turn around
#include "UI/DebugUI/UIConsole.hpp"
#include "GameObject/DemoGameObjects/OrbitOtherObject.hpp"
#include "GameObject/DemoGameObjects/MoveBetweenObject.hpp"
#include "GameObject/DemoGameObjects/ScaleChangingObject.hpp"

void MusaEngine::LoadContent()
{
	GetMeshManager().Initialize();

	Path bunnyPakPath(EngineAssetPath());
	bunnyPakPath /= "Models/stanford-bunny.pak";
	LoadPakFile(bunnyPakPath);

	Path astroPakPath(EngineAssetPath());
	astroPakPath /= "Models/astro-boy.pak";
	LoadPakFile(astroPakPath);

	Path gethPakPath(EngineAssetPath());
	gethPakPath /= "Models/geth-trooper.pak";
	LoadPakFile(gethPakPath);

	ImportTTFont(Path("C:\\Windows\\Fonts\\Arial.ttf"));

	NOT_USED Mesh* box = GetMeshManager().FindMesh(Mesh::BoxName);
	NOT_USED Mesh* bunny = GetMeshManager().FindMesh("stanford-bunny");
	NOT_USED Mesh* astro_boy = GetMeshManager().FindMesh("astro-boy");
	NOT_USED Mesh* geth = GetMeshManager().FindMesh("geth-trooper");

	ShaderID unlitVertID = Shader::FindOrLoadShaderFile("StandardTransform.mvs");
	ShaderID unlitFragID = Shader::FindOrLoadShaderFile("StandardUnlit.mfs");

	Texture* gethTex = GetTextureManager().FindTexture("ME3_360_ENEMY_Geth_Trooper_Body_D_t0");
	TextureResource& gethTexResource = gethTex->GetResource();

	Texture* astroTex = GetTextureManager().FindTexture("astroboy_t0");
	TextureResource& astroTexResource = astroTex->GetResource();

	// TODO - This is currently required because of how the UpdateTextureBlob function works.
	// Fix this issue
	NOT_USED Texture* whiteTex = WhiteTexture();
	//TextureResource& whiteResource = whiteTex->GetResource();

	Material* gethMat = new Material(unlitVertID, unlitFragID);
	TextureSamplerDescriptor texture = gethMat->GetTextureSamplerConstant("mainTexture");
	gethMat->SetTextureSamplerResource(texture, gethTexResource);

	Material* astroMat = new Material(unlitVertID, unlitFragID);
	texture = astroMat->GetTextureSamplerConstant("mainTexture");
	astroMat->SetTextureSamplerResource(texture, astroTexResource);
	
// 	GameObject* gethObject = world->CreateGameObject<GameObject>();
// 	gethObject->SetModel(ModelFactory::CreateModel(geth, gethMat));

	// TODO - Be able to reuse a material (have some sort of material ID system potentially?)

	//const i32 viewWidth = viewport->GetWidth();
	//const i32 viewHeight = viewport->GetHeight();
	//const f32 aspectRatio = (f32)viewWidth / (f32)viewHeight;

	//Musa::Entity camera = ecsWorld.CreateEntity<CameraComponent, TranslationComponent, InputContextComponent>();
	//ecsWorld.SetComponentDataOn(camera, CameraComponent{ {},
	//	Rectf {0, 0, 1, 1},
	//	UnnormalizeColor(Color32(.7f, .7f, .8f)),
	//	.1f, 10000.f,
	//	60.f, aspectRatio,
	//	ProjectionType::Perspective
	//});
	//ecsWorld.SetComponentDataOn(camera, InputContextComponent{ {},
	//	CreateDefaultInputContext()
	//});
	//ecsWorld.SetComponentDataOn(camera, TranslationComponent{ {},
	//	Vector3(0.f, 0.f, 10.f)
	//});

	//Musa::Entity gethEntity = ecsWorld.CreateEntity<TranslationComponent, MeshRenderComponent>();
	//ecsWorld.SetComponentDataOn(gethEntity, MeshRenderComponent{ {},
	//		geth,
	//		gethMat
	//	}
	//);

	//Musa::Entity astroEntity = ecsWorld.CreateEntity<MeshRenderComponent, TranslationComponent, RotationComponent>();
	//ecsWorld.SetComponentDataOn(astroEntity, MeshRenderComponent{ {},
	//	astro_boy,
	//	astroMat
	//});
	//ecsWorld.SetComponentDataOn(astroEntity, TranslationComponent{ {},
	//	Vector3(5.f, 5.f, 2.f)
	//});
	//ecsWorld.SetComponentDataOn(astroEntity, RotationComponent{ {},
	//	Quat(ROT_Y, Math::DegreesToRadians(45.f))
	//});

	//// Input
	//ecsWorld.CreateSystem<OSInputSystem>(*inputMap);
	//ecsWorld.CreateSystem<GameInputSystem>();

	//// BEGIN GAME SYSTEMS
	//ecsWorld.CreateSystem<GodCamSystem>();
	//// END GAME SYSTEMS

	//// End of Update Pushing of data
	//ecsWorld.CreateSystem<CameraSystem>();
	//ecsWorld.CreateSystem<WorldTransformSystem>();

	//// Prerender data management
	//ecsWorld.CreateSystem<MaterialBatchingSystem>();
	//ecsWorld.CreateSystem<ViewCullingSystem>();

// 
// 	GameObject* go = world->CreateGameObject<OrbitOtherObject>(*gethObject, 5.5f, Vector4::RightAxis);
// 	go->SetModel(ModelFactory::CreateModel(astro_boy, astroMat));
// 	go->SetPos(Vector4(-1.5f, 2.5f, 0));
// 	
// 
// 	Material* orbitMat = new Material(unlitVertID, unlitFragID);
// 	texture = orbitMat->GetTextureSamplerConstant("mainTexture");
// 	orbitMat->SetTextureSamplerResource(texture, whiteResource);
// 	orbitMat->SetColor(Color32::Blue());
// 
// 	go = world->CreateGameObject<OrbitOtherObject>(*go, 2.f, Vector4::UpAxis);
// 	go->SetModel(ModelFactory::CreateModel(box, orbitMat));
// 	go->SetPos(Vector4(5, 0, 0));

	/*
	constexpr u32 numDoubleObjects = 400;

	Vector4 pointA(-200.f, 2.f, -1.f);
	Vector4 pointB(-200.f, 4.f, -1.f);

	Vector4 scalePos(-400, 5, 7.f);
	float dir = 1;
	Material* blueMat = new Material(unlitVertID, unlitFragID);
	blueMat->SetColor(Color32::Blue());
	texture = blueMat->GetTextureSamplerConstant("mainTexture");
	blueMat->SetTextureSamplerResource(texture, whiteResource);



	Color32 color = Color32::Blue();
	for (u32 i = 0; i < numDoubleObjects; ++i)
	{
		Vector4 tmp = pointA;
		pointA = pointB;
		pointB = tmp;

		Material* cubeMat = new Material(unlitVertID, unlitFragID);
		cubeMat->SetColor(color);
		texture = cubeMat->GetTextureSamplerConstant("mainTexture");
		cubeMat->SetTextureSamplerResource(texture, whiteResource);

		go = world->CreateGameObject<MoveBetweenObject>(pointA, pointB, 1.f);
		go->SetModel(ModelFactory::CreateModel(box, cubeMat));

		pointA.x += 1.5f;
		pointB.x += 1.5f;

		constexpr f32 scaleDelta = 2.f;
		if (dir < 0)
		{
			go = world->CreateGameObject<ScaleChangingObject>(1.f, 3.f, scaleDelta);
			color = Color32::Blue();
		}
		else
		{
			go = world->CreateGameObject<ScaleChangingObject>(3.f, 1.f, scaleDelta);
			color = Color32::Red();
		}

		cubeMat = new Material(unlitVertID, unlitFragID);
		cubeMat->SetColor(color);
		texture = cubeMat->GetTextureSamplerConstant("mainTexture");
		cubeMat->SetTextureSamplerResource(texture, whiteResource);

		go->SetModel(ModelFactory::CreateModel(box, cubeMat));
		go->SetPos(scalePos);
		
		scalePos.y *= dir;
		scalePos.x += 2.5f;
		dir *= -1.f;
	}

	pointA.y -= 5.f;
	pointB.y -= 5.f;
	scalePos.y = -5;
	for (u32 i = 0; i < numDoubleObjects; ++i)
	{
		Vector4 tmp = pointA;
		pointA = pointB;
		pointB = tmp;

		Material* cubeMat = new Material(unlitVertID, unlitFragID);
		cubeMat->SetColor(color);
		texture = cubeMat->GetTextureSamplerConstant("mainTexture");
		cubeMat->SetTextureSamplerResource(texture, whiteResource);

		go = world->CreateGameObject<MoveBetweenObject>(pointA, pointB, 1.f);
		go->SetModel(ModelFactory::CreateModel(box, cubeMat));

		pointA.x -= 1.5f;
		pointB.x -= 1.5f;

		constexpr f32 scaleDelta = 2.f;
		if (dir < 0)
		{
			go = world->CreateGameObject<ScaleChangingObject>(1.f, 3.f, scaleDelta);
			color = Color32::Blue();
		}
		else
		{
			go = world->CreateGameObject<ScaleChangingObject>(3.f, 1.f, scaleDelta);
			color = Color32::Red();
		}

		cubeMat = new Material(unlitVertID, unlitFragID);
		cubeMat->SetColor(color);
		texture = cubeMat->GetTextureSamplerConstant("mainTexture");
		cubeMat->SetTextureSamplerResource(texture, whiteResource);

		go->SetModel(ModelFactory::CreateModel(box, cubeMat));
		go->SetPos(scalePos);

		scalePos.y *= dir;
		scalePos.x -= 2.5f;
		dir *= -1.f;
	}
	//*/


	// TODO - LEAKING MEMEORY!!!
	//UI::Console* console = new UI::Console;
	//uiContext->AddWidget(*console);
}

void MusaEngine::UnloadContent()
{
	GetMeshManager().Deinitialize();
	GetTextureManager().Deinitialize();
}

void MusaEngine::UpdateAndRender(f32 /*tick*/)
{
	SCOPED_TIMED_BLOCK(UpdateAndRender);

	BEGIN_TIMED_BLOCK(Update);
	//ecsWorld.Update();
	uiContext->Update();
	END_TIMED_BLOCK(Update);

	BEGIN_TIMED_BLOCK(Render);
	RenderFrame();
	END_TIMED_BLOCK(Render);
}

void MusaEngine::GatherFrameMetrics()
{
	{
		//SCOPED_TIMED_BLOCK(GatherMetrics);
		GetProfilingStatistics().CollectAllFrameMetrics();
	}
	GetProfilingStatistics().ProfileFrameIncrement();
}

void MusaEngine::Resize(u32 newWidth, u32 newHeight)
{
	viewport->Resize(newWidth, newHeight);
}

void MusaEngine::RenderFrame()
{
	Camera* mainCamera = GetCameraManager().GetActiveCamera();
	screenView->AssociateCameraWithView(*mainCamera);
	renderPipeline->Execute(*screenView, *uiContext);
}

