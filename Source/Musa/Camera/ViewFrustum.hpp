// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Extents.hpp"
#include "Math/Vector4.hpp"
#include "Math/Matrix4.hpp"

struct SphereBounds;

class ViewFrustum
{
	ViewFrustum(
		const Matrix4& viewMatrix,
		const Vector4& origin,  
		const Extents2D& nearExtents,
		const Extents2D& farExtents,
		f32 nearPlane,
		f32 farPlane
	);

	bool DoesSphereIntersect(const SphereBounds& bounds);

private:
	void CalculateCollisionNormals();

private:
	// near points
	Vector4 nearTopLeft;
	Vector4 nearTopRight;
	Vector4 nearBottomLeft;
	Vector4 nearBottomRight;

	// far points
	Vector4 farTopLeft;
	Vector4 farTopRight;
	Vector4 farBottomLeft;
	Vector4 farBottomRight;

	// Normals for collision
	Vector4 nearPlaneNormal;
	Vector4 farPlaneNormal;
	Vector4 topNormal;
	Vector4 bottomNormal;
	Vector4 leftNormal;
	Vector4 rightNormal;


};