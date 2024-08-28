// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/UniquePtr.hpp"
//#include "Malloc16Align.h"
#include "Containers/DynamicArray.hpp"
#include "Mesh/MeshRenderInfo.hpp"

class Mesh;
class Matrix4;
class Material;
class Scene;
class SkeletonBone;
class SkeletonInstance;
struct BonePoseData;

class Model
{
public:
	Model(Mesh* mesh, Material* mat);
	~Model();

	void SetWorld(const Matrix4& world);
	void SetBonePose(const DynamicArray<BonePoseData>& poses);
	void SetMaterial(Material* mat);
	void SetScene(Scene& scene);
	void SetActive(bool isActive);
	void SetActiveSkeleton(SkeletonInstance& skel);

	SkeletonInstance* GetSkeleton() { return skeleton; }
	const Matrix4& GetWorld() const { return world; }
	Material* GetMaterial() const { return material; }
	Mesh* GetMesh() const { return mesh; }

	const Matrix4* GetBoneMats() const;
	DynamicArray<BonePoseData> GetPoseData() const;

	MeshRenderInfo& GetRenderInfo() const { return *renderInfo; }

private:
	void SetupMeshRenderInfo();

private:
	static constexpr u32 MaxBones = 120;

	Matrix4 boneMats[MaxBones];
	Matrix4 world;
	DynamicArray<BonePoseData> poseData;
	SkeletonInstance* skeleton = nullptr;
	Mesh* mesh;
	Material* material;
	UniquePtr<MeshRenderInfo> renderInfo;
	Scene* scene = nullptr;
	bool active = true;
};