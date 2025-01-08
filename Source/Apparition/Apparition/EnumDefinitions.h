#pragma once

#include "BasicTypes/Intrinsics.hpp"

enum class AllocationScope : u32
{
	Command, // Command allocation
	Object,  // Specific object allocation
	Cache,   // PipelineCache allocation
	Device,  // Device allocation
	Instance // Instance allocation
};

enum class ValidationSeverity : u32
{
	None,
	Verbose,
	Info,
	Warning,
	Error
};

enum DebugMessageType
{
	General = 0x00000001,
	Validation = 0x00000002,
	Performance = 0x00000004,
};
