// Copyright 2020, Nathan Blane

#pragma once

#include "Logging/LogCore.hpp"

#define MUSA_LOG(Channel, level, msg, ...) GetLogger().Log(Channel, level, msg, ##__VA_ARGS__)

#define MUSA_DEBUG(Channel, msg, ...) MUSA_LOG(Channel, LogLevel::Debug, msg, ##__VA_ARGS__)
#define MUSA_INFO(Channel, msg, ...) MUSA_LOG(Channel, LogLevel::Info, msg, ##__VA_ARGS__)
#define MUSA_WARN(Channel, msg, ...) MUSA_LOG(Channel, LogLevel::Warning, msg, ##__VA_ARGS__)
#define MUSA_ERR(Channel, msg, ...) MUSA_LOG(Channel, LogLevel::Error, msg, ##__VA_ARGS__)
#define MUSA_FATAL(Channel, msg, ...) MUSA_LOG(Channel, LogLevel::Fatal, msg, ##__VA_ARGS__)

