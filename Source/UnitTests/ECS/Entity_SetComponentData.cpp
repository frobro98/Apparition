// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"

#include "ECS/World.hpp"
#include "ECS/Archetype.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"

using namespace Musa;

TEST(SetOneEntityComponents, EntitySetComponentData)
{
	World w;

	Entity e = w.CreateEntity<Position, Rotation>();
	CHECK_TRUE(w.IsEntityValid(e));
	CHECK_EQ(w.archetypes.Size(), 1);

	Archetype* a = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(a->types.Size(), 2);
	CHECK_EQ(a->chunks.Size(), 1);

	ChunkArray<Position> posArr = GetChunkArray<Position>(a->chunks[0]);
	ChunkArray<Rotation> rotArr = GetChunkArray<Rotation>(a->chunks[0]);

	CHECK_TRUE(posArr.IsValid());
	CHECK_EQ(posArr.size, 1);
	CHECK_TRUE(rotArr.IsValid());
	CHECK_EQ(rotArr.size, 1);

	CHECK_TRUE(posArr[0].position.IsZero());
	CHECK_TRUE(rotArr[0].rotation.IsIdentity());

	Vector4 testVec = Vector4(1, 2, 3, 4);
	Quat testQuat = Quat(5, 6, 7, 8);

	w.SetComponentDataOn(e, Position{ {}, testVec });

	CHECK_FALSE(posArr[0].position.IsZero());
	CHECK_TRUE(rotArr[0].rotation.IsIdentity());

	CHECK_EQ(posArr[0].position, testVec);

	w.SetComponentDataOn(e, Rotation{ {}, testQuat } );

	CHECK_FALSE(posArr[0].position.IsZero());
	CHECK_FALSE(rotArr[0].rotation.IsIdentity());

	CHECK_EQ(posArr[0].position, testVec);
	CHECK_EQ(rotArr[0].rotation, testQuat);
}

TEST(SetTwoEntityComponents, EntitySetComponentData)
{
	World w;

	Entity e0 = w.CreateEntity<Position, Rotation>();
	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_EQ(w.archetypes.Size(), 1);

	Entity e1 = w.CreateEntity<Position, Rotation>();
	CHECK_TRUE(w.IsEntityValid(e1));
	CHECK_EQ(w.archetypes.Size(), 1);

	// Verify World state
	CHECK_ZERO(w.deadIndices.Size());
	CHECK_EQ(w.entityBridges.Size(), 2);


	Archetype* a = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(a->types.Size(), 2);
	CHECK_EQ(a->chunks.Size(), 1);

	ChunkArray<Position> posArr = GetChunkArray<Position>(a->chunks[0]);
	ChunkArray<Rotation> rotArr = GetChunkArray<Rotation>(a->chunks[0]);

	CHECK_TRUE(posArr.IsValid());
	CHECK_EQ(posArr.size, 2);
	CHECK_TRUE(rotArr.IsValid());
	CHECK_EQ(rotArr.size, 2);

	for (const auto& posElem : posArr)
	{
		CHECK_TRUE(posElem.position.IsZero());
	}
	for (const auto& rotElem : rotArr)
	{
		CHECK_TRUE(rotElem.rotation.IsIdentity());
	}

	Vector4 testVec = Vector4(1, 2, 3, 4);
	Quat testQuat = Quat(5, 6, 7, 8);

	w.SetComponentDataOn(e0, Position{ {}, testVec });

	CHECK_FALSE(posArr[0].position.IsZero());
	CHECK_TRUE(rotArr[0].rotation.IsIdentity());

	CHECK_EQ(posArr[0].position, testVec);

	w.SetComponentDataOn(e1, Rotation{ {}, testQuat });

	for (u32 i = 0; i < posArr.size; ++i)
	{
		if (i == 0)
		{
			CHECK_FALSE(posArr[i].position.IsZero());
			CHECK_TRUE(rotArr[i].rotation.IsIdentity());
			CHECK_EQ(posArr[i].position, testVec);
		}
		else
		{
			CHECK_TRUE(posArr[i].position.IsZero());
			CHECK_FALSE(rotArr[i].rotation.IsIdentity());
			CHECK_EQ(rotArr[i].rotation, testQuat);
		}
	}
}

