// Copyright 2020, Nathan Blane

#include "Mesh.h"
#include "File/DirectoryLocations.hpp"
#include "File/FileSystem.hpp"
#include "Model/ModelFileHeader.h"
#include "Graphics/GraphicsInterface.hpp"

#define offset(s, m) ((void *)((unsigned int)(&s[0].m) - (unsigned int)(s)))

static void FillMeshVertexBuffer(NativeVertexBuffer* vertBuffer, const DynamicArray<Vertex>& vertices)
{
	Assert(vertBuffer);
	void* data = GetGraphicsInterface().LockVertexBuffer(vertBuffer, vertices.SizeInBytes(), 0);
	Memcpy(data, vertices.GetData(), vertices.SizeInBytes());
	GetGraphicsInterface().UnlockVertexBuffer(vertBuffer);
}

static void FillMeshIndexBuffer(NativeIndexBuffer* indexBuffer, const DynamicArray<Face>& faces)
{
	Assert(indexBuffer);
	void* data = GetGraphicsInterface().LockIndexBuffer(indexBuffer, faces.SizeInBytes(), 0);
	Memcpy(data, faces.GetData(), faces.SizeInBytes());
	GetGraphicsInterface().UnlockIndexBuffer(indexBuffer);
}

Mesh::Mesh(const DynamicArray<Vertex>& vertList, const DynamicArray<Face>& faceList)
	: vertexBuffer(GetGraphicsInterface().CreateVertexBuffer(vertList.SizeInBytes())),
	indexBuffer(GetGraphicsInterface().CreateIndexBuffer(faceList.SizeInBytes(), sizeof(Face) / 3))
{
	FillMeshVertexBuffer(vertexBuffer, vertList);
	FillMeshIndexBuffer(indexBuffer, faceList);
}

Mesh::Mesh(const DynamicArray<Vertex>& vertList, const DynamicArray<Face>& faceList, const SphereBounds& colInfo)
	: vertexBuffer(GetGraphicsInterface().CreateVertexBuffer(vertList.SizeInBytes())),
	indexBuffer(GetGraphicsInterface().CreateIndexBuffer(faceList.SizeInBytes(), sizeof(Face) / 3)),
	boundingSphere(colInfo)
{
	FillMeshVertexBuffer(vertexBuffer, vertList);
	FillMeshIndexBuffer(indexBuffer, faceList);
}

Mesh::Mesh(const DynamicArray<Vertex>& vertList, const DynamicArray<Face>& faceList, const DynamicArray<VertexBoneWeights>& /*weights*/, const SphereBounds& colInfo)
	: vertexBuffer(GetGraphicsInterface().CreateVertexBuffer(vertList.SizeInBytes())),
	indexBuffer(GetGraphicsInterface().CreateIndexBuffer(faceList.SizeInBytes(), sizeof(Face) / 3)),
	boundingSphere(colInfo)
{
	FillMeshVertexBuffer(vertexBuffer, vertList);
	FillMeshIndexBuffer(indexBuffer, faceList);
}

Mesh::~Mesh()
{
	GetGraphicsInterface().DestroyVertexBuffer(vertexBuffer);
	GetGraphicsInterface().DestroyIndexBuffer(indexBuffer);
}
