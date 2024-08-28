// Copyright 2020, Nathan Blane

#pragma once

#include "String/String.h"

class Mesh;

enum class Primitive
{
	Sphere,
	Box,
	Plane,
	Pyramid
};

class MeshManager
{
public:

	void Initialize();
	void Deinitialize();

	Mesh* LoadPrimitive(Primitive primitiveMesh);
	Mesh* LoadFrustumForCamera(class Camera* camera);

	Mesh* LoadMeshFromPak(u8* modelData, const char* modelName);
	Mesh* LoadMeshFromPak(u8* modelData, u8* skinningData, const char* modelName);
	void UnloadMesh(const char* modelName);
	Mesh* FindMesh(const char* modelName);

private:

	Mesh* LoadCameraFrustum(class Camera* cam);
	Mesh* LoadSpherePrimitive();
	Mesh* LoadBoxPrimitive();
	Mesh* LoadPlanePrimitive();
	Mesh* LoadPyramidPrimitive();

	Mesh* LoadMeshInternalPak(u8* modelData, const char* modelName);
	Mesh* LoadMeshInternalPak(u8* modelData, u8* skinningData, const char* modelName);
	void UnloadMeshInternal(const char* modelName);

	struct MeshNode
	{
		String meshName;
		MeshNode* next = nullptr;
		Mesh* mesh = nullptr;
	};

	MeshNode* head = nullptr;
};

MeshManager& GetMeshManager();
