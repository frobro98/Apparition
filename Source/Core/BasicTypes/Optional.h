// Copyright 2020, Nathan Blane

#pragma once

#include <type_traits>

// TODO - Revisit implementing std::optional within my engine...
// namespace
// {
// 
// 
// 
// }
// 
// template<typename OptionalType>
// class Optional
// {
// 	static_assert(!std::is_reference_v<OptionalType>, "Reference types aren't supported!");
// public:
// 	using value_type = OptionalType;
// 	using reference_type = OptionalType&;
// 	using pointer_type = OptionalType*;
// public:
// 	constexpr Optional() noexcept;
// 
// private:
// 	union
// 	{
// 		char empty;
// 		OptionalType value;
// 	};
// 	bool hasValue;
// };

