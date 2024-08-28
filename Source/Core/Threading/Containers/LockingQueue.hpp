#pragma once

#include "BasicTypes/Uncopyable.hpp"
#include "Containers/Queue.h"
#include "Threading/ScopedReadLock.hpp"
#include "Threading/ScopedWriteLock.hpp"

template <class Elem>
class LockingQueue : private Uncopyable
{
public:
	template <typename PushType>
	void Push(PushType&& elem);
	Elem Pop();

	bool IsEmpty();
	u32 Size();

private:
	Queue<Elem> queue;
	ReadWriteLock mut;
};

template<class Elem>
template<typename PushType>
inline void LockingQueue<Elem>::Push(PushType && elem)
{
	ScopedWriteLock lock(mut);
	queue.Push(FORWARD(PushType, elem));
}

template<class Elem>
inline Elem LockingQueue<Elem>::Pop()
{
	ScopedWriteLock lock(mut);
	return queue.Pop();
}

template<class Elem>
inline bool LockingQueue<Elem>::IsEmpty()
{
	ScopedReadLock lock(mut);
	return queue.IsEmpty();
}

template<class Elem>
inline u32 LockingQueue<Elem>::Size()
{
	ScopedReadLock lock(mut);
	return queue.Size();
}

