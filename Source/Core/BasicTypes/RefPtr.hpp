// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/ConcurrentTypes.hpp"
#include "CoreAPI.hpp"

class CORE_API Counter
{
public:
	void Increment();
	void Decrement();

	inline u32 GetReferenceCount() const { return count; }

private:
	u32 count = 1;
};

class ThreadSafeCounter
{
public:
	ThreadSafeCounter();

	ThreadSafeCounter(const ThreadSafeCounter& other);
	ThreadSafeCounter(ThreadSafeCounter&& other) noexcept;
	ThreadSafeCounter& operator=(const ThreadSafeCounter& other);
	ThreadSafeCounter& operator=(ThreadSafeCounter&& other) noexcept;

	void Increment();
	void Decrement();

	inline u32 GetReferenceCount() const { return count.load(std::memory_order_seq_cst); }

private:
	uatom32 count;
};

template <typename Type, class Count = Counter>
class RefPtr final
{
public:
	RefPtr() = default;
	RefPtr(Type* ptr);
	~RefPtr();
	
	template<typename... Args>
	RefPtr(Args&&... args);

	RefPtr(const RefPtr& other);
	RefPtr(RefPtr&& other);
	RefPtr& operator=(const RefPtr& other);
	RefPtr& operator=(RefPtr&& other);

	Type& operator*();
	Type* operator->();

private:
	void Release();

private:
	Type* data = nullptr;
	Count* counter = nullptr;
};

template<typename Type, class Count>
inline RefPtr<Type, Count>::RefPtr(Type* ptr)
	: data(ptr),
	counter(new Count)
{
}

template<typename Type, class Count>
inline RefPtr<Type, Count>::~RefPtr()
{
	Release();
}

template<typename Type, class Count>
template<typename ...Args>
inline RefPtr<Type, Count>::RefPtr(Args&&... args)
	:data(new Type(FORWARD(Args, args)...)),
	counter(new Count)
{
}

template<typename Type, class Count>
inline RefPtr<Type, Count>::RefPtr(const RefPtr& other)
	: data(other.data),
	counter(other.counter)
{
	counter->Increment();
}

template<typename Type, class Count>
inline RefPtr<Type, Count>::RefPtr(RefPtr&& other)
	: data(other.data),
	counter(other.counter)
{
	other.data = nullptr;
	other.counter = nullptr;
}

template<typename Type, class Count>
inline RefPtr<Type, Count>& RefPtr<Type, Count>::operator=(const RefPtr& other)
{
	if (this != &other)
	{
		data = other.data;
		counter = other.counter;
		counter->Increment();
	}

	return *this;
}

template<typename Type, class Count>
inline RefPtr<Type, Count>& RefPtr<Type, Count>::operator=(RefPtr&& other)
{
	if (this != &other)
	{
		data = other.data;
		counter = other.counter;
		other.data = nullptr;
		other.counter = nullptr;
	}

	return *this;
}

template<typename Type, class Count>
inline Type& RefPtr<Type, Count>::operator*()
{
	return *data;
}

template<typename Type, class Count>
inline Type* RefPtr<Type, Count>::operator->()
{
	return data;
}

template<typename Type, class Count>
inline void RefPtr<Type, Count>::Release()
{
	counter->Decrement();
	if (counter->GetReferenceCount() == 0)
	{
		delete counter;
		delete data;
	}
}
