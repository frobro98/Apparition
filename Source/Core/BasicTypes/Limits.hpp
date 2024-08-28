#pragma once


#include "BasicTypes/Intrinsics.hpp"

// Intrinsic Max
constexpr u8  U8Max = 0xff;
constexpr u8  I8Max = 0x7f;
constexpr u16 U16Max = 0xffff;
constexpr u16 I16Max = 0x7fff;
constexpr u32 U32Max = 0xffffffff;
constexpr i32 I32Max = 0x7fffffff;
constexpr u64 U64Max = 0xffffffffffffffff;
constexpr i64 I64Max = 0x7fffffffffffffff;

// Intrinsic Min
constexpr u8  U8Min = 0x0;
constexpr u16 U16Min = 0x0000;
constexpr u32 U32Min = 0x00000000;
constexpr u64 U64Min = 0x0000000000000000;
constexpr i8  I8Min = (-127 - 1);
constexpr i16 I16Min = (-32'767 - 1);
constexpr i32 I32Min = (-2'147'483'647 - 1);
constexpr i64 I64Min = (-9'223'372'036'854'775'807 - 1);
