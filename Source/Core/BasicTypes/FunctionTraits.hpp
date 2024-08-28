#pragma once

#include "CoreFlags.hpp"
#include "BasicTypes/Utility.hpp"
#include "Utilities/MacroHelpers.hpp"

#define EMIT_CDECL(func, param0, param1, param2) \
	func(__cdecl, param0, param1, param2)

#ifdef _WIN32

#ifdef _M_CEE
#define EMIT_CLRCALL(FUNC, OPT1, OPT2, OPT3) \
	FUNC(__clrcall, OPT1, OPT2, OPT3)

#else // _M_CEE 
#define EMIT_CLRCALL(FUNC, OPT1, OPT2, OPT3)
#endif // _M_CEE 

#if defined(_M_IX86) && !defined(_M_CEE)
#define EMIT_FASTCALL(FUNC, OPT1, OPT2, OPT3) \
	FUNC(__fastcall, OPT1, OPT2, OPT3)

#else // defined(_M_IX86) && !defined(_M_CEE)
#define EMIT_FASTCALL(FUNC, OPT1, OPT2, OPT3)
#endif // defined(_M_IX86) && !defined(_M_CEE)

#ifdef _M_IX86
#define EMIT_STDCALL(FUNC, OPT1, OPT2, OPT3) \
	FUNC(__stdcall, OPT1, OPT2, OPT3)
#define EMIT_THISCALL(FUNC, OPT1, OPT2, OPT3) \
	FUNC(__thiscall, OPT1, OPT2, OPT3)

#else //_M_IX86 
#define EMIT_STDCALL(FUNC, OPT1, OPT2, OPT3)
#define EMIT_THISCALL(FUNC, OPT1, OPT2, OPT3)
#endif //_M_IX86

#if ((defined(_M_IX86) && _M_IX86_FP >= 2) \
	|| defined(_M_X64)) && !defined(_M_CEE)
#define EMIT_VECTORCALL(FUNC, OPT1, OPT2, OPT3) \
	FUNC(__vectorcall, OPT1, OPT2, OPT3)
#else // defined(_M_IX86) && _M_IX86_FP >= 2 etc. 
#define EMIT_VECTORCALL(FUNC, OPT1, OPT2, OPT3)
#endif // defined(_M_IX86) && _M_IX86_FP >= 2 etc. 

#else

#endif // WIN32

#define NON_MEMBER_FUNC(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_CDECL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_CLRCALL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_FASTCALL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_STDCALL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_VECTORCALL(func, cv_opt, ref_opt, noexcept_opt)

#define NON_MEMBER_FUNC_CV(func, ref_opt, noexcept_opt) \
	NON_MEMBER_FUNC(func, , ref_opt, noexcept_opt) \
	NON_MEMBER_FUNC(func, const, ref_opt, noexcept_opt) \
	NON_MEMBER_FUNC(func, volatile, ref_opt, noexcept_opt) \
	NON_MEMBER_FUNC(func, const volatile, ref_opt, noexcept_opt) \

#define NON_MEMBER_FUNC_CV_REF(func, noexcept_opt) \
	NON_MEMBER_FUNC_CV(func, , noexcept_opt) \
	NON_MEMBER_FUNC_CV(func, &, noexcept_opt) \
	NON_MEMBER_FUNC_CV(func, &&, noexcept_opt) \


#define NON_MEMBER_FUNC_CV_REF_NOEXCEPT(func) \
	NON_MEMBER_FUNC_CV_REF(func, ) \
	NON_MEMBER_FUNC_CV_REF(func, noexcept)

#define MEMBER_FUNC(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_CDECL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_CLRCALL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_FASTCALL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_STDCALL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_THISCALL(func, cv_opt, ref_opt, noexcept_opt) \
	EMIT_VECTORCALL(func, cv_opt, ref_opt, noexcept_opt)

#define MEMBER_FUNC_CV(func, ref_opt, noexcept_opt) \
	MEMBER_FUNC(func, , ref_opt, noexcept_opt) \
	MEMBER_FUNC(func, const, ref_opt, noexcept_opt) \
	MEMBER_FUNC(func, volatile, ref_opt, noexcept_opt) \
	MEMBER_FUNC(func, const volatile, ref_opt, noexcept_opt)

#define MEMBER_FUNC_CV_REF(func, noexcept_opt) \
	MEMBER_FUNC_CV(func, , noexcept_opt) \
	MEMBER_FUNC_CV(func, &, noexcept_opt) \
	MEMBER_FUNC_CV(func, &&, noexcept_opt)

#define MEMBER_FUNC_CV_REF_NOEXCEPT(func) \
	MEMBER_FUNC_CV_REF(func, ) \
	MEMBER_FUNC_CV_REF(func, noexcept)

#define CLASS_DEFINE_CV(func) \
	func(EMPTY_ARGUMENT) \
	func(const) \
	func(volatile) \
	func(const volatile)

#define CLASS_DEFINE_CV_REF_NOEXCEPT(func) \
	func(EMPTY_ARGUMENT) \
	func(const) \
	func(volatile) \
	func(const volatile) \
	func(&) \
	func(const &) \
	func(volatile &) \
	func(const volatile &) \
	func(&&) \
	func(const &&) \
	func(volatile &&) \
	func(const volatile &&) \
	func(noexcept) \
	func(const noexcept) \
	func(volatile noexcept) \
	func(const volatile noexcept) \
	func(& noexcept) \
	func(const & noexcept) \
	func(volatile & noexcept) \
	func(const volatile & noexcept) \
	func(&& noexcept) \
	func(const && noexcept) \
	func(volatile && noexcept) \
	func(const volatile && noexcept)

template<typename T>
struct remove_pointer
{
	using type = T;
};

#define REMOVE_POINTER_(cv_opt) \
	template<typename T> \
	struct remove_pointer<T * cv_opt> \
	{	\
		using type = T; \
	};

CLASS_DEFINE_CV(REMOVE_POINTER_)
#undef REMOVE_POINTER_

template<typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

namespace Internal
{
template <typename... T>
struct ArgTypes_
{
};

template <typename T0>
struct ArgTypes_<T0>
{
	using argument_type = T0;
};

template <typename T0, typename T1>
struct ArgTypes_<T0, T1>
{
	using first_argument_type = T0;
	using second_argument_type = T1;
};
} // namespace

/**** is_function definitions ****/
namespace Internal
{
template<typename T>
struct IsFunction_
{
	using BoolType = false_type;
};

#define IS_FUNCTION_(call_opt, cv_opt, ref_opt, noexcept_opt) \
	template<typename Ret, typename... Args> \
	struct IsFunction_<Ret call_opt (Args...) cv_opt ref_opt noexcept_opt> \
		: Internal::ArgTypes_<Args...> \
	{ \
		using BoolType_ = true_type; \
		using result_type = Ret; \
	};

#define IS_FUNCTION_ELLIPSIS_(cv_ref_noexcept_opt) \
	template<typename Ret, typename... Args> \
	struct IsFunction_<Ret (Args..., ...) cv_ref_noexcept_opt> \
	{ \
		using BoolType_ = true_type; \
		using result_type = Ret; \
	};

NON_MEMBER_FUNC_CV_REF_NOEXCEPT(IS_FUNCTION_)
CLASS_DEFINE_CV_REF_NOEXCEPT(IS_FUNCTION_ELLIPSIS_)

#undef IS_FUNCTION_
#undef IS_FUNCTION_ELLIPSIS_
}

template<typename T>
struct is_function 
	: Internal::IsFunction_<T>::BoolType
{
};

template<typename T>
inline constexpr bool is_function_v = is_function<T>::value;

/**** End is_function definitions ****/

/**** is_member_function_pointer definitions ****/
namespace Internal
{
template<typename T>
struct IsMemfuncPtr
{
	using BoolType = false_type;
};

#define IS_MEMFUNCPTR_(call_opt, cv_opt, ref_opt, noexcept_opt) \
	template<typename Ret, class This, typename... Args> \
	struct IsMemfuncPtr<Ret (call_opt This::*)(Args...) cv_opt ref_opt noexcept_opt> \
		: Internal::ArgTypes_<cv_opt This*, Args...> \
	{ \
		using BoolType = true_type; \
		using result_type = Ret; \
		using ClassType_ = This; \
		/*using _Guide_type = enable_if<!is_same_v<int REF_OPT, int&&>, _Ret (_Types...)>;*/ \
	};

#define IS_MEMFUNCPTR_ELLIPSIS_(cv_ref_noexcept_opt) \
	template<typename Ret, class This, typename... Args> \
	struct IsMemfuncPtr<Ret (This::*)(Args..., ...) cv_ref_noexcept_opt> \
	{ \
		using BoolType = true_type; \
		using result_type = Ret; \
		using ClassType_ = This; \
		/*using _Guide_type = enable_if<false>;*/ \
	};

MEMBER_FUNC_CV_REF_NOEXCEPT(IS_MEMFUNCPTR_)
CLASS_DEFINE_CV_REF_NOEXCEPT(IS_MEMFUNCPTR_ELLIPSIS_)

#undef IS_MEMFUNCPTR_
#undef IS_MEMFUNCPTR_ELLIPSIS_

template<typename T, bool pmf = IsMemfuncPtr<T>::BoolType::value>
struct IsMemObjPtr : false_type
{
};

template<typename T0, typename T1>
struct IsMemObjPtr<T0 T1::*, false> : true_type
{
};
} // namespace Internal

