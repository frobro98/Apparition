#pragma once

// Taken from https://codereview.stackexchange.com/questions/14730/impossibly-fast-delegate-in-c11


#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include "BasicTypes/Utility.hpp"

template <typename T> 
class Delegate;

template<class Ret, class ...Args>
class Delegate<Ret(Args...)>
{
    using stub_ptr_type = Ret(*)(void*, Args&&...);

    Delegate(void* const o, stub_ptr_type const m) noexcept :
        object_ptr_(o),
        stub_ptr_(m)
    {
    }

public:
    Delegate() = default;

    Delegate(const Delegate&) = default;

    Delegate(Delegate&&) = default;

    Delegate(std::nullptr_t const) noexcept : Delegate() { }

    template <class C, typename =
        typename std::enable_if <std::is_class<C>{}>::type>
    explicit Delegate(const C* const obj) noexcept :
        object_ptr_(const_cast<C*>(obj))
    {
    }

    template <class C, typename =
        typename std::enable_if<std::is_class<C>{}>::type>
    explicit Delegate(const C& obj) noexcept :
        object_ptr_(const_cast<C*>(&obj))
    {
    }

    template <class C>
    Delegate(C* const objectPtr, Ret(C::* const methodPtr)(Args...))
    {
        *this = from(objectPtr, methodPtr);
    }

    template <class C>
    Delegate(C* const objectPtr, const Ret(C::* const methodPtr)(Args...))
    {
        *this = from(objectPtr, methodPtr);
    }

    template <class C>
    Delegate(C& object, Ret(C::* const methodPtr)(Args...))
    {
        *this = from(object, methodPtr);
    }

    template <class C>
    Delegate(const C& object, const Ret(C::* const methodPtr)(Args...))
    {
        *this = from(object, methodPtr);
    }

    template <
        typename Func,
        typename = typename std::enable_if<
        !std::is_same<Delegate, typename std::decay<Func>::type>{}
        >::type
    >
    Delegate(Func&& f) :
        store_(operator new(sizeof(typename std::decay<Func>::type)),
            functor_deleter<typename std::decay<Func>::type>),
        store_size_(sizeof(typename std::decay<Func>::type))
    {
        using functor_type = typename std::decay<Func>::type;

        new(store_.get()) functor_type(std::forward<Func>(f));

        object_ptr_ = store_.get();

        stub_ptr_ = functor_stub<functor_type>;

        deleter_ = deleter_stub<functor_type>;
    }

    Delegate& operator=(Delegate const&) = default;

    Delegate& operator=(Delegate&&) = default;

    template <class C>
    Delegate& operator=(Ret(C::* const rhs)(Args...))
    {
        return *this = from(static_cast<C*>(object_ptr_), rhs);
    }

    template <class C>
    Delegate& operator=(Ret(C::* const rhs)(Args...) const)
    {
        return *this = from(static_cast<C const*>(object_ptr_), rhs);
    }

    template <
        typename Func,
        typename = typename std::enable_if <
        !std::is_same<Delegate, typename std::decay<Func>::type>{}
        >::type
    >
    Delegate& operator=(Func&& f)
    {
        using functor_type = typename std::decay<Func>::type;

        if ((sizeof(functor_type) > store_size_) || !store_.unique())
        {
            store_.reset(operator new(sizeof(functor_type)),
                functor_deleter<functor_type>);

            store_size_ = sizeof(functor_type);
        }
        else
        {
            deleter_(store_.get());
        }

        new (store_.get()) functor_type(FORWARD(Func, f));

        object_ptr_ = store_.get();

        stub_ptr_ = functor_stub<functor_type>;

        deleter_ = deleter_stub<functor_type>;

        return *this;
    }

    template <Ret(* const functionPtr)(Args...)>
    static Delegate from() noexcept
    {
        return { nullptr, function_stub<functionPtr> };
    }

    template <class C, Ret(C::* const methodPtr)(Args...)>
    static Delegate from(C* const object_ptr) noexcept
    {
        return { object_ptr, method_stub<C, methodPtr> };
    }

    template <class C, Ret(C::* const methodPtr)(Args...) const>
    static Delegate from(const C* const object_ptr) noexcept
    {
        return { const_cast<C*>(object_ptr), const_method_stub<C, methodPtr> };
    }

    template <class C, Ret(C::* const methodPtr)(Args...)>
    static Delegate from(C& object) noexcept
    {
        return { &object, method_stub<C, methodPtr> };
    }

    template <class C, Ret(C::* const methodPtr)(Args...) const>
    static Delegate from(const C& object) noexcept
    {
        return { const_cast<C*>(&object), const_method_stub<C, methodPtr> };
    }

    template <typename Func>
    static Delegate from(Func&& f)
    {
        return std::forward<Func>(f);
    }

    static Delegate from(Ret(* const functionPtr)(Args...))
    {
        return functionPtr;
    }

    template <class C>
    using member_pair =
        std::pair<C* const, Ret(C::* const)(Args...)>;

    template <class C>
    using const_member_pair =
        std::pair<const C* const, Ret(C::* const)(Args...) const>;

    template <class C>
    static Delegate from(C* const object_ptr,
        Ret(C::* const methodPtr)(Args...))
    {
        return member_pair<C>(object_ptr, methodPtr);
    }

    template <class C>
    static Delegate from(const C* const object_ptr,
        Ret(C::* const methodPtr)(Args...) const)
    {
        return const_member_pair<C>(object_ptr, methodPtr);
    }

    template <class C>
    static Delegate from(C& object, Ret(C::* const methodPtr)(Args...))
    {
        return member_pair<C>(&object, methodPtr);
    }

    template <class C>
    static Delegate from(const C& object,
        Ret(C::* const methodPtr)(Args...) const)
    {
        return const_member_pair<C>(&object, methodPtr);
    }

    void reset() { stub_ptr_ = nullptr; store_.reset(); }

    void reset_stub() noexcept { stub_ptr_ = nullptr; }

    void swap(Delegate& other) noexcept { ::std::swap(*this, other); }

    bool operator==(Delegate const& rhs) const noexcept
    {
        return (object_ptr_ == rhs.object_ptr_) && (stub_ptr_ == rhs.stub_ptr_);
    }

    bool operator!=(Delegate const& rhs) const noexcept
    {
        return !operator==(rhs);
    }

    bool operator<(Delegate const& rhs) const noexcept
    {
        return (object_ptr_ < rhs.object_ptr_) ||
            ((object_ptr_ == rhs.object_ptr_) && (stub_ptr_ < rhs.stub_ptr_));
    }

    bool operator==(std::nullptr_t const) const noexcept
    {
        return !stub_ptr_;
    }

    bool operator!=(std::nullptr_t const) const noexcept
    {
        return stub_ptr_;
    }

    explicit operator bool() const noexcept { return stub_ptr_; }

    Ret operator()(Args... args) const
    {
        //  assert(stub_ptr);
        return stub_ptr_(object_ptr_, std::forward<Args>(args)...);
    }

private:
    friend struct std::hash<Delegate>;

    using deleter_type = void (*)(void*);

    void* object_ptr_;
    stub_ptr_type stub_ptr_{};

    deleter_type deleter_;

    std::shared_ptr<void> store_;
    std::size_t store_size_;

    template <class T>
    static void functor_deleter(void* const p)
    {
        static_cast<T*>(p)->~T();

        operator delete(p);
    }

    template <class T>
    static void deleter_stub(void* const p)
    {
        static_cast<T*>(p)->~T();
    }

    template <Ret(*function_ptr)(Args...)>
    static Ret function_stub(void* const, Args&&... args)
    {
        return function_ptr(::std::forward<Args>(args)...);
    }

    template <class C, Ret(C::* method_ptr)(Args...)>
    static Ret method_stub(void* const object_ptr, Args&&... args)
    {
        return (static_cast<C*>(object_ptr)->*method_ptr)(
            ::std::forward<Args>(args)...);
    }

    template <class C, Ret(C::* method_ptr)(Args...) const>
    static Ret const_method_stub(void* const object_ptr, Args&&... args)
    {
        return (static_cast<C const*>(object_ptr)->*method_ptr)(
            ::std::forward<Args>(args)...);
    }

    template <typename>
    struct is_member_pair : std::false_type { };

    template <class C>
    struct is_member_pair< ::std::pair<C* const,
        Ret(C::* const)(Args...)> > : std::true_type
    {
    };

    template <typename>
    struct is_const_member_pair : std::false_type { };

    template <class C>
    struct is_const_member_pair< ::std::pair<C const* const,
        Ret(C::* const)(Args...) const> > : std::true_type
    {
    };

    template <typename T>
    static typename ::std::enable_if <
        !(is_member_pair<T>{} ||
            is_const_member_pair<T>{}),
        Ret
    > ::type
        functor_stub(void* const object_ptr, Args&&... args)
    {
        return (*static_cast<T*>(object_ptr))(::std::forward<Args>(args)...);
    }

    template <typename T>
    static typename ::std::enable_if <
        is_member_pair<T>::value ||
        is_const_member_pair<T>::value,
        Ret
    > ::type
        functor_stub(void* const object_ptr, Args&&... args)
    {
        return (static_cast<T*>(object_ptr)->first->*
            static_cast<T*>(object_ptr)->second)(::std::forward<Args>(args)...);
    }
};

namespace std
{
template <typename Ret, typename ...Args>
struct hash<::Delegate<Ret(Args...)>>
{
    size_t operator()(::Delegate<Ret(Args...)> const& d) const noexcept
    {
        auto const seed(hash<void*>()(d.object_ptr_));

        return hash<typename ::Delegate<Ret(Args...)>::stub_ptr_type>()(
            d.stub_ptr_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
};
}


