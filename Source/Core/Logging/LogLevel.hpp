#pragma once

namespace LogLevel
{
	enum Type
	{
		Debug,
		Info,
		Warning,
		Error,
		Fatal,

		Max
	};
};
static_assert(LogLevel::Max == LogLevel::Fatal + 1);


forceinline const tchar* ToString(LogLevel::Type level)
{
	// TODO - Implement enum strings with macro tricks?
	switch (level)
	{
		case LogLevel::Debug: return "DBG";
		case LogLevel::Info: return "INF";
		case LogLevel::Warning: return "WRN";
		case LogLevel::Error: return "ERR";
		case LogLevel::Fatal: return "FATAL";
		default:
			return nullptr;
	}
}
