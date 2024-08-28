// Copyright 2020, Nathan Blane

#include "ViewFrustum.hpp"
#include "Math/BoundsVolumes.hpp"

ViewFrustum::ViewFrustum(
	const Matrix4& viewMatrix,
	const Vector4& origin,
	const Extents2D& nearExtents,
	const Extents2D& farExtents,
	f32 nearPlane,
	f32 farPlane
)
{
	Vector4 forward(viewMatrix[m2], viewMatrix[m6], viewMatrix[m10]);
	Vector4 right(viewMatrix[m0], viewMatrix[m4], viewMatrix[m8]);
	Vector4 up(viewMatrix[m1], viewMatrix[m5], viewMatrix[m9]);
	f32 nearWidth = nearExtents.width;
	f32 nearHeight = nearExtents.height;
	f32 farWidth = farExtents.width;
	f32 farHeight = farExtents.height;

	nearTopLeft = origin - forward * nearPlane + up * nearHeight * 0.5f - right * nearWidth * 0.5f;
	nearTopRight = origin - forward * nearPlane + up * nearHeight * 0.5f + right * nearWidth * 0.5f;
	nearBottomLeft = origin - forward * nearPlane - up * nearHeight * 0.5f - right * nearWidth * 0.5f;
	nearBottomRight = origin - forward * nearPlane - up * nearHeight * 0.5f + right * nearWidth * 0.5f;

	farTopLeft = origin - forward * farPlane + up * farHeight * 0.5f - right * farWidth * 0.5f;
	farTopRight = origin - forward * farPlane + up * farHeight * 0.5f + right * farWidth * 0.5f;
	farBottomLeft = origin - forward * farPlane - up * farHeight * 0.5f - right * farWidth * 0.5f;
	farBottomRight = origin - forward * farPlane - up * farHeight * 0.5f + right * farWidth * 0.5f;
}

bool ViewFrustum::DoesSphereIntersect(const SphereBounds& bounds)
{
	Vector4 pos = bounds.position;
	float radius = bounds.radius;

	// Near Plane Test
	Vector4 vec = pos - nearBottomRight;
	float nearDist = vec.Dot(nearPlaneNormal);

	// Far Plane Test
	vec = pos - farTopLeft;
	float farDist = vec.Dot(farPlaneNormal);

	// Left Plane Test
	vec = pos - farTopLeft;
	float leftDist = vec.Dot(leftNormal);

	// Right Plane Test
	vec = pos - nearBottomRight;
	float rightDist = vec.Dot(rightNormal);

	// Top Plane Test
	vec = pos - farTopLeft;
	float topDist = vec.Dot(topNormal);

	// Bottom Plane Test
	vec = pos - nearBottomRight;
	float bottomDist = vec.Dot(bottomNormal);

	return nearDist < radius &&
		farDist < radius &&
		leftDist < radius &&
		rightDist < radius &&
		topDist < radius &&
		bottomDist < radius;
}

void ViewFrustum::CalculateCollisionNormals()
{
	Vector4 A = nearBottomLeft - nearTopLeft;
	Vector4 B = nearTopRight - nearTopLeft;
	Vector4 C = farTopLeft - nearTopLeft;

	nearPlaneNormal = A.Cross(B);
	nearPlaneNormal.Normalize();

	leftNormal = C.Cross(A);
	leftNormal.Normalize();

	topNormal = B.Cross(C);
	topNormal.Normalize();

	A = farBottomLeft - farBottomRight;
	B = farTopRight - farBottomRight;
	C = nearBottomRight - farBottomRight;

	farPlaneNormal = A.Cross(B);
	farPlaneNormal.Normalize();

	rightNormal = B.Cross(C);
	rightNormal.Normalize();

	bottomNormal = C.Cross(A);
	bottomNormal.Normalize();
}
