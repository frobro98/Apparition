// Copyright 2020, Nathan Blane

#include "Math/MathFunctions.hpp"
#include "MeshManager.h"
#include "Mesh.h"
#include "File/FileSystem.hpp"
#include "Model/ModelFileHeader.h"
#include "File/DirectoryLocations.hpp"
#include "Camera/Camera.h"
#include "MeshUtilities.hpp"

Mesh* MeshManager::LoadPrimitive(Primitive primitiveMesh)
{
	switch (primitiveMesh)
	{
	case Primitive::Sphere:
		return LoadSpherePrimitive();
	case Primitive::Box:
		return LoadBoxPrimitive();
	case Primitive::Plane:
		return LoadPlanePrimitive();
	case Primitive::Pyramid:
		return LoadPyramidPrimitive();
	default:
		Assert(false);
	}

	return nullptr;
}

Mesh* MeshManager::LoadFrustumForCamera(NOT_USED Camera* cam)
{
	Vertex vertices[9];
	Face faces[8];

	// TODO - Come back to managing the model of a frustum
// 	cam->Update();
// 	Frustum camFrustum = cam->GetCameraFrustum();
// 
// 	faces[0].v0 = 0;
// 	faces[0].v1 = 1;
// 	faces[0].v2 = 2;
// 
// 	faces[1].v0 = 0;
// 	faces[1].v1 = 2;
// 	faces[1].v2 = 3;
// 
// 	faces[2].v0 = 0;
// 	faces[2].v1 = 3;
// 	faces[2].v2 = 4;
// 
// 	faces[3].v0 = 0;
// 	faces[3].v1 = 4;
// 	faces[3].v2 = 1;
// 
// 	// far
// 	faces[4].v0 = 1;
// 	faces[4].v1 = 2;
// 	faces[4].v2 = 3;
// 
// 	// far
// 	faces[5].v0 = 3;
// 	faces[5].v1 = 4;
// 	faces[5].v2 = 1;
// 
// 	// near
// 	faces[6].v0 = 5;
// 	faces[6].v1 = 6;
// 	faces[6].v2 = 7;
// 
// 	// near
// 	faces[7].v0 = 7;
// 	faces[7].v1 = 8;
// 	faces[7].v2 = 5;
// 
// 	Vector pos = cam->GetPosition();
// 	vertices[0].x = pos.x;
// 	vertices[0].y = pos.y;
// 	vertices[0].z = pos.z;
// 	vertices[0].nx = 0.000000f;
// 	vertices[0].ny = 0.000000f;
// 	vertices[0].nz = 0.000000f;
// 	vertices[0].u = 0.000000f;
// 	vertices[0].v = 0.000000f;
// 
// 	vertices[1].x = camFrustum.farTopRight.x;
// 	vertices[1].y = camFrustum.farTopRight.y;
// 	vertices[1].z = camFrustum.farTopRight.z;
// 	vertices[1].nx = 0.000000f;
// 	vertices[1].ny = 0.000000f;
// 	vertices[1].nz = 0.000000f;
// 	vertices[1].u = 0.000000f;
// 	vertices[1].v = 0.000000f;
// 
// 	vertices[2].x = camFrustum.farTopLeft.x;
// 	vertices[2].y = camFrustum.farTopLeft.y;
// 	vertices[2].z = camFrustum.farTopLeft.z;
// 	vertices[2].nx = 0.000000f;
// 	vertices[2].ny = 0.000000f;
// 	vertices[2].nz = 0.000000f;
// 	vertices[2].u = 0.000000f;
// 	vertices[2].v = 0.000000f;
// 
// 	vertices[3].x = camFrustum.farBottomLeft.x;
// 	vertices[3].y = camFrustum.farBottomLeft.y;
// 	vertices[3].z = camFrustum.farBottomLeft.z;
// 	vertices[3].nx = 0.000000f;
// 	vertices[3].ny = 0.000000f;
// 	vertices[3].nz = 0.000000f;
// 	vertices[3].u = 0.000000f;
// 	vertices[3].v = 0.000000f;
// 
// 	vertices[4].x = camFrustum.farBottomRight.x;
// 	vertices[4].y = camFrustum.farBottomRight.y;
// 	vertices[4].z = camFrustum.farBottomRight.z;
// 	vertices[4].nx = 0.000000f;
// 	vertices[4].ny = 0.000000f;
// 	vertices[4].nz = 0.000000f;
// 	vertices[4].u = 0.000000f;
// 	vertices[4].v = 0.000000f;
// 
// 	vertices[5].x = camFrustum.nearTopRight.x;
// 	vertices[5].y = camFrustum.nearTopRight.y;
// 	vertices[5].z = camFrustum.nearTopRight.z;
// 	vertices[5].nx = 0.000000f;
// 	vertices[5].ny = 0.000000f;
// 	vertices[5].nz = 0.000000f;
// 	vertices[5].u = 0.000000f;
// 	vertices[5].v = 0.000000f;
// 
// 	vertices[6].x = camFrustum.nearTopLeft.x;
// 	vertices[6].y = camFrustum.nearTopLeft.y;
// 	vertices[6].z = camFrustum.nearTopLeft.z;
// 	vertices[6].nx = 0.000000f;
// 	vertices[6].ny = 0.000000f;
// 	vertices[6].nz = 0.000000f;
// 	vertices[6].u = 0.000000f;
// 	vertices[6].v = 0.000000f;
// 
// 	vertices[7].x = camFrustum.nearBottomLeft.x;
// 	vertices[7].y = camFrustum.nearBottomLeft.y;
// 	vertices[7].z = camFrustum.nearBottomLeft.z;
// 	vertices[7].nx = 0.000000f;
// 	vertices[7].ny = 0.000000f;
// 	vertices[7].nz = 0.000000f;
// 	vertices[7].u = 0.000000f;
// 	vertices[7].v = 0.000000f;
// 
// 	vertices[8].x = camFrustum.nearBottomRight.x;
// 	vertices[8].y = camFrustum.nearBottomRight.y;
// 	vertices[8].z = camFrustum.nearBottomRight.z;
// 	vertices[8].nx = 0.000000f;
// 	vertices[8].ny = 0.000000f;
// 	vertices[8].nz = 0.000000f;
// 	vertices[8].u = 0.000000f;
// 	vertices[8].v = 0.000000f;

	DynamicArray<Vertex> verts(vertices);
	GenerateMeshTangents(verts, faces);

	Mesh* mesh = new Mesh(MOVE(verts), faces);
	MeshNode* node = new MeshNode;
	node->meshName = "Frustum";
	node->mesh = mesh;
	node->next = head;
	head = node;

	return mesh;
}

Mesh* MeshManager::LoadMeshFromPak(u8* modelData, const char * modelName)
{
	ModelFileHeader* header = reinterpret_cast<ModelFileHeader*>(modelData);
	u8* vertPtr = modelData + header->vertBufferOffset;
	u8* facePtr = modelData + header->facesBufferOffset;

	DynamicArray<Vertex> vertices(header->numVerts);
	DynamicArray<Face> faces(header->numFaces);
	SphereBounds boundingSphere = header->boundingSphere;

	Memcpy(vertices.GetData(), vertices.Size() * sizeof(Vertex), vertPtr, header->numVerts * sizeof(Vertex));
	Memcpy(faces.GetData(), faces.Size() * sizeof(Face), facePtr, header->numFaces * sizeof(Face));

	Mesh* mesh = new Mesh(MOVE(vertices), MOVE(faces), boundingSphere);
	MeshNode* node = new MeshNode;
	node->meshName = modelName;
	node->mesh = mesh;

	node->next = head;
	head = node;

	return mesh;
}

Mesh* MeshManager::LoadMeshFromPak(u8 * modelData, u8* skinningData, const char * modelName)
{
	ModelFileHeader* header = reinterpret_cast<ModelFileHeader*>(modelData);
	u8* vertPtr = modelData + header->vertBufferOffset;
	u8* facePtr = modelData + header->facesBufferOffset;


	DynamicArray<Vertex> vertices(header->numVerts);
	size_t verticesBytes = vertices.Size() * sizeof(Vertex);

	DynamicArray<VertexBoneWeights> skinningWeights(header->numVerts);
	size_t skinningWeightBytes = skinningWeights.Size() * sizeof(VertexBoneWeights);

	DynamicArray<Face> faces(header->numFaces);
	size_t facesBytes = faces.Size() * sizeof(Face);

	SphereBounds boundingSphere = header->boundingSphere;

	Memcpy(vertices.GetData(), verticesBytes, vertPtr, verticesBytes);
	Memcpy(skinningWeights.GetData(), skinningWeightBytes, skinningData, skinningWeightBytes);
	Memcpy(faces.GetData(), facesBytes, facePtr, facesBytes);

	Mesh* mesh = new Mesh(MOVE(vertices), MOVE(faces), MOVE(skinningWeights), boundingSphere);
	MeshNode* node = new MeshNode;
	node->meshName = modelName;
	node->mesh = mesh;

	node->next = head;
	head = node;

	return mesh;
}

void MeshManager::UnloadMesh(const char* modelName)
{
	MeshNode* current = head;
	while (current->next != nullptr)
	{
		if (current->next->meshName == modelName)
		{
			MeshNode* node = current->next;
			current->next = current->next->next;
			delete node->mesh;
			delete node;
			break;
		}

		current = current->next;
	}
}

Mesh* MeshManager::FindMesh(const char * modelName)
{
	MeshNode* current = head;
	while (current != nullptr)
	{
		if (current->meshName == modelName)
		{
			return current->mesh;
		}

		current = current->next;
	}
	return nullptr;
}

// Credited by Joey DeVries for this
// Arametric equation for a sphere F(u, v, r) = [cos(u)*sin(v)*r, cos(v), sin(u)*sin(v)*r] where 
// u is longitude [0, 2PI] and v is latitude [0, PI] (note the difference in their range)
Mesh* MeshManager::LoadSpherePrimitive()
{
	constexpr u32 XSegments = 64;
	constexpr u32 YSegments = 64;

	DynamicArray<Vertex> vertices;
	vertices.Reserve((XSegments + 1) * (YSegments + 1));
	for (u32 y = 0; y <= YSegments; ++y)
	{
		for (u32 x = 0; x <= XSegments; ++x)
		{
			float xSeg = (float)x / (float)XSegments;
			float ySeg = (float)y / (float)YSegments;
			float xPos = Math::Cos(xSeg * Math::TwoPi) * Math::Sin(ySeg * Math::Pi);
			float yPos = Math::Cos(ySeg * Math::Pi);
			float zPos = Math::Sin(xSeg * Math::TwoPi) * Math::Sin(ySeg * Math::Pi);

			vertices.Add(
				Vertex{
					Vector3(xPos, yPos, zPos),
					Vector3(xPos, yPos, zPos),
					Vector3(0, 0, 0),
					Vector2(xSeg, ySeg)
				}
			);
		}
	}
	

	DynamicArray<Face> faces;
	faces.Reserve(XSegments * YSegments * 2);
	for (i32 y = 0; y < YSegments; ++y)
	{
		for (i32 x = 0; x < XSegments; ++x)
		{
			faces.Add(Face{
					(u32)((y + 1) * (XSegments + 1) + x),
					(u32)( y		 * (XSegments + 1) + x),
					(u32)( y		 * (XSegments + 1) + x + 1)
				});

			faces.Add(Face{
					(u32)((y + 1) * (XSegments + 1) + x),
					(u32)( y		 * (XSegments + 1) + x + 1),
					(u32)((y + 1) * (XSegments + 1) + x + 1)
				});
		}
	}

	GenerateMeshTangents(vertices, faces);

	Mesh* mesh = new Mesh(MOVE(vertices), MOVE(faces));
	MeshNode* node = new MeshNode;
	node->meshName = Mesh::SphereName;
	node->mesh = mesh;
	node->next = head;
	head = node;

	return mesh;
}

Mesh* MeshManager::LoadBoxPrimitive()
{
	Vertex vertices[] =
	{
		/* Position */         /* Normal */          /* Tangent */   /* UV */

	// Back
	{Vector3(-0.5f, -0.5f, -0.5f),   Vector3(0.0f,  0.0f, -1.0f),    Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f)},
	{Vector3(0.5f, -0.5f, -0.5f),    Vector3(0.0f,  0.0f, -1.0f),    Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f)},
	{Vector3(0.5f,  0.5f, -0.5f),    Vector3(0.0f,  0.0f, -1.0f),    Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f)},
	{Vector3(-0.5f,  0.5f, -0.5f),   Vector3(0.0f,  0.0f, -1.0f),    Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f)},

	// Front
	{Vector3(-0.5f, -0.5f,  0.5f),   Vector3(0.0f,  0.0f, 1.0f),     Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f)},
	{Vector3(0.5f, -0.5f,  0.5f),    Vector3(0.0f,  0.0f, 1.0f),     Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f)},
	{Vector3(0.5f,  0.5f,  0.5f),    Vector3(0.0f,  0.0f, 1.0f),     Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f)},
	{Vector3(-0.5f,  0.5f,  0.5f),   Vector3(0.0f,  0.0f, 1.0f),     Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f)},

	// Left
	{Vector3(-0.5f,  0.5f,  0.5f),   Vector3(-1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f)},
	{Vector3(-0.5f,  0.5f, -0.5f),   Vector3(-1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f)},
	{Vector3(-0.5f, -0.5f, -0.5f),   Vector3(-1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f)},
	{Vector3(-0.5f, -0.5f,  0.5f),   Vector3(-1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f)},

	// Right
	 {Vector3(0.5f,  0.5f,  0.5f),    Vector3(1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f)},
	 {Vector3(0.5f,  0.5f, -0.5f),    Vector3(1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f)},
	 {Vector3(0.5f, -0.5f, -0.5f),    Vector3(1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f)},
	 {Vector3(0.5f, -0.5f,  0.5f),    Vector3(1.0f,  0.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f)},

	 // Bottom
	{Vector3(-0.5f, -0.5f, -0.5f),   Vector3(0.0f, -1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f)},
	{Vector3(0.5f, -0.5f, -0.5f),    Vector3(0.0f, -1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f)},
	{Vector3(0.5f, -0.5f,  0.5f),    Vector3(0.0f, -1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f)},
	{Vector3(-0.5f, -0.5f,  0.5f),   Vector3(0.0f, -1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f)},

	// Top
	{Vector3(-0.5f,  0.5f, -0.5f),   Vector3(0.0f,  1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 0.0f)},
	{Vector3(0.5f,  0.5f, -0.5f),    Vector3(0.0f,  1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 0.0f)},
	{Vector3(0.5f,  0.5f,  0.5f),    Vector3(0.0f,  1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(1.0f, 1.0f)},
	{Vector3(-0.5f,  0.5f,  0.5f),   Vector3(0.0f,  1.0f,  0.0f),   Vector3(0.0f, 0.0f, 0.0f),  Vector2(0.0f, 1.0f)},
	};

	Face faces[] =
	{
		// Backs
		{2, 1, 0},
		{0, 3, 2},

		// Front
		{4, 5, 6},
		{6, 7, 4},

		// Left
		{8, 9, 10},
		{10, 11, 8},

		// Right
		{14, 13, 12},
		{12, 15, 14},

		// Bottom
		{16, 17, 18},
		{18, 19, 16},

		// Top
		{22, 21, 20},
		{20, 23, 22}
	};

	DynamicArray<Vertex> verts(vertices);
	GenerateMeshTangents(verts, faces);

	Mesh* mesh = new Mesh(MOVE(verts), faces);
	MeshNode* node = new MeshNode;
	node->meshName = Mesh::BoxName;
	node->mesh = mesh;
	node->next = head;
	head = node;

	return mesh;
}

Mesh* MeshManager::LoadPlanePrimitive()
{
	Vertex verts[] = 
	{
		/* Position */         /* Normal */     /* Tangent */    /* Binormal */  /* UV */
		{Vector3( 0.5f,  0.5f, 0.f),  Vector3(0.0f, 0.0f, 1.f),   Vector3(0.f, 0.f, 0.f),  Vector2(0.f, 0.f) },
		{Vector3(-0.5f,  0.5f, 0.f),  Vector3(0.0f, 0.0f, 1.f),   Vector3(0.f, 0.f, 0.f),  Vector2(1.f, 0.f) },
		{Vector3(-0.5f, -0.5f, 0.f),  Vector3(0.0f, 0.0f, 1.f),   Vector3(0.f, 0.f, 0.f),  Vector2(1.f, 1.f) },
		{Vector3( 0.5f, -0.5f, 0.f),  Vector3(0.0f, 0.0f, 1.f),   Vector3(0.f, 0.f, 0.f),  Vector2(0.f, 1.f) }
	};

	Face faces[] = 
	{
		{2, 0, 1},
		{2, 3, 0}
	};

	DynamicArray<Vertex> vertices(verts);
	GenerateMeshTangents(vertices, faces);

	Mesh* mesh = new Mesh(MOVE(vertices), faces);
	MeshNode* node = new MeshNode;
	node->meshName = Mesh::PlaneName;
	node->mesh = mesh;
	node->next = head;
	head = node;

	return mesh;
}

Mesh* MeshManager::LoadPyramidPrimitive()
{
	Face faces[] =
	{
		{ 2,1,0 },
		{ 0,3,2 },
		{ 6,5,4 },
		{ 6,7,5 },
		{ 10,9,8 },
		{ 10,11,9 },
		{ 14,13,12 },
		{ 14,15,13 },
		{ 18,17,16 },
		{ 16,19,18 },
		{ 22,21,20 },
		{ 20,23,22 }
	};

	Vertex  verts[] =
	{
		/* Position */                       /* Normal */                     /* Tangent */              /* UV */
		{ Vector3(-0.25f, 0.0f, -0.25f),     Vector3(-0.6f,  0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2{0.0f, 0.0f} },
		{ Vector3(-0.25f, 0.0f, -0.25f),     Vector3(-0.6f, -0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 1.0f) },
		{ Vector3( 0.25f, 0.0f, -0.25f),     Vector3( 0.6f, -0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 1.0f) },
		{ Vector3( 0.0f,  1.0f,  0.0f),      Vector3( 0.6f,  0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 0.0f) },
		{ Vector3( 0.25f, 0.0f, -0.25f),     Vector3( 0.6f, -0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 1.0f) },
		{ Vector3( 0.25f, 0.0f,  0.25f),     Vector3( 0.6f, -0.6f,  0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 1.0f) },
		{ Vector3( 0.0f,  1.0f,  0.0f),      Vector3( 0.6f,  0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 0.0f) },
		{ Vector3( 0.25f, 0.0f,  0.25f),     Vector3( 0.6f,  0.6f,  0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 0.0f) },
		{ Vector3( 0.25f, 0.0f,  0.25f),     Vector3( 0.6f, -0.6f, 0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 1.0f) },
		{ Vector3(-0.25f, 0.0f,  0.25f),     Vector3(-0.6f, -0.6f, 0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 1.0f) },
		{ Vector3( 0.0f,  1.0f,  0.0f),      Vector3( 0.6f,  0.6f, 0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 0.0f) },
		{ Vector3(-0.0f,  1.0f,  0.0f),      Vector3(-0.6f,  0.6f, 0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 0.0f) },
		{ Vector3(-0.25f, 0.0f,  0.25f),     Vector3(-0.6f, -0.6f,  0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 0.0f) },
		{ Vector3(-0.25f, 0.0f, -0.25f),     Vector3(-0.6f, -0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 1.0f) },
		{ Vector3(-0.0f,  1.0f,  0.0f),      Vector3(-0.6f,  0.6f,  0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 0.0f) },
		{ Vector3(-0.0f,  1.0f,  0.0f),      Vector3(-0.6f,  0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 1.0f) },
		{ Vector3(-0.25f, 0.0f,  0.25f),     Vector3(-0.6f, -0.6f,  0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 0.0f) },
		{ Vector3( 0.25f, 0.0f,  0.25f),     Vector3( 0.6f, -0.6f,  0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 0.0f) },
		{ Vector3( 0.25f, 0.0f, -0.25f),     Vector3( 0.6f, -0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 1.0f) },
		{ Vector3(-0.25f, 0.0f, -0.25f),     Vector3(-0.6f, -0.6f, -0.6f),    Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 1.0f) },
		{ Vector3(-0.0f,  1.0f,  0.0f),      Vector3(-0.6f, 0.6f, -0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 1.0f) },
		{ Vector3( 0.0f,  1.0f,  0.0f),      Vector3( 0.6f, 0.6f, -0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 1.0f) },
		{ Vector3( 0.0f,  1.0f,  0.0f),      Vector3( 0.6f, 0.6f,  0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(1.0f, 0.0f) },
		{ Vector3(-0.0f,  1.0f,  0.0f),      Vector3(-0.6f, 0.6f,  0.6f),     Vector3(0.f, 0.f, 0.f),   Vector2(0.0f, 0.0f) },

	};
	
	DynamicArray<Vertex> vertices(verts);
	GenerateMeshTangents(vertices, faces);

	Mesh* mesh = new Mesh(MOVE(vertices), faces);
	MeshNode* node = new MeshNode;
	node->meshName = Mesh::PyramidName;
	node->mesh = mesh;
	node->next = head;
	head = node;

	return mesh;
}

void MeshManager::Initialize()
{
	LoadPrimitive(Primitive::Box);
	LoadPrimitive(Primitive::Plane);
	LoadPrimitive(Primitive::Sphere);
	LoadPrimitive(Primitive::Pyramid);
}

void MeshManager::Deinitialize()
{
	MeshNode* current = head;
	while (current != nullptr)
	{
		MeshNode* node = current->next;
		if (node != nullptr)
		{
			current->next = current->next->next;

			delete node->mesh;
			delete node;
		}

		current = current->next;
	}
}

MeshManager& GetMeshManager()
{
	static MeshManager meshMan;
	return meshMan;
}
