// Copyright 2020, Nathan Blane

#pragma once

#include "Math/Matrix4.hpp"
#include "Math/MathAPI.hpp"

namespace Math
{

MATH_API Matrix4 ConstructViewMatrix(const Vector4& position, const Vector4& lookAt, const Vector4& up);
MATH_API Matrix4 ConstructViewMatrix(const Vector4& position, const Vector4& forward, const Vector4& right, const Vector4& up);
MATH_API Matrix4 ConstructViewMatrix(const Vector4& position, const Quat& rotation);

MATH_API Matrix4 ConstructPerspectiveMatrix(f32 fov, f32 aspectRatio, f32 nearPlane, f32 farPlane);
MATH_API Matrix4 ConstructOrthographicMatrix(f32 width, f32 height, f32 nearPlane, f32 farPlane);
MATH_API Matrix4 ConstructOrthographicMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 nearPlane, f32 farPlane);
MATH_API Matrix4 ConstructScreenTransformMatrix(f32 screenWidth, f32 screenHeight, f32 worldDepth = 1.f);

MATH_API Matrix4 ConstructFastInverseViewMatrix(const Vector4& position, const Quat& rotation);
MATH_API Matrix4 ConstructFastInversePerspectiveMatrix(const Matrix4& perspective);
//MATH_API Matrix4 ConstructInversePerspectiveMatrix(f32 fov, f32 aspectRatio, f32 nearPlane, f32 farPlane);
MATH_API Matrix4 ConstructFastInverseOrthographicMatrix(f32 width, f32 height, f32 nearPlane, f32 farPlane);
MATH_API Matrix4 ConstructFastInverseOrthographicMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 nearPlane, f32 farPlane);
MATH_API Matrix4 ConstructFastInverseScreenMatrix(f32 screenWidth, f32 screenHeight, f32 worldDepth = 1.f);

// This function exists for rotating a plane's normal, for the purpose of transforming the plane.
// The adjoint is important for this because it produces the same result as multiplying the plane
// by the inverse of the matrix. Rotation is important because this matrix will only be applied to
// the normal of the plane. Translation doesn't apply, and because the inverse of the rotation
// matrix is its transpose, there's no need to perform the costly inverse operation
//
// Learn more from the "On the Transformation of Surface Normals" paper
//	https://www.glassner.com/wp-content/uploads/2014/04/On-the-Transformation-of-Surface-Normals.pdf
MATH_API Matrix4 GetTransposeAdjointWithoutTranslation(const Matrix4& m);
}