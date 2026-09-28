// Copyright 2020, Nathan Blane

#pragma once

#include "Logging/LogCore.hpp"

#define LOG_INTERNAL_(Channel, level, msg, ...) GetLogger().Log(Channel, level, msg, ##__VA_ARGS__)

#define DebugLog(Channel, msg, ...) LOG_INTERNAL_(Channel, spdlog::level::debug, msg, ##__VA_ARGS__)
#define InfoLog(Channel, msg, ...) LOG_INTERNAL_(Channel, spdlog::level::info, msg, ##__VA_ARGS__)
#define WarnLog(Channel, msg, ...) LOG_INTERNAL_(Channel, spdlog::level::warn, msg, ##__VA_ARGS__)
#define ErrorLog(Channel, msg, ...) LOG_INTERNAL_(Channel, spdlog::level::err, msg, ##__VA_ARGS__)
#define FatalLog(Channel, msg, ...) LOG_INTERNAL_(Channel, spdlog::level::critical, msg, ##__VA_ARGS__)

