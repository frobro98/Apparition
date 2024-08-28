// Copyright 2020, Nathan Blane

#include "MatrixFunctions.hpp"
#include "MathFunctions.hpp"
#include "Quat.hpp"
#include "Debugging/Assertion.hpp"

namespace Math
{
Matrix4 ConstructViewMatrix(const Vector4& position, const Vector4& lookAt, const Vector4& up)
{
	Vector4 viewForward = position - lookAt;
	viewForward.Normalize();

	Vector4 viewRight = up.Cross(viewForward);
	viewRight.Normalize();

	Vector4 viewUp = viewForward.Cross(viewRight);
	viewUp.Normalize();

	return ConstructViewMatrix(position, viewForward, viewRight, viewUp);
}
Matrix4 ConstructViewMatrix(const Vector4& position, const Vector4& forward, const Vector4& right, const Vector4& up)
{
	Matrix4 view = Matrix4(
		Vector4(right.x, up.x, -forward.x, 0.f),
		Vector4(right.y, up.y, -forward.y, 0.f),
		Vector4(right.z, up.z, -forward.z, 0.f),
		Vector4(-(position.Dot(right)), -(position.Dot(up)), (position.Dot(forward)))
	);

	AssertFunc([&] 
	{
		Matrix4 viewRot = Matrix4(
			right,
			up,
			-forward,
			Vector4(0.f, 0.f, 0.f, 1.f)
		);

		return (view * ConstructFastInverseViewMatrix(position, Quat(viewRot))).IsIdentity();
	}, "View inverse isn't calculated correctly!");

	return view;
}

// Found how to construct a view matrix from position and rotation from here:
//	https://gamedev.stackexchange.com/questions/178643/the-view-matrix-finally-explained
Matrix4 ConstructViewMatrix(const Vector4& position, const Quat& rotation)
{
	Matrix4 invRot = Matrix4(rotation).GetTranspose();
	Matrix4 trans = Matrix4(TRANS, -position);
	return trans * invRot;
}

Matrix4 ConstructPerspectiveMatrix(f32 fovDeg, f32 aspectRatio, f32 nearPlane, f32 farPlane)
{
	// NOTE: This is an OpenGL projection matrix. The adjustment happens in the shader currently

	NOT_USED f32 tanHalfFOV = Math::Tan(Math::DegreesToRadians(fovDeg) * .5f);
	NOT_USED f32 nearHeight = 2.f * Math::Tan(Math::DegreesToRadians(fovDeg) * .5f) * nearPlane;
	NOT_USED f32 nearWidth = nearHeight * aspectRatio;
	NOT_USED f32 doubleNearPlane = 2.f * nearPlane;
	NOT_USED f32 planeDifference = farPlane - nearPlane;

	Matrix4 projection;
	projection[m0] = 1.f / (aspectRatio * tanHalfFOV);
	projection[m1] = 0;
	projection[m2] = 0;
	projection[m3] = 0;

	projection[m4] = 0;
	projection[m5] = 1.f / tanHalfFOV;
	projection[m6] = 0;
	projection[m7] = 0;

	projection[m8] = 0;
	projection[m9] = 0;
	projection[m10] = -(farPlane + nearPlane) / (planeDifference);
	projection[m11] = -1.f;

	projection[m12] = 0;
	projection[m13] = 0;
	projection[m14] = -(farPlane * nearPlane) / planeDifference;
	projection[m15] = 0;

	// TODO - This is a vulkan adjustment. This should be something that gets adjusted for all graphics apis
	NOT_USED Matrix4 clipAdjustGL = Matrix4(
		Vector4(1.f, 0.f, 0.f, 0.f),
		Vector4(0.f, -1.f, 0.f, 0.f),
		Vector4(0.f, 0.f, .5f, 0),
		Vector4(0.f, 0.f, .5f, 1)
	);

	NOT_USED Matrix4 clipAdjust = Matrix4(
		Vector4(1.f, 0.f, 0.f, 0.f),
		Vector4(0.f, -1.f, 0.f, 0.f),
		Vector4(0.f, 0.f, 1.f, 0),
		Vector4(0.f, 0.f, 0.f, 1)
	);

	projection = projection * clipAdjustGL;

	Assert((projection * ConstructFastInversePerspectiveMatrix(projection)).IsIdentity());

	return projection;
}

Matrix4 ConstructOrthographicMatrix(f32 width, f32 height, f32 nearPlane, f32 farPlane)
{
	const f32 halfWidth = width * .5f;
	const f32 halfHeight = height * .5f;
	return ConstructOrthographicMatrix(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
}

Matrix4 ConstructOrthographicMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 nearPlane, f32 farPlane)
{
	// NOTE: This is an OpenGL projection matrix. The adjustment happens in the shader currently
	Matrix4 projection;
	projection[m0] = 2.f / (right - left);
	projection[m1] = 0;
	projection[m2] = 0;
	projection[m3] = -(right + left) / (right - left);

	projection[m4] = 0;
	projection[m5] = 2.f / (top - bottom);
	projection[m6] = 0;
	projection[m7] = -(top + bottom) / (top - bottom);

	projection[m8] = 0;
	projection[m9] = 0;
	projection[m10] = -2.f / (farPlane - nearPlane);
	projection[m11] = -(farPlane + nearPlane) / (farPlane - nearPlane);

	projection[m12] = 0;
	projection[m13] = 0;
	projection[m14] = 0;
	projection[m15] = 1;

	Assert((projection * ConstructFastInverseOrthographicMatrix(left, right, bottom, top, nearPlane, farPlane)).IsEqual(Matrix4(IDENTITY)));

	return projection;
}

Matrix4 ConstructScreenTransformMatrix(f32 screenWidth, f32 screenHeight, f32 worldDepth)
{
	f32 halfScreenWidth = screenWidth * .5f;
	f32 halfScreeHeight = screenHeight * .5f;
	f32 halfDepth = worldDepth * .5f;
	Matrix4 projection;
	projection[m0] = halfScreenWidth;
	projection[m1] = 0;
	projection[m2] = 0;
	projection[m3] = 0;

	projection[m4] = 0;
	projection[m5] = -halfScreeHeight;
	projection[m6] = 0;
	projection[m7] = 0;

	projection[m8] = 0;
	projection[m9] = 0;
	projection[m10] = halfDepth;
	projection[m11] = 0;

	projection[m12] = halfScreenWidth;
	projection[m13] = halfScreeHeight;
	projection[m14] = halfDepth;
	projection[m15] = 1;

	Assert((projection * ConstructFastInverseScreenMatrix(screenWidth, screenHeight, worldDepth)).IsEqual(Matrix4(IDENTITY)));

	return projection;
}

Matrix4 ConstructFastInverseViewMatrix(const Vector4& position, const Quat& rotation)
{
	Matrix4 rot = Matrix4(rotation);
	Matrix4 trans = Matrix4(TRANS, position);
	return rot * trans;
}

Matrix4 ConstructFastInversePerspectiveMatrix(const Matrix4& perspective)
{
	f32 a = perspective[m0];
	f32 b = perspective[m5];
	f32 c = perspective[m10];
	f32 d = perspective[m14];

	return Matrix4(
		Vector4(1.f / a, 0.f, 0.f, 0.f),
		Vector4(0.f, 1.f / b, 0.f, 0.f),
		Vector4(0.f, 0.f, 0.f, 1.f / d),
		Vector4(0.f, 0.f, -1.f, c / d)
	);
}

Matrix4 ConstructFastInverseOrthographicMatrix(f32 width, f32 height, f32 nearPlane, f32 farPlane)
{
	const f32 halfWidth = width * .5f;
	const f32 halfHeight = height * .5f;
	return ConstructFastInverseOrthographicMatrix(-halfWidth, halfWidth, -halfHeight, halfHeight, nearPlane, farPlane);
}

Matrix4 ConstructFastInverseOrthographicMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 nearPlane, f32 farPlane)
{
	Matrix4 projection;
	projection[m0] = (right - left) * .5f;
	projection[m1] = 0;
	projection[m2] = 0;
	projection[m3] = (right + left) * .5f;

	projection[m4] = 0;
	projection[m5] = (top - bottom) * .5f;
	projection[m6] = 0;
	projection[m7] = (top + bottom) * .5f;

	projection[m8] = 0;
	projection[m9] = 0;
	projection[m10] = -(farPlane - nearPlane) * .5f;
	projection[m11] = -(farPlane + nearPlane) * .5f;

	projection[m12] = 0;
	projection[m13] = 0;
	projection[m14] = 0;
	projection[m15] = 1;

	//Assert((projection * ConstructOrthographicMatrix(left, right, bottom, top, nearPlane, farPlane)).IsEqual(Matrix(IDENTITY)));
	return projection;
}

Matrix4 ConstructFastInverseScreenMatrix(f32 screenWidth, f32 screenHeight, f32 worldDepth)
{
	Matrix4 projection;
	projection[m0] = 2.f / screenWidth;
	projection[m1] = 0;
	projection[m2] = 0;
	projection[m3] = 0;

	projection[m4] = 0;
	projection[m5] = -2.f / screenHeight;
	projection[m6] = 0;
	projection[m7] = 0;

	projection[m8] = 0;
	projection[m9] = 0;
	projection[m10] = 2.f / worldDepth;
	projection[m11] = 0;

	projection[m12] = -1.f;
	projection[m13] = 1.f;
	projection[m14] = -1.f;
	projection[m15] = 1;

	return projection;
}

MATH_API Matrix4 GetTransposeAdjointWithoutTranslation(const Matrix4& m)
{
	// Just constructing the cofactor matrix of a 3x3
	// [ + - + ]
	// [ - + - ]
	// [ + - + ]
	Matrix4 result;

	result[m0] = m[m5] * m[m10] - m[m6] * m[m9];
	result[m1] = m[m6] * m[m8] - m[m4] * m[m10];
	result[m2] = m[m4] * m[m9] - m[m5] * m[m8];
	result[m3] = 0.f;

	result[m4] = m[m2] * m[m9] - m[m1] * m[m10];
	result[m5] = m[m0] * m[m10] - m[m2] * m[m8];
	result[m6] = m[m1] * m[m8] - m[m0] * m[m9];
	result[m7] = 0.f;

	result[m8] = m[m1] * m[m6] - m[m2] * m[m5];
	result[m9] = m[m2] * m[m4] - m[m0] * m[m6];
	result[m10] = m[m0] * m[m5] - m[m1] * m[m4];
	result[m11] = 0.f;

	result[m12] = 0.f;
	result[m13] = 0.f;
	result[m14] = 0.f;
	result[m15] = 1.f;

	return result;
}

}