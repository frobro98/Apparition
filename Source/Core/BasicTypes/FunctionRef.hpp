// Copyright 2020, Nathan Blane

#pragma once

#include <type_traits>

template <typename Sig>
class FunctionRef;


template <typename Ret, typename... Args>
class FunctionRef<Ret(Args...)> final
{
	using Callback = Ret(*)(void*, Args...);
public:
	template <typename Func, typename = std::enable_if_t<
		std::is_invocable_v<Func, Args...> && !std::is_same_v<std::decay_t<Func>, FunctionRef>>
	>
	FunctionRef(Func&& f) noexcept
		: ptr(&f)
	{
		callback = [](void* p, Args... args)
		{
			return (*reinterpret_cast<Func*>(p))(
				FORWARD(Args, args)...);
		};
	}

	Ret operator()(Args... args) const
		noexcept(noexcept(callback(ptr, FORWARD(Args, args)...)))
	{
		return callback(ptr, FORWARD(Args, args)...);
	}

private:
	void* ptr;
	Callback callback;
};
