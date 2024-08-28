#include "CameraObject.h"
#include "Camera/Camera.h"
#include "Camera/CameraManager.h"
#include "Input/Input.hpp"
#include "Model/ModelFactory.h"
#include "Model/MeshManager.h"
#include "Model/Mesh.h"
#include "Model/Model.h"
#include "Shader/Material.h"

// TODO - This needs to be removed!
#include "Graphics/Vulkan/VulkanDescriptorSet.h"
#include "Graphics/Vulkan/VulkanShaderManager.h"
#include "Graphics/Vulkan/VulkanShader.h"
#include "Graphics/Vulkan/VulkanDescriptorLayoutManager.h"

CameraObject::CameraObject(const char* camName)
	:cameraName(camName)
{
	camera = GetCameraManager().FindCamera(cameraName);
	Assert(camera != nullptr);
	position = camera->GetPosition();
	lookat = camera->GetLookAt();
	//camera->SetClearColor(Color{.7f, .7f, .8f, 1.f});
}

void CameraObject::SetCamera()
{
	GetCameraManager().SetActiveCamera(cameraName);
}

void CameraObject::Update(float /*tick*/)
{
	camera->SetOrientationAndPosition(lookat, position, camera->GetUp());

	world =  Matrix(TRANS, position);

	if (model != nullptr)
	{
		model->SetWorld(world);
	}
	if (debugVolume != nullptr)
	{
		TransformDebugVolume();
	}
}

void CameraObject::ShowDebugVolume(bool show)
{
	if (debugShown != show)
	{
		if (show && debugVolume == nullptr)
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

			Material* mat = new Material(*vertShader, *fragShader, "defaultTex", Color32::White());
			mat->EnableWireframe();
			debugVolume = ModelFactory::CreateModel(MeshManager::LoadFrustumForCamera(camera), mat);
		}
		else if (show && debugVolume != nullptr)
		{
			debugVolume->SetActive(true);
		}
		else if (!show && debugVolume != nullptr)
		{
			debugVolume->SetActive(false);
		}

		debugShown = show;
	}
}

void CameraObject::SetupGodCam()
{
	GetInputManager().RegisterContinuousInput(KeyInput::Key_W, 1, this, &CameraObject::MoveForward);
	GetInputManager().RegisterContinuousInput(KeyInput::Key_S, -1, this, &CameraObject::MoveForward);

	GetInputManager().RegisterContinuousInput(KeyInput::Key_D, 1, this, &CameraObject::MoveRight);
	GetInputManager().RegisterContinuousInput(KeyInput::Key_A, -1, this, &CameraObject::MoveRight);

	GetInputManager().RegisterContinuousInput(KeyInput::Key_E, 1, this, &CameraObject::MoveUp);
	GetInputManager().RegisterContinuousInput(KeyInput::Key_Q, -1, this, &CameraObject::MoveUp);

	GetInputManager().RegisterMouseAxes(this, &CameraObject::LookAtAdjust);
}

void CameraObject::MoveForward(int32 mod)
{
	position += camera->GetForward() * (float)-mod * 1.5f;
	lookat += camera->GetForward() * (float)-mod * 1.5f;
}

void CameraObject::MoveRight(int32 mod)
{
	position += camera->GetRight() * (float)mod * 1.5f;
	lookat += camera->GetRight() * (float)mod * 1.5f;
}

void CameraObject::MoveUp(int32 mod)
{
	position += camera->GetUp() * (float)mod * 1.5f;
	lookat += camera->GetUp() * (float)mod * 1.5f;
}

void CameraObject::LookAtAdjust(float changeX, float changeY)
{
	const float speed = .1f;
 	Quat quatX(ROT_AXIS_ANGLE, camera->GetRight(), -changeY * speed);
 	Quat quatY(ROT_Y, -changeX * speed);

 	Vector newLookAtDir = camera->GetForward() * quatX * quatY;
 	newLookAtDir.Normalize();
 	lookat = position - newLookAtDir;
	Vector up = camera->GetUp() * quatX * quatY;
	camera->SetOrientationAndPosition(lookat, position, up);
 	
}

void CameraObject::TransformDebugVolume()
{
	debugVolume->SetWorld(world);
}
