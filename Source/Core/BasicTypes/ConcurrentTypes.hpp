// Copyright 2020, Nathan Blane

#pragma once

#include <atomic>
#include "BasicTypes/Intrinsics.hpp"

using uatom64 = std::atomic<u64>;
using atom64 = std::atomic<i64>;

using uatom32 = std::atomic<u32>;
using atom32 = std::atomic<i32>;

using uatom16 = std::atomic<u16>;
using atom16 = std::atomic<i16>;

using uatom8 = std::atomic<u8>;
using atom8 = std::atomic<i8>;

using atomf = std::atomic<f32>;
using atomd = std::atomic <f64>;

using uflag = std::atomic_flag;

static_assert(uatom64::is_always_lock_free, "64-bit unsigned integer should be always lock-free!");
static_assert(uatom32::is_always_lock_free, "32-bit unsigned integer should be always lock-free!");
static_assert(uatom16::is_always_lock_free, "16-bit unsigned integer should be always lock-free!");
static_assert(uatom8::is_always_lock_free, "8-bit unsigned integer should be always lock-free!");

static_assert(atom64::is_always_lock_free, "64-bit integer should be always lock-free!");
static_assert(atom32::is_always_lock_free, "32-bit integer should be always lock-free!");
static_assert(atom16::is_always_lock_free, "16-bit integer should be always lock-free!");
static_assert(atom8::is_always_lock_free, "8-bit integer should be always lock-free!");

static_assert(atomf::is_always_lock_free, "32-bit floating-point should be always lock-free!");
static_assert(atomd::is_always_lock_free, "64-bit floating-point should be always lock-free!");
