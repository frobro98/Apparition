// Copyright 2020, Nathan Blane

#pragma once

#include <cstdint>

#include "CoreFlags.hpp"

using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;

using uptr = uintptr_t;

using unichar =  wchar_t;
using ansichar = char;

#ifdef UNICODE
using tchar = unichar;
#define text(x) L##x
#else
using tchar =  ansichar;
#define text(x) x
#endif

using f32 = float;
using f64 = double;

using b32 = u32;
