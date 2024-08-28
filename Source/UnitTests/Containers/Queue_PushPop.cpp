// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "Containers/Queue.h"

TEST(PushOnePopOne, QueuePushPop)
{
	Queue<u32> numberQueue;

	CHECK_ZERO(numberQueue.Size());

	numberQueue.Push(1);
	CHECK_EQ(numberQueue.Size(), 1);

	CHECK_EQ(numberQueue.Peek(), 1);
	numberQueue.Pop();

	CHECK_ZERO(numberQueue.Size());
}

TEST(PushThreePopThree, QueuePushPop)
{
	Queue<u32> numberQueue;

	CHECK_ZERO(numberQueue.Size());

	numberQueue.Push(1);
	CHECK_EQ(numberQueue.Size(), 1);

	numberQueue.Push(2);
	CHECK_EQ(numberQueue.Size(), 2);

	numberQueue.Push(3);
	CHECK_EQ(numberQueue.Size(), 3);

	CHECK_EQ(numberQueue.Peek(), 1);
	numberQueue.Pop();
	CHECK_EQ(numberQueue.Size(), 2);

	CHECK_EQ(numberQueue.Peek(), 2);
	numberQueue.Pop();
	CHECK_EQ(numberQueue.Size(), 1);

	CHECK_EQ(numberQueue.Peek(), 3);
	numberQueue.Pop();
	CHECK_TRUE(numberQueue.IsEmpty());

	CHECK_ZERO(numberQueue.Size());
}

TEST(Push200Pop100, QueuePushPop)
{
	Queue<u32> numberQueue;
	CHECK_ZERO(numberQueue.Size());

	for (u32 i = 0; i < 200; ++i)
	{
		numberQueue.Push(i + 1);
	}

	for (u32 i = 0; i < 100; ++i)
	{
		CHECK_EQ(numberQueue.Peek(), i + 1);
		numberQueue.Pop();
	}

	CHECK_EQ(numberQueue.Size(), 100);
}

TEST(Push500Pop250, QueuePushPop)
{
	Queue<u32> numberQueue;
	CHECK_ZERO(numberQueue.Size());

	for (u32 i = 0; i < 500; ++i)
	{
		numberQueue.Push(i + 1);
	}

	for (u32 i = 0; i < 250; ++i)
	{
		CHECK_EQ(numberQueue.Peek(), i + 1);
		numberQueue.Pop();
	}

	CHECK_EQ(numberQueue.Size(), 250);
}
