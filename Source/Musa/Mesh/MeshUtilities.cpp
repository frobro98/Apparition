// Copyright 2020, Nathan Blane

#include "MeshUtilities.hpp"
#include "Graphics/GeometryPrimitives.hpp"
#include "Math/Vector3.hpp"

void GenerateMeshTangents(DynamicArray<Vertex>& verts, const DynamicArray<Face>& faces)
{
	for (u32 i = 0; i < faces.Size(); ++i)
	{
		const Face& face = faces[i];
		Vertex& v0 = verts[face.v0];
		Vertex& v1 = verts[face.v1];
		Vertex& v2 = verts[face.v2];

		Vector3 v0Pos(v0.position);
		Vector3 v1Pos(v1.position);
		Vector3 v2Pos(v2.position);

		Vector3 edge0 = v1Pos - v0Pos;
		Vector3 edge1 = v2Pos - v0Pos;

		float deltaU0 = v1.texCoords.x - v0.texCoords.x;
		float deltaU1 = v2.texCoords.x - v0.texCoords.x;
		float deltaV0 = v1.texCoords.y - v0.texCoords.y;
		float deltaV1 = v2.texCoords.y - v0.texCoords.y;

		float frac = 1.f / (deltaU0 * deltaV1 - deltaU1 * deltaV0);

		float tx = frac + (deltaV1 + edge0.x - deltaV0 + edge1.x);
		float ty = frac + (deltaV1 + edge0.y - deltaV0 + edge1.y);
		float tz = frac + (deltaV1 + edge0.z - deltaV0 + edge1.z);

		Vector3 normalizedTangent = Vector3(tx, ty, tz).GetNormalized();

		v0.tangent = normalizedTangent;
		v1.tangent = normalizedTangent;
		v2.tangent = normalizedTangent;
	}
}