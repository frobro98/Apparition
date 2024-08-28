// Copyright 2020, Nathan Blane

#pragma once

struct Vector2;
struct Vector3;
struct Vector4;

namespace Internal
{
// TODO - Find a way to simplify this vector behavior. Theres a lot of duplicate behavior here...
// NOTE - This is where the switch will be to actually implement SIMD vs regular

float VectorDotVector(const Vector2& v0, const Vector2& v1);
void VectorNormalize(Vector2& v);
float VectorMagnitudeSqr(const Vector2& v);
float VectorMagnitude(const Vector2& v);
float VectorInverseMagnitude(const Vector2& v);
float VectorAngleBetweenVectors(const Vector2& v0, const Vector2& v1);

float VectorDotVector(const Vector3& v0, const Vector3& v1);
void VectorCrossVector(const Vector3& v0, const Vector3& v1, Vector3& perpVector);
void VectorNormalize(Vector3& v);
float VectorMagnitudeSqr(const Vector3& v);
float VectorMagnitude(const Vector3& v);
float VectorInverseMagnitude(const Vector3& v);
float VectorAngleBetweenVectors(const Vector3& v0, const Vector3& v1);

float VectorDotVector(const Vector4& v0, const Vector4& v1);
void VectorCrossVector(const Vector4& v0, const Vector4& v1, Vector4& perpVector);
void VectorNormalize(Vector4& v);
float VectorMagnitudeSqr(const Vector4& v);
float VectorMagnitude(const Vector4& v);
float VectorInverseMagnitude(const Vector4& v);
float VectorAngleBetweenVectors(const Vector4& v0, const Vector4& v1);

// Vector Math
void VectorAddTo(Vector2& v0, const Vector2& v1);
void VectorSubTo(Vector2& v0, const Vector2& v1);
void VectorMultiTo(Vector2& v0, const Vector2& v1);
void VectorMultiTo(Vector2& v0, float s);
void VectorDivTo(Vector2& v0, float s);

void VectorAddTo(Vector3& v0, const Vector3& v1);
void VectorSubTo(Vector3& v0, const Vector3& v1);
void VectorMultiTo(Vector3& v0, float s);
void VectorDivTo(Vector3& v0, float s);

void VectorAddTo(Vector4& v0, const Vector4& v1);
void VectorSubTo(Vector4& v0, const Vector4& v1);
void VectorMultiTo(Vector4& v0, float s);
void VectorDivTo(Vector4& v0, float s);

}