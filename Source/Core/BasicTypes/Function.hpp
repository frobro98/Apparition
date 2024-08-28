// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "BasicTypes/FunctionTraits.hpp"
#include "Debugging/Assertion.hpp"

#include <memory>

template<typename>
struct force_function_heap_allocation
	: false_type
{
};

template<typename>
class Function;

namespace detail
{
struct manager_storage_type;
struct function_manager;
struct functor_padding
{
protected:
	size_t padding_first;
	size_t padding_second;
};

struct empty_struct
{
};

template<typename T, typename Allocator>
struct is_inplace_allocated
{
	static constexpr bool value
		// so that it fits
		= sizeof(T) <= sizeof(functor_padding)
		// so that it will be aligned
		&& alignment_of<functor_padding>::value % alignment_of<T>::value == 0
		// so that we can offer noexcept move
		&& is_nothrow_move_constructible<T>::value
		// so that the user can override it
		&& !force_function_heap_allocation<T>::value;
};

template<typename T>
T to_functor(T&& func)
{
	return FORWARD(T, func);
}
template<typename Result, typename Class, typename... Arguments>
auto to_functor(Result(Class::* func)(Arguments...)) -> decltype(mem_fn(func))
{
	return mem_fn(func);
}
template<typename Result, typename Class, typename... Arguments>
auto to_functor(Result(Class::* func)(Arguments...) const) -> decltype(mem_fn(func))
{
	return mem_fn(func);
}

template<typename T>
struct functor_type
{
	typedef decltype(to_functor(declval<T>())) type;
};

template<typename T>
bool is_null(const T&)
{
	return false;
}
template<typename Result, typename... Arguments>
bool is_null(Result(* const& function_pointer)(Arguments...))
{
	return function_pointer == nullptr;
}
template<typename Result, typename Class, typename... Arguments>
bool is_null(Result(Class::* const& function_pointer)(Arguments...))
{
	return function_pointer == nullptr;
}
template<typename Result, typename Class, typename... Arguments>
bool is_null(Result(Class::* const& function_pointer)(Arguments...) const)
{
	return function_pointer == nullptr;
}

template<typename, typename>
struct is_valid_function_argument
{
	static const bool value = false;
};

template<typename Result, typename... Arguments>
struct is_valid_function_argument<Function<Result(Arguments...)>, Result(Arguments...)>
{
	static const bool value = false;
};

template<typename T, typename Result, typename... Arguments>
struct is_valid_function_argument<T, Result(Arguments...)>
{
	template<typename U>
	static decltype(to_functor(declval<U>())(declval<Arguments>()...)) check(U*);
	template<typename>
	static empty_struct check(...);

	static const bool value = is_convertible<decltype(check<T>(nullptr)), Result>::value;
};

typedef const function_manager* manager_type;

struct manager_storage_type
{
	template<typename Allocator>
	Allocator& get_allocator() noexcept
	{
		return reinterpret_cast<Allocator&>(manager);
	}
	template<typename Allocator>
	const Allocator& get_allocator() const noexcept
	{
		return reinterpret_cast<const Allocator&>(manager);
	}

	functor_padding functor;
	manager_type manager;
};

template<typename T, typename Allocator, typename Enable = void>
struct function_manager_inplace_specialization
{
	template<typename Result, typename... Arguments>
	static Result call(const functor_padding& storage, Arguments... arguments)
	{
		// do not call get_functor_ref because I want this function to be fast
		// in debug when nothing gets inlined
		return const_cast<T&>(reinterpret_cast<const T&>(storage))(FORWARD(Arguments, arguments)...);
	}

	static void store_functor(manager_storage_type& storage, T to_store)
	{
		new (&get_functor_ref(storage)) T(FORWARD(T, to_store));
	}
	static void move_functor(manager_storage_type& lhs, manager_storage_type&& rhs) noexcept
	{
		new (&get_functor_ref(lhs)) T(MOVE(get_functor_ref(rhs)));
	}
	static void destroy_functor(Allocator&, manager_storage_type& storage) noexcept
	{
		get_functor_ref(storage).~T();
	}
	static T& get_functor_ref(const manager_storage_type& storage) noexcept
	{
		return const_cast<T&>(reinterpret_cast<const T&>(storage.functor));
	}
};
template<typename T, typename Allocator>
struct function_manager_inplace_specialization<T, Allocator, typename enable_if<!is_inplace_allocated<T, Allocator>::value>::type>
{
	template<typename Result, typename... Arguments>
	static Result call(const functor_padding& storage, Arguments... arguments)
	{
		// do not call get_functor_ptr_ref because I want this function to be fast
		// in debug when nothing gets inlined
		return (*reinterpret_cast<const typename std::allocator_traits<Allocator>::pointer&>(storage))(FORWARD(Arguments, arguments)...);
	}

	static void store_functor(manager_storage_type& self, T to_store)
	{
		Allocator& allocator = self.get_allocator<Allocator>();;
		static_assert(sizeof(typename std::allocator_traits<Allocator>::pointer) <= sizeof(self.functor), "The allocator's pointer type is too big");
		typename std::allocator_traits<Allocator>::pointer* ptr = new (&get_functor_ptr_ref(self)) typename std::allocator_traits<Allocator>::pointer(std::allocator_traits<Allocator>::allocate(allocator, 1));
		std::allocator_traits<Allocator>::construct(allocator, *ptr, FORWARD(T, to_store));
	}
	static void move_functor(manager_storage_type& lhs, manager_storage_type&& rhs) noexcept
	{
		static_assert(std::is_nothrow_move_constructible<typename std::allocator_traits<Allocator>::pointer>::value, "we can't offer a noexcept swap if the pointer type is not nothrow move constructible");
		new (&get_functor_ptr_ref(lhs)) typename std::allocator_traits<Allocator>::pointer(MOVE(get_functor_ptr_ref(rhs)));
		// this next assignment makes the destroy function easier
		get_functor_ptr_ref(rhs) = nullptr;
	}
	static void destroy_functor(Allocator& allocator, manager_storage_type& storage) noexcept
	{
		typename std::allocator_traits<Allocator>::pointer& pointer = get_functor_ptr_ref(storage);
		if (!pointer) return;
		std::allocator_traits<Allocator>::destroy(allocator, pointer);
		std::allocator_traits<Allocator>::deallocate(allocator, pointer, 1);
	}
	static T& get_functor_ref(const manager_storage_type& storage) noexcept
	{
		return *get_functor_ptr_ref(storage);
	}
	static typename std::allocator_traits<Allocator>::pointer& get_functor_ptr_ref(manager_storage_type& storage) noexcept
	{
		return reinterpret_cast<typename std::allocator_traits<Allocator>::pointer&>(storage.functor);
	}
	static const typename std::allocator_traits<Allocator>::pointer& get_functor_ptr_ref(const manager_storage_type& storage) noexcept
	{
		return reinterpret_cast<const typename std::allocator_traits<Allocator>::pointer&>(storage.functor);
	}
};

template<typename T, typename Allocator>
static const function_manager& get_default_manager();

template<typename T, typename Allocator>
static void create_manager(manager_storage_type& storage, Allocator&& allocator)
{
	new (&storage.get_allocator<Allocator>()) Allocator(MOVE(allocator));
	storage.manager = &get_default_manager<T, Allocator>();
}

// this struct acts as a vtable. it is an optimization to prevent
// code-bloat from rtti. see the documentation of boost::function
struct function_manager
{
	template<typename T, typename Allocator>
	inline static constexpr function_manager create_default_manager()
	{

		return function_manager
		{
			&templated_call_move_and_destroy<T, Allocator>,
			&templated_call_copy<T, Allocator>,
			&templated_call_copy_functor_only<T, Allocator>,
			&templated_call_destroy<T, Allocator>
		};
	}

	void (* const call_move_and_destroy)(manager_storage_type& lhs, manager_storage_type&& rhs);
	void (* const call_copy)(manager_storage_type& lhs, const manager_storage_type& rhs);
	void (* const call_copy_functor_only)(manager_storage_type& lhs, const manager_storage_type& rhs);
	void (* const call_destroy)(manager_storage_type& manager);

	template<typename T, typename Allocator>
	static void templated_call_move_and_destroy(manager_storage_type& lhs, manager_storage_type&& rhs)
	{
		typedef function_manager_inplace_specialization<T, Allocator> specialization;
		specialization::move_functor(lhs, MOVE(rhs));
		specialization::destroy_functor(rhs.get_allocator<Allocator>(), rhs);
		create_manager<T, Allocator>(lhs, MOVE(rhs.get_allocator<Allocator>()));
		rhs.get_allocator<Allocator>().~Allocator();
	}
	template<typename T, typename Allocator>
	static void templated_call_copy(manager_storage_type& lhs, const manager_storage_type& rhs)
	{
		typedef function_manager_inplace_specialization<T, Allocator> specialization;
		create_manager<T, Allocator>(lhs, Allocator(rhs.get_allocator<Allocator>()));
		specialization::store_functor(lhs, specialization::get_functor_ref(rhs));
	}
	template<typename T, typename Allocator>
	static void templated_call_destroy(manager_storage_type& self)
	{
		typedef function_manager_inplace_specialization<T, Allocator> specialization;
		specialization::destroy_functor(self.get_allocator<Allocator>(), self);
		self.get_allocator<Allocator>().~Allocator();
	}
	template<typename T, typename Allocator>
	static void templated_call_copy_functor_only(manager_storage_type& lhs, const manager_storage_type& rhs)
	{
		typedef function_manager_inplace_specialization<T, Allocator> specialization;
		specialization::store_functor(lhs, specialization::get_functor_ref(rhs));
	}
};
template<typename T, typename Allocator>
inline static const function_manager& get_default_manager()
{
	static constexpr function_manager default_manager = function_manager::create_default_manager<T, Allocator>();
	return default_manager;
}

template<typename Result, typename...>
struct typedeffer
{
	typedef Result result_type;
};
template<typename Result, typename Argument>
struct typedeffer<Result, Argument>
{
	typedef Result result_type;
	typedef Argument argument_type;
};
template<typename Result, typename First_Argument, typename Second_Argument>
struct typedeffer<Result, First_Argument, Second_Argument>
{
	typedef Result result_type;
	typedef First_Argument first_argument_type;
	typedef Second_Argument second_argument_type;
};
}

template<typename Result, typename... Arguments>
class Function<Result(Arguments...)>
	: public detail::typedeffer<Result, Arguments...>
{
public:
	Function() noexcept
	{
		initialize_empty();
	}
	Function(nullptr_t) noexcept
	{
		initialize_empty();
	}
	Function(Function&& other) noexcept
	{
		initialize_empty();
		Swap(other);
	}
	Function(const Function& other)
		: call(other.call)
	{
		other.manager_storage.manager->call_copy(manager_storage, other.manager_storage);
	}
	template<typename T>
	Function(T functor,
		typename enable_if<detail::is_valid_function_argument<T, Result(Arguments...)>::value, detail::empty_struct>::type = detail::empty_struct()) noexcept(detail::is_inplace_allocated<T, std::allocator<typename detail::functor_type<T>::type>>::value)
	{
		if (detail::is_null(functor))
		{
			initialize_empty();
		}
		else
		{
			typedef typename detail::functor_type<T>::type functor_type;
			initialize(detail::to_functor(FORWARD(T, functor)), std::allocator<functor_type>());
		}
	}
	template<typename Allocator>
	Function(std::allocator_arg_t, const Allocator&)
	{
		// ignore the allocator because I don't allocate
		initialize_empty();
	}
	template<typename Allocator>
	Function(std::allocator_arg_t, const Allocator&, nullptr_t)
	{
		// ignore the allocator because I don't allocate
		initialize_empty();
	}
	template<typename Allocator, typename T>
	Function(std::allocator_arg_t, const Allocator& allocator, T functor,
		typename enable_if<detail::is_valid_function_argument<T, Result(Arguments...)>::value, detail::empty_struct>::type = detail::empty_struct())
		noexcept(detail::is_inplace_allocated<T, Allocator>::value)
	{
		if (detail::is_null(functor))
		{
			initialize_empty();
		}
		else
		{
			initialize(detail::to_functor(FORWARD(T, functor)), Allocator(allocator));
		}
	}
	template<typename Allocator>
	Function(std::allocator_arg_t, const Allocator& allocator, const Function& other)
		: call(other.call)
	{
		typedef typename std::allocator_traits<Allocator>::template rebind_alloc<Function> MyAllocator;

		// first try to see if the allocator matches the target type
		detail::manager_type manager_for_allocator = &detail::get_default_manager<typename std::allocator_traits<Allocator>::value_type, Allocator>();
		if (other.manager_storage.manager == manager_for_allocator)
		{
			detail::create_manager<typename std::allocator_traits<Allocator>::value_type, Allocator>(manager_storage, Allocator(allocator));
			manager_for_allocator->call_copy_functor_only(manager_storage, other.manager_storage);
		}
		// if it does not, try to see if the target contains my type. this
		// breaks the recursion of the last case. otherwise repeated copies
		// would allocate more and more memory
		else
		{
			detail::manager_type manager_for_function = &detail::get_default_manager<Function, MyAllocator>();
			if (other.manager_storage.manager == manager_for_function)
			{
				detail::create_manager<Function, MyAllocator>(manager_storage, MyAllocator(allocator));
				manager_for_function->call_copy_functor_only(manager_storage, other.manager_storage);
			}
			else
			{
				// else store the other function as my target
				initialize(other, MyAllocator(allocator));
			}
		}
	}
	template<typename Allocator>
	Function(std::allocator_arg_t, const Allocator&, Function&& other) noexcept
	{
		// ignore the allocator because I don't allocate
		initialize_empty();
		swap(other);
	}

	Function& operator=(Function other) noexcept
	{
		Swap(other);
		return *this;
	}
	~Function() noexcept
	{
		manager_storage.manager->call_destroy(manager_storage);
	}

	Result operator()(Arguments... arguments) const
	{
		return call(manager_storage.functor, FORWARD(Arguments, arguments)...);
	}

	template<typename T, typename Allocator>
	void Assign(T&& functor, const Allocator& allocator) noexcept(detail::is_inplace_allocated<T, Allocator>::value)
	{
		function(std::allocator_arg, allocator, functor).swap(*this);
	}

	void Swap(Function& other) noexcept
	{
		detail::manager_storage_type temp_storage;
		other.manager_storage.manager->call_move_and_destroy(temp_storage, MOVE(other.manager_storage));
		manager_storage.manager->call_move_and_destroy(other.manager_storage, MOVE(manager_storage));
		temp_storage.manager->call_move_and_destroy(manager_storage, MOVE(temp_storage));

		// TODO - Replace with internal implementation
		std::swap(call, other.call);
	}

	operator bool() const noexcept
	{
		return call != nullptr;
	}

private:
	detail::manager_storage_type manager_storage;
	Result(*call)(const detail::functor_padding&, Arguments...);

	template<typename T, typename Allocator>
	void initialize(T functor, Allocator&& allocator)
	{
		call = &detail::function_manager_inplace_specialization<T, Allocator>::template call<Result, Arguments...>;
		detail::create_manager<T, Allocator>(manager_storage, FORWARD(Allocator, allocator));
		detail::function_manager_inplace_specialization<T, Allocator>::store_functor(manager_storage, FORWARD(T, functor));
	}

	typedef Result(*Empty_Function_Type)(Arguments...);
	void initialize_empty() noexcept
	{
		typedef std::allocator<Empty_Function_Type> Allocator;
		static_assert(detail::is_inplace_allocated<Empty_Function_Type, Allocator>::value, "The empty function should benefit from small functor optimization");

		detail::create_manager<Empty_Function_Type, Allocator>(manager_storage, Allocator());
		detail::function_manager_inplace_specialization<Empty_Function_Type, Allocator>::store_functor(manager_storage, nullptr);
		call = nullptr;
	}
};

template<typename T>
bool operator==(nullptr_t, const Function<T>& rhs) noexcept
{
	return !rhs;
}
template<typename T>
bool operator==(const Function<T>& lhs, nullptr_t) noexcept
{
	return !lhs;
}
template<typename T>
bool operator!=(nullptr_t, const Function<T>& rhs) noexcept
{
	return rhs;
}
template<typename T>
bool operator!=(const Function<T>& lhs, nullptr_t) noexcept
{
	return lhs;
}

template<typename T>
void Swap(Function<T>& lhs, Function<T>& rhs)
{
	lhs.swap(rhs);
}

