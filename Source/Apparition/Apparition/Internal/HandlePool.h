#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"

// Resizeable pool that keeps track of available handles
// 
// Note(nblane): Will be sized to an initial default and users will be able to override this during Apparition init.
// Users will also be able to set whether pools are resizeable or not, to allow for set resource amounts during runtime
struct HandlePool
{
    // Dynamic array containing which generation the handle is on. This is purely for validation, since there could be stale handle
    // that thinks we're pointing to another resource, but instead, it's just stale
    DynamicArray<u32> handleIndexGenerations;
    // Dynamic stack of free slots
    DynamicArray<u32> freeHandleIndices;
    u32 stackTop = 0;
    bool isResizeable = true;
};

constexpr inline u32 InvalidHandleIndex = 0;

HandlePool CreateHandlePool(u32 initialSize);
void PushFreedHandleIndex(HandlePool& handlePool, u32 freedHandleIndex);
u32 PopFreeHandleIndex(HandlePool& handlePool);
// Returns the generation that this handle index is on
u32 GetHandleGeneration(HandlePool& handlePool, u32 handleIndex);
// We need to be able to resize
void ResizeHandlePool(HandlePool& pool, u32 growSize);
// Checks if a handle index is valid
bool IsHandleValid(const HandlePool& pool, u32 handleIndex, u32 generation);