template<typename T>
struct is_member_object_pointer
	: Internal::IsMemObjPtr<remove_cv_t<T>>::type
{
};

template<typename T>
inline constexpr bool is_member_object_pointer_v = is_member_object_pointer<T>::value;

template<typename T>
struct is_member_function_pointer
	: Internal::IsMemfuncPtr<remove_cv_t<T>>::BoolType
{
};

template<typename T>
inline constexpr bool is_member_function_pointer_v = is_member_function_pointer<T>::value;

/**** End is_member_function_pointer definitions ****/


/**** mem_fn definition ****/
namespace Internal
{
template<typename T, typename = void>
struct WeakResultType
{
};

template<typename T>
struct WeakResultType<T,
	void_t<typename T::result_type>>
{
	using result_type = typename T::result_type;
};

template<typename T, typename = void>
struct WeakArgType
	: WeakResultType<T>
{
};

template<typename T>
struct WeakArgType<T,
	void_t<typename T::argument_type>>
	: WeakResultType<T>
{
	using argument_type = typename T::argument_type;
};

template<typename T, typename = void>
struct WeakBinaryArgs
	: WeakArgType<T>
{
};

template<typename T>
struct WeakBinaryArgs<T,
	void_t<typename T::first_argument_type, typename T::second_argument_type>>
	: WeakArgType<T>
{
	using first_argument_type = typename T::first_argument_type;
	using second_argument_type = typename T::second_argument_type;
};

template<typename T>
struct WeakTypes_
{
	using IsFOrPf = is_function<remove_pointer_t<T>>;
	using IsPmf = is_member_function_pointer<remove_cv_t<T>>;
	using type = conditional_t<IsFOrPf::value, IsFOrPf,
		conditional_t<IsPmf::value, IsPmf,
		WeakBinaryArgs<T>>>;
};
} // namespace Internal

// Return object that 
template<typename T>
class MemFnStore 
	: public Internal::WeakTypes_<T>::type
{
private:
	T memPtr;

public:
	explicit MemFnStore(T val) noexcept
		: memPtr(val)
	{
	}

	template<typename... Args>
	auto operator()(Args&&... args) const
		noexcept(noexcept(std::invoke(memPtr, FORWARD(Args, args)...)))
		-> decltype(std::invoke(memPtr, FORWARD(Args, args)...))
	{
		return std::invoke(memPtr, FORWARD(Args, args)...);
	}
};

template<typename Ret, typename T>
NODISCARD inline MemFnStore<Ret T::*> mem_fn(Ret T::* memPtr) noexcept
{
	return MemFnStore<Ret T::*>(memPtr);
}

/**** End mem_fn definition ****/

#undef EMIT_CDECL
#undef EMIT_CLRCALL
#undef EMIT_FASTCALL
#undef EMIT_STDCALL
#undef EMIT_THISCALL
#undef EMIT_VECTORCALL
#undef NON_MEMBER_FUNC
#undef NON_MEMBER_FUNC_CV
#undef NON_MEMBER_FUNC_CV_REF
#undef NON_MEMBER_FUNC_CV_REF_NOEXCEPT
#undef MEMBER_FUNC
#undef MEMBER_FUNC_CV
#undef MEMBER_FUNC_CV_REF
#undef MEMBER_FUNC_CV_REF_NOEXCEPT
#undef CLASS_DEFINE_CV
#undef CLASS_DEFINE_CV_REF_NOEXCEPT

