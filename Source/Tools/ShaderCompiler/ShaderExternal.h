// Copyright 2020, Nathan Blane

#pragma once

#include "CoreFlags.hpp"

// #if Vulkan
WALL_WRN_PUSH
#include "glslang/Public/ShaderLang.h"
#include "spirv/GlslangToSpv.h"
#include "spirv/disassemble.h"
#include "spirv_reflect/spirv_reflect.h"
WALL_WRN_POP
//#endif