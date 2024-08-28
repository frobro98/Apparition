// Copyright 2020, Nathan Blane

#pragma once

#include <iterator>

#include "BasicTypes/Intrinsics.hpp"
#include "Platform/Platform.hpp"
#include "Logging/LogFunctions.hpp"
#include "Logging/CoreLogChannels.hpp"

namespace Debug
{
	namespace Internal
	{
		template<class T>
		constexpr forceinline bool internalExpressionCheck(const T& exprCheck)
		{
			return (exprCheck) ? true : false;
		}

		template <>
		constexpr forceinline bool internalExpressionCheck(const bool& boolExpr)
		{
			return boolExpr;
		}
	}
	
	template<typename... Args>
	forceinline void AssertionFailed(const tchar* expr, const tchar* file, i32 line, const tchar* desc, NOT_USED Args... args)
	{
		fmt::memory_buffer buffer;
		fmt::vformat_to(std::back_inserter(buffer), std::string_view{ desc }, fmt::make_format_args(args...));

		MUSA_LOG(AssertionLog, LogLevel::Fatal, "{} failed! File {}, Line {}. {}", expr, file, line, buffer.data());
		Platform::DebugBreak();
	}

	template <typename Ref>
	inline bool RefIsNotNull(Ref& ref)
	{
		return &ref != nullptr;
	}

	template <typename Ref, typename... RefArgs>
	inline bool RefIsNotNull(Ref& ref, RefArgs&&... args)
	{
		return &ref != nullptr && RefIsNotNull(args...);
	}
}

#ifdef M_DEBUG
#define AssertStr(x, str)														\
		do																		\
		{																		\
			if(!Debug::Internal::internalExpressionCheck(x))					\
				Debug::AssertionFailed(#x, __FILE__, __LINE__, str);			\
																				\
		}while(false)

#define Assert(x) AssertStr(x, #x)

#define Assertf(x, str, ...)													\
		do																		\
		{																		\
			if(!Debug::Internal::internalExpressionCheck(x))					\
				Debug::AssertionFailed(#x, __FILE__, __LINE__, str, ##__VA_ARGS__);		\
			__assume(x);														\
		} while(false)

#define AssertFunc(func, str)											\
		do																\
		{																\
			if(!(func)())												\
				Debug::AssertionFailed(#func, __FILE__, __LINE__, str);	\
		} while (false)

#define REF_CHECK(...) Assert(Debug::RefIsNotNull(__VA_ARGS__))

#else
#define AssertStr(x, str)
#define Assert(x)
#define Assertf(x, str, ...)
#define AssertFunc(func, str)
#define REF_CHECK(...)
#endif