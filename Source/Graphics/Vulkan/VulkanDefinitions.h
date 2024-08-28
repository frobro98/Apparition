// Copyright 2020, Nathan Blane

#pragma once

#include "Debugging/Assertion.hpp"
#include "CoreFlags.hpp"

//WALL_WRN_PUSH
#define VK_PLATFORM_SURFACE_EXTENSION VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
//WALL_WRN_POP

#define CHECK_VK(expression) Assert(expression == VK_SUCCESS)

static PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_ = nullptr;
#define vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT_

static PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_ = nullptr;
#define vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT_

static PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT_ = nullptr;
#define vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT_

static PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT_ = nullptr;
#define vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT_

static PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT_ = nullptr;
#define vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT_

static PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT_ = nullptr;
#define vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT_

static PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT_ = nullptr;
#define vkQueueInsertDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT_

static PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT_ = nullptr;
#define vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT_

static PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT_ = nullptr;
#define vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT_

static PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT_ = nullptr;
#define vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT_


// TODO - Create vulkan allocator for hook into my memory system
