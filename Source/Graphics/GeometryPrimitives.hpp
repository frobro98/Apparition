// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Vector2.hpp"
#include "Math/Vector3.hpp"
#include "BasicTypes/Color.hpp"

	// NOTE - MSVC Internal compiler error occurs when these variables are all defaulted with '= 0'
	// Don't know how to approach this issue. This is the error given:
	//
	// INTERNAL COMPILER ERROR in 'C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Tools\MSVC\14.16.27023\bin\HostX86\x86\CL.exe'
	// Please choose the Technical Support command on the Visual C++
	// Help menu, or open the Technical Support help file for more information
	//
struct Vertex
{
	Vector3 position;
	Vector3 normal;
	Vector3 tangent;
	Vector2 texCoords;

	friend bool operator==(const Vertex& vert0, const Vertex& vert1)
	{
		return vert0.position == vert1.position &&
			vert0.normal == vert1.normal &&
			vert0.tangent == vert1.tangent &&
			vert0.texCoords == vert1.texCoords;
	}

	friend bool operator<(const Vertex& vert0, const Vertex& vert1)
	{
		return vert0.position < vert1.position &&
			vert0.normal < vert1.normal &&
			vert0.tangent < vert1.tangent &&
			vert0.texCoords < vert1.texCoords;
	}

	friend bool operator>(const Vertex& vert0, const Vertex& vert1)
	{
		return vert0.position > vert1.position &&
			vert0.normal > vert1.normal &&
			vert0.tangent > vert1.tangent &&
			vert0.texCoords > vert1.texCoords;
	}
};

struct PrimitiveVertex
{
	Vector3 position;
	Vector2 texCoords;
	Color32 color;
};

// PrimitiveVertex MakePrimitiveVertex(Vector2 position, Vector2 texCoods, Color color);
// PrimitiveVertex MakePrimitiveVertex(Vector3 position, Vector2 texCoods, Color color);
// PrimitiveVertex MakePrimitiveVertex(Vector3 position, Vector2 texCoods, Color32 color);

struct VertexBoneWeights
{
	static constexpr u32 MaxWeightInfluences = 4;

	float weights[MaxWeightInfluences] = { 0.f, 0.f, 0.f, 0.f };
	u32 boneIndices[MaxWeightInfluences] = { 0, 0, 0, 0 };
};

struct Face
{
	u32 v0;
	u32 v1;
	u32 v2;
};