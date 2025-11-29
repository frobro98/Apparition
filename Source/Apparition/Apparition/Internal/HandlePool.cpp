
#include "HandlePool.h"

HandlePool CreateHandlePool(u32 initialSize)
{
    HandlePool newHandlePool;

    // Initialize both arrays
    // We want to ensure that 0 is not used. If we keep track of it within 
    // the handle pool, it simplifies behavior
    const u32 realSize = initialSize + 1;
    newHandlePool.handleIndexGenerations.Resize(realSize);
    // We are only considering 0 within handle indicies. This does not affect the 
    // pool of free handle indices
    newHandlePool.freeHandleIndices.Resize(initialSize);

    // Fill free handle stack
    // 
    // If the stack is expanded, we want the stack location to stay the same while
    // growing the stack capacity. If we start the stack at the front of the array, 
    // we're good to expand capacity without screwing with the front of the stack
    for (u32 i = 1; i < realSize; ++i)
    {
        // Adjust for i not considering 0 a valid handle index
        newHandlePool.freeHandleIndices[i-1] = i;
    }

    return newHandlePool;
}

static void ValidateFreePoolAgainstFreedIndex(const HandlePool& handlePool, u32 freedHandleIndex)
{
    // If the stack is full of handles, we should not be calling this function at all
    Assert(handlePool.stackTop > 0);
    for (u32 i = handlePool.freeHandleIndices.Size() - 1; i > handlePool.stackTop; --i)
    {
        Assert(handlePool.freeHandleIndices[i] != freedHandleIndex);
    }
}

void PushFreedHandleIndex(HandlePool& handlePool, u32 freedHandleIndex)
{
    ValidateFreePoolAgainstFreedIndex(handlePool, freedHandleIndex);
    // stackTop is already pointing to a valid handle, so we want to ensure it's pointing to an invalid handle index
    handlePool.freeHandleIndices[--handlePool.stackTop] = freedHandleIndex;
    Assert(handlePool.stackTop < handlePool.freeHandleIndices.Size());
}

u32 PopFreeHandleIndex(HandlePool& handlePool)
{
    if (handlePool.stackTop < handlePool.freeHandleIndices.Size())
    {
        // Increment the top after we get access to the free index
        u32 handleIndex = handlePool.freeHandleIndices[handlePool.stackTop++];
        Assert(handleIndex != InvalidHandleIndex);
        Assert(handleIndex < handlePool.handleIndexGenerations.Size());
        return handleIndex;
    }
    
    return InvalidHandleIndex;
}

u32 TakeHandleIndex(HandlePool& handlePool, u32 handleIndex)
{
    Assert(!handlePool.handleIndexGenerations.IsEmpty());
    Assert(handleIndex != InvalidHandleIndex);
    return ++handlePool.handleIndexGenerations[handleIndex];
}

void ResizeHandlePool(HandlePool& pool, u32 growSize)
{
    Assert(pool.isResizeable);

    const u32 stackBottom = pool.freeHandleIndices.Size();
    const u32 newFreeHandleArrSize = stackBottom + growSize;
    pool.freeHandleIndices.Resize(newFreeHandleArrSize);
    // Initialize the newly allocated indicies
    for (u32 i = stackBottom + 1; i < pool.freeHandleIndices.Size() + 1; ++i)
    {
        pool.freeHandleIndices[i - 1] = i;
    }
}
