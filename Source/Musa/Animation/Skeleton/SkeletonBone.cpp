
#include "SkeletonBone.h"
#include "Animation/KeyFrame.h"
#include "Model/Model.h"
#include "Mesh/Mesh.h"
#include "Mesh/MeshManager.h"
#include "Shader/Material.hpp"
#include "Math/Matrix4.hpp"
#include "Model/ModelFactory.h"

SkeletonBone::SkeletonBone()
	: world(new Matrix4(IDENTITY))
{
	Mesh* pyramid = GetMeshManager().FindMesh(Mesh::PyramidName);
	Material* mat = new Material;
	debugModel = ModelFactory::CreateModel(pyramid, mat);
}

SkeletonBone::~SkeletonBone()
{
}

void SkeletonBone::SetIndex(u32 index)
{
	boneIndex = index;
}

void SkeletonBone::SetDebug(bool debug)
{
	debugModel->SetActive(debug);
}

void SkeletonBone::SetOrientation(const Matrix4& orientation)
{
	debugModel->SetWorld(orientation);
}

Matrix4 SkeletonBone::GetWorld() const
{
	return *world;
}

void SkeletonBone::UpdateAnimationTransform(FrameData* /*boneData*/)
{
	//Assert(parent != nullptr);

// 	Matrix4 parentWorld = *parent->world;
// 
// 	FrameData* correctData = &boneData[boneIndex];
// 	Matrix4 trans = Matrix4(TRANS, correctData->translation);
// 	Quat rotation = correctData->rotation;
// 	Matrix4 scale = Matrix4(SCALE, correctData->scale);
// 
// 	*world = scale * rotation * trans * parentWorld;
// 
// 	debugModel->SetWorld(*world);
}

void SkeletonBone::UpdateRootAnimationTransform(FrameData* boneData, const Matrix4& par)
{
	Matrix4 parentWorld = par;

	FrameData* correctData = &boneData[boneIndex];
	Matrix4 trans = Matrix4(TRANS, correctData->translation);
	Quat rotation = correctData->rotation;
	Matrix4 scale = Matrix4(SCALE, correctData->scale);

	*world = scale * rotation * trans * parentWorld;

	debugModel->SetWorld(*world);
}
