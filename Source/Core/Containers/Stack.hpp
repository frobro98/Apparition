// Copyright 2020, Nathan Blane

#pragma once

#include "Containers/DynamicArray.hpp"
#include "CoreAPI.hpp"

template <typename Type>
class CORE_TEMPLATE Stack
{
public:
	using ValueType = Type;
	using ReferenceType = Type&;
	using PointerType = Type*;

public:
	Stack() = default;
	Stack(u32 startStack);

	template<typename PushType>
	void Push(PushType&& item);
	void Pop();
	NODISCARD Type& Peek();

	NODISCARD u32 Size() const;
	NODISCARD bool IsEmpty() const;

private:
	DynamicArray<ValueType> stackSpace;
};

template<typename Type>
inline Stack<Type>::Stack(u32 startStack)
{
	stackSpace.Reserve(startStack);
}

template<typename Type>
template<typename PushType>
inline void Stack<Type>::Push(PushType&& item)
{
	stackSpace.Add(FORWARD(PushType, item));
}

template<typename Type>
inline void Stack<Type>::Pop()
{
	stackSpace.RemoveLast();
}

template<typename Type>
inline Type& Stack<Type>::Peek()
{
	return stackSpace.Last();
}

template<typename Type>
inline u32 Stack<Type>::Size() const
{
	return stackSpace.Size();
}

template<typename Type>
inline bool Stack<Type>::IsEmpty() const
{
	return stackSpace.IsEmpty();
}