TEST(Set50EntityComponentsEveryOther, EntitySetComponentData)
{
	World w;

	constexpr u32 EntityCount = 50;

	for (u32 i = 0; i < EntityCount; ++i)
	{
		Entity e = w.CreateEntity<Position, Rotation>();
		CHECK_TRUE(w.IsEntityValid(e));
		CHECK_EQ(w.entityBridges.Size(), i + 1);
		CHECK_EQ(w.archetypes.Size(), 1);

		Archetype* a = GetOrCreateArchetypeFrom<Rotation, Position>(w);
		CHECK_EQ(w.archetypes.Size(), 1);
		CHECK_PTR(a);
		CHECK_EQ(a->chunks.Size(), 1);
		CHECK_EQ(a->types.Size(), 2);
	}

	CHECK_EQ(w.entityBridges.Size(), EntityCount);

	auto IsEven = [](u32 num) {
		return num % 2 == 0;
	};

	for (u32 i = 0; i < EntityCount; ++i)
	{
		Entity e{ i, 1 };
		CHECK_TRUE(w.IsEntityValid(e));

		if (IsEven(i))
		{
			w.SetComponentDataOn(e, Position{ {}, Vector4(50.f, 25.f, 12.5, 1.f + i) });
		}
		else
		{
			w.SetComponentDataOn(e, Rotation{ {},  Quat(60.f, 30.f, 15.f, 1.f + i) });
		}
	}

	Archetype* a = GetOrCreateArchetypeFrom<Position, Rotation>(w);
	CHECK_EQ(w.archetypes.Size(), 1);
	ChunkArray<Position> posArr = GetChunkArray<Position>(a->chunks[0]);
	CHECK_EQ(posArr.size, EntityCount);
	ChunkArray<Rotation> rotArr = GetChunkArray<Rotation>(a->chunks[0]);
	CHECK_EQ(rotArr.size, EntityCount);

	for (u32 i = 0; i < EntityCount; ++i)
	{
		if (IsEven(i))
		{
			Position p = { {}, Vector4(50.f, 25.f, 12.5f, 1.f + i) };
			CHECK_EQ(posArr[i].position, p.position);
			CHECK_TRUE(rotArr[i].rotation.IsIdentity());
		}
		else
		{
			Rotation r = { {}, Quat(60.f, 30.f, 15.f, 1.f + i) };
			CHECK_EQ(rotArr[i].rotation, r.rotation);
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}
}

TEST(GetAndSetOneEntityComponents, EntitySetComponentData)
{
	World w;

	Entity e = w.CreateEntity<Position, Rotation>();
	CHECK_TRUE(w.IsEntityValid(e));
	CHECK_EQ(w.archetypes.Size(), 1);

	Archetype* a = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(a->types.Size(), 2);
	CHECK_EQ(a->chunks.Size(), 1);

	ChunkArray<Position> posArr = GetChunkArray<Position>(a->chunks[0]);
	ChunkArray<Rotation> rotArr = GetChunkArray<Rotation>(a->chunks[0]);

	CHECK_TRUE(posArr.IsValid());
	CHECK_EQ(posArr.size, 1);
	CHECK_TRUE(rotArr.IsValid());
	CHECK_EQ(rotArr.size, 1);

	CHECK_TRUE(posArr[0].position.IsZero());
	CHECK_TRUE(rotArr[0].rotation.IsIdentity());

	Vector4 testVec = Vector4(1, 2, 3, 4);
	Quat testQuat = Quat(5, 6, 7, 8);

	Position& p = w.GetComponentDataOn<Position>(e);
	p.position = testVec;

	CHECK_FALSE(posArr[0].position.IsZero());
	CHECK_TRUE(rotArr[0].rotation.IsIdentity());

	CHECK_EQ(posArr[0].position, testVec);

	Rotation& r = w.GetComponentDataOn<Rotation>(e);
	r.rotation = testQuat;

	CHECK_FALSE(posArr[0].position.IsZero());
	CHECK_FALSE(rotArr[0].rotation.IsIdentity());

	CHECK_EQ(posArr[0].position, testVec);
	CHECK_EQ(rotArr[0].rotation, testQuat);
}

TEST(GetAndSetTwoEntityComponents, EntitySetComponentData)
{
	World w;

	Entity e0 = w.CreateEntity<Position, Rotation>();
	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_EQ(w.archetypes.Size(), 1);

	Entity e1 = w.CreateEntity<Position, Rotation>();
	CHECK_TRUE(w.IsEntityValid(e1));
	CHECK_EQ(w.archetypes.Size(), 1);

	// Verify World state
	CHECK_ZERO(w.deadIndices.Size());
	CHECK_EQ(w.entityBridges.Size(), 2);


	Archetype* a = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(a->types.Size(), 2);
	CHECK_EQ(a->chunks.Size(), 1);

	ChunkArray<Position> posArr = GetChunkArray<Position>(a->chunks[0]);
	ChunkArray<Rotation> rotArr = GetChunkArray<Rotation>(a->chunks[0]);

	CHECK_TRUE(posArr.IsValid());
	CHECK_EQ(posArr.size, 2);
	CHECK_TRUE(rotArr.IsValid());
	CHECK_EQ(rotArr.size, 2);

	for (const auto& posElem : posArr)
	{
		CHECK_TRUE(posElem.position.IsZero());
	}
	for (const auto& rotElem : rotArr)
	{
		CHECK_TRUE(rotElem.rotation.IsIdentity());
	}

	Vector4 testVec = Vector4(1, 2, 3, 4);
	Quat testQuat = Quat(5, 6, 7, 8);

	Position& p = w.GetComponentDataOn<Position>(e0);
	p.position = testVec;

	CHECK_FALSE(posArr[0].position.IsZero());
	CHECK_TRUE(rotArr[0].rotation.IsIdentity());

	CHECK_EQ(posArr[0].position, testVec);

	Rotation& r = w.GetComponentDataOn<Rotation>(e1);
	r.rotation = testQuat;

	for (u32 i = 0; i < posArr.size; ++i)
	{
		if (i == 0)
		{
			CHECK_FALSE(posArr[i].position.IsZero());
			CHECK_TRUE(rotArr[i].rotation.IsIdentity());
			CHECK_EQ(posArr[i].position, testVec);
		}
		else
		{
			CHECK_TRUE(posArr[i].position.IsZero());
			CHECK_FALSE(rotArr[i].rotation.IsIdentity());
			CHECK_EQ(rotArr[i].rotation, testQuat);
		}
	}
}

TEST(GetAndSet50EntityComponentsEveryOther, EntitySetComponentData)
{
	World w;

	constexpr u32 EntityCount = 50;

	for (u32 i = 0; i < EntityCount; ++i)
	{
		Entity e = w.CreateEntity<Position, Rotation>();
		CHECK_TRUE(w.IsEntityValid(e));
		CHECK_EQ(w.entityBridges.Size(), i + 1);
		CHECK_EQ(w.archetypes.Size(), 1);

		Archetype* a = GetOrCreateArchetypeFrom<Rotation, Position>(w);
		CHECK_EQ(w.archetypes.Size(), 1);
		CHECK_PTR(a);
		CHECK_EQ(a->chunks.Size(), 1);
		CHECK_EQ(a->types.Size(), 2);
	}

	CHECK_EQ(w.entityBridges.Size(), EntityCount);

	auto IsEven = [](u32 num) {
		return num % 2 == 0;
	};

	for (u32 i = 0; i < EntityCount; ++i)
	{
		Entity e{ i, 1 };
		CHECK_TRUE(w.IsEntityValid(e));

		if (IsEven(i))
		{
			Position& p = w.GetComponentDataOn<Position>(e);
			p.position = Vector4(50.f, 25.f, 12.5, 1.f + i);
		}
		else
		{
			Rotation& r = w.GetComponentDataOn<Rotation>(e);
			r.rotation = Quat(60.f, 30.f, 15.f, 1.f + i);
		}
	}

	Archetype* a = GetOrCreateArchetypeFrom<Position, Rotation>(w);
	CHECK_EQ(w.archetypes.Size(), 1);
	ChunkArray<Position> posArr = GetChunkArray<Position>(a->chunks[0]);
	CHECK_EQ(posArr.size, EntityCount);
	ChunkArray<Rotation> rotArr = GetChunkArray<Rotation>(a->chunks[0]);
	CHECK_EQ(rotArr.size, EntityCount);

	for (u32 i = 0; i < EntityCount; ++i)
	{
		if (IsEven(i))
		{
			Position p = { {}, Vector4(50.f, 25.f, 12.5f, 1.f + i) };
			CHECK_EQ(posArr[i].position, p.position);
			CHECK_TRUE(rotArr[i].rotation.IsIdentity());
		}
		else
		{
			Rotation r = { {}, Quat(60.f, 30.f, 15.f, 1.f + i) };
			CHECK_EQ(rotArr[i].rotation, r.rotation);
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}
}
