// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"

#define MUSA_VERSION(major, minor, patch) \
			((major << 24) | ((minor & 0xff) << 16) | (patch))

constexpr u32 MusaVersion = MUSA_VERSION(0, 0, 0);
constexpr u32 MusaVersionMajor = (MusaVersion >> 24);
constexpr u32 MusaVersionMinor = ((MusaVersion >> 16) & 0xff);
constexpr u32 MusaVersionPatch = (MusaVersion & 0xffff);

constexpr u32 MusaImporterVersion = MUSA_VERSION(0, 0, 1);