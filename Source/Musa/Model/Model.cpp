// Copyright 2020, Nathan Blane

#include "Model.h"
#include "Shader/Material.hpp"
#include "Mesh/Mesh.h"
#include "Mesh/MeshRenderInfo.hpp"
// #include "Animation/Skeleton/SkeletonBone.h"
//#include "Animation/Skeleton/Skeleton.h"
#include "Archiver/SkeletonHeader.h"
#include "Graphics/GraphicsInterface.hpp"
#include "Graphics/UniformBuffers.h"

Model::Model(Mesh* modelMesh, Material* mat)
	: mesh(modelMesh),
	material(mat)
{
	Memset(boneMats, 0, sizeof(Matrix4) * MaxBones);
	SetupMeshRenderInfo();
}

Model::~Model()
{
	material = nullptr;
	mesh = nullptr;
}

void Model::SetupMeshRenderInfo()
{
	renderInfo = MakeUnique<MeshRenderInfo>();
	renderInfo->vertexBuffer = &mesh->GetVertexBuffer();
	renderInfo->indexBuffer = &mesh->GetIndexBuffer();
	renderInfo->transformBuffer = GetGraphicsInterface().CreateUniformBuffer(sizeof(PrimUniformBuffer));
	renderInfo->meshMaterial = &material->GetRenderDescription();
}

void Model::SetWorld(const Matrix4& worldMat)
{
	world = worldMat;
}

void Model::SetBonePose(const DynamicArray<BonePoseData>& poses)
{
	poseData = poses;
}

void Model::SetMaterial(Material* mat)
{
	material = mat;
}

void Model::SetScene(Scene& scene_)
{
	scene = &scene_;
}

void Model::SetActive(bool isActive)
{
	// TODO - Find a way to implement the active vs not active
	if (isActive != active)
	{
// 		if (isActive)
// 		{
// 			owningDrawList->AddToRenderList(this);
// 		}
// 		else
// 		{
// 			owningDrawList->RemoveFromRenderList(this);
// 		}
// 
// 		active = isActive;
	}
}

void Model::SetActiveSkeleton(SkeletonInstance& skel)
{
	skeleton = &skel;
}

const Matrix4* Model::GetBoneMats() const
{
	return boneMats;
}

DynamicArray<BonePoseData> Model::GetPoseData() const
{
	return poseData;
}
