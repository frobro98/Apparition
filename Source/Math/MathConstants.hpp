// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

namespace Math
{
constexpr f32 Zero = 0.0f;							// ZERO 
constexpr f32 One = 1.0f;							// ONE 

constexpr f64 Infinity = 1e+300 * 1e+300;

constexpr f64 NatLogOf2_dbl = 0.693147180559945309;
constexpr f64 NatLogOf10_dbl = 2.302585092994045684;
constexpr f32 NatLogOf2 = (f32)NatLogOf2_dbl;
constexpr f32 NatLogOf10 = (f32)NatLogOf10_dbl;

constexpr f64 SqrtOf2_dbl = 1.414213562373095;
constexpr f64 InvSqrtOf2_dbl = 1.f / SqrtOf2_dbl;

// Pi
constexpr f64 Pi_dbl =		3.14159265358979323846;
constexpr f64 PiOver2_dbl = 1.57079632687948966192;
constexpr f64 PiOver3_dbl = 1.04719755119659774615;
constexpr f64 PiOver4_dbl = 0.78539816339744830961;
constexpr f64 PiOver8_dbl = 0.39269908169872415481;

constexpr f64 TwoPi_dbl = 6.28318530717958647692;
constexpr f64 TwoPiOver3_dbl = 2.09439510239319549231;
constexpr f64 ThreePiOver4_dbl = 2.35619449019234492885;
constexpr f64 ThreePiOver8_dbl = 1.17809724509617246442;
constexpr f64 FivePiOver8_dbl = 1.96349540849362077404;
constexpr f64 SevenPiOver8_dbl = 2.74889357189106908365;

constexpr f64 PiOver180_dbl = 0.01745329251994329577;
constexpr f64 InvPiOver180_dbl = 57.2957795130823208768;

constexpr f32 Pi = (f32)Pi_dbl;
constexpr f32 PiOver2 = (f32)PiOver2_dbl;
constexpr f32 PiOver3 = (f32)PiOver3_dbl;
constexpr f32 PiOver4 = (f32)PiOver4_dbl;
constexpr f32 PiOver8 = (f32)PiOver8_dbl;

constexpr f32 TwoPi = (f32)TwoPi_dbl;
constexpr f32 TwoPiOver3 = (f32)TwoPiOver3_dbl;
constexpr f32 ThreePiOver4 = (f32)ThreePiOver4_dbl;
constexpr f32 ThreePiOver8 = (f32)ThreePiOver8_dbl;
constexpr f32 FivePiOver8 = (f32)FivePiOver8_dbl;
constexpr f32 SevenPiOver8 = (f32)SevenPiOver8_dbl;

constexpr f32 PiOver180 = (f32)PiOver180_dbl;
constexpr f32 InvPiOver180 = (f32)InvPiOver180_dbl;

constexpr f32 SqrtOf2 = (f32)SqrtOf2_dbl;
constexpr f32 InvSqrtOf2 = (f32)InvSqrtOf2_dbl;

constexpr f32 InternalTolerence = 0.0001f;        // constant for internal tolerances

}
