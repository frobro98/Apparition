// Copyright 2020, Nathan Blane

#pragma once

struct Quat;
struct Vector4;
class Matrix4;

namespace Internal
{

float QuaternionDotQuaternion(const Quat& q0, const Quat& q1);
void  QuaternionNormalize(Quat& q);
void  QuaternionInverse(Quat& q);
void  QuaternionSetMatrix(Quat& q, const Matrix4& m);
float QuaternionMagnitude(const Quat& q);
float QuaternionMagnitudeSqr(const Quat& q);
float QuaternionMagnitudeInverse(const Quat& q);
void QuaternionLqcvq(const Quat& q, const Vector4& v, Vector4& out);
void QuaternionLqvqc(const Quat& q, const Vector4& v, Vector4& out);
void QuaternionMultiByElement(Quat& q0, const Quat& q1);

void QuaternionAddTo(Quat& q0, const Quat& q1);
void QuaternionSubTo(Quat& q0, const Quat& q1);
void QuaternionMultiTo(Quat& q0, const Quat& q1);
void QuaternionDivTo(Quat& q0, const Quat& q1);

void QuaternionAddTo(Quat& q, float val);
void QuaternionSubTo(Quat& q, float val);
void QuaternionSubTo(float val, Quat& q);
void QuaternionMultiTo(Quat& q, float val);
void QuaternionDivTo(float val, Quat& q);
void QuaternionDivTo(Quat& q, float val);

}