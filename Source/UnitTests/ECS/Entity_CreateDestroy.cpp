// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"

#include "ECS/World.hpp"
#include "ECS/Archetype.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"

using namespace Musa;

TEST(EntityCreate, EntityCreateDestroy)
{
	World w;

	Entity e = w.CreateEntity<Position>();

	CHECK_TRUE(w.IsEntityValid(e));

	CHECK_EQ(e.id, 0);
	CHECK_EQ(e.version, 1);
	CHECK_EQ(w.entityBridges.Size(), 1);
	
	EntityBridge& b = w.entityBridges[0];
	
	CHECK_EQ(b.version, e.version);

	CHECK_PTR(b.chunk.header);
	CHECK_PTR(b.chunk.data);
	CHECK_EQ(b.chunkIndex, 0);

	CHECK_EQ(w.archetypes.Size(), 1);
}

TEST(EntityDestroy, EntityCreateDestroy)
{
	World w;

	Entity e = w.CreateEntity<Position>();

	CHECK_TRUE(w.IsEntityValid(e));

	CHECK_EQ(e.id, 0);
	CHECK_EQ(e.version, 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	EntityBridge& b = w.entityBridges[0];

	CHECK_EQ(b.version, e.version);

	CHECK_PTR(b.chunk.header);
	CHECK_PTR(b.chunk.data);
	CHECK_EQ(b.chunkIndex, 0);

	w.DestroyEntity(e);

	CHECK_FALSE(w.IsEntityValid(e));

	CHECK_EQ(w.deadIndices.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);
	
	CHECK_NULL(b.chunk.header);
	CHECK_NULL(b.chunk.data);

	CHECK_EQ(w.archetypes.Size(), 1);

	Archetype* a = w.archetypes[0].Get();

	CHECK_PTR(a);
	CHECK_EQ(a->types.Size(), 1);
	CHECK_EQ(a->types[0], GetComponentTypeFor<Position>());

	CHECK_EQ(a->chunks.Size(), 1);
	CHECK_ZERO(a->chunks[0].header->entityCount);
}

TEST(EntityCreateDestroyLoop, EntityCreateDestroy)
{
	World w;

	constexpr u32 loops = 100;
	for (u32 i = 0; i < loops; ++i)
	{
		Entity e = w.CreateEntity<Position, Rotation>();

		CHECK_TRUE(w.IsEntityValid(e));
		CHECK_EQ(w.deadIndices.Size(), 0);
		CHECK_EQ(w.archetypes.Size(), 1);

		CHECK_EQ(e.id, 0);
		CHECK_EQ(e.version, i + 1);
		CHECK_EQ(w.entityBridges.Size(), 1);

		EntityBridge& b = w.entityBridges[0];

		CHECK_EQ(b.version, e.version);
		CHECK_PTR(b.chunk.header);
		CHECK_PTR(b.chunk.data);
		CHECK_EQ(b.chunkIndex, 0);

		w.DestroyEntity(e);

		CHECK_FALSE(w.IsEntityValid(e));
		CHECK_EQ(w.deadIndices.Size(), 1);
		CHECK_EQ(w.archetypes.Size(), 1);
	}
}

TEST(EntityCreateMultiple, EntityCreateDestroy)
{
	World w;

	Entity e0 = w.CreateEntity<Position>();
	Entity e1 = w.CreateEntity<Position>();
	Entity e2 = w.CreateEntity<Position>();

	CHECK_EQ(w.archetypes.Size(), 1);
	
	Archetype* archetype = w.archetypes[0].Get();
	CHECK_PTR(archetype);


	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_TRUE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(e0.id, 0);
	CHECK_EQ(e1.id, 1);
	CHECK_EQ(e2.id, 2);
	CHECK_EQ(e0.version, 1);
	CHECK_EQ(e1.version, 1);
	CHECK_EQ(e2.version, 1);

	CHECK_EQ(archetype->chunks.Size(), 1);
	CHECK_EQ(archetype->chunks[0].header->entityCount, 3);

	// Validating chunk data
	ArchetypeChunk chunk = archetype->chunks[0];
	ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
	CHECK_TRUE(arr.IsValid());

	for (u32 i = 0; i < arr.size; ++i)
	{
		CHECK_EQ(arr[i].id, i);
		CHECK_EQ(arr[i].version, 1);
	}

	ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
	CHECK_TRUE(posArr.IsValid());
	for (u32 i = 0; i < posArr.size; ++i)
	{
		CHECK_TRUE(posArr[i].position.IsZero());
	}

}

TEST(EntityCreateMultipleDestroyOne, EntityCreateDestroy)
{
	World w;

	Entity e0 = w.CreateEntity<Position>();
	Entity e1 = w.CreateEntity<Position>();
	Entity e2 = w.CreateEntity<Position>();

	CHECK_EQ(w.archetypes.Size(), 1);

	Archetype* archetype = w.archetypes[0].Get();
	CHECK_PTR(archetype);


	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_TRUE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(e0.id, 0);
	CHECK_EQ(e1.id, 1);
	CHECK_EQ(e2.id, 2);
	CHECK_EQ(e0.version, 1);
	CHECK_EQ(e1.version, 1);
	CHECK_EQ(e2.version, 1);

	CHECK_EQ(archetype->chunks.Size(), 1);

	ArchetypeChunk chunk = archetype->chunks[0];
	CHECK_EQ(chunk.header->entityCount, 3);

	// Validating chunk data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());

		for (u32 i = 0; i < arr.size; ++i)
		{
			CHECK_EQ(arr[i].id, i);
			CHECK_EQ(arr[i].version, 1);
		}

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}

	// Destroy one entity
	w.DestroyEntity(e1);

	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_FALSE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(w.entityBridges.Size(), 3);
	CHECK_EQ(w.deadIndices.Size(), 1);
	CHECK_EQ(w.deadIndices[0], 1);

	CHECK_EQ(chunk.header->entityCount, 2);

	// Revalidate the entity data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());
		CHECK_EQ(arr.size, 2);

		CHECK_EQ(arr[0].id, 0);
		CHECK_EQ(arr[0].version, 1);
	
		CHECK_EQ(arr[1].id, 2);
		CHECK_EQ(arr[1].version, 1);

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		CHECK_EQ(posArr.size, 2);

		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}
}

TEST(EntityCreateMultipleDestroyTwo, EntityCreateDestroy)
{
	World w;

	Entity e0 = w.CreateEntity<Position>();
	Entity e1 = w.CreateEntity<Position>();
	Entity e2 = w.CreateEntity<Position>();

	CHECK_EQ(w.archetypes.Size(), 1);

	Archetype* archetype = w.archetypes[0].Get();
	CHECK_PTR(archetype);


	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_TRUE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(e0.id, 0);
	CHECK_EQ(e1.id, 1);
	CHECK_EQ(e2.id, 2);
	CHECK_EQ(e0.version, 1);
	CHECK_EQ(e1.version, 1);
	CHECK_EQ(e2.version, 1);

	CHECK_EQ(archetype->chunks.Size(), 1);

	ArchetypeChunk chunk = archetype->chunks[0];
	CHECK_EQ(chunk.header->entityCount, 3);

	// Validating chunk data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());

		for (u32 i = 0; i < arr.size; ++i)
		{
			CHECK_EQ(arr[i].id, i);
			CHECK_EQ(arr[i].version, 1);
		}

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}

	// Destroy one entity
	w.DestroyEntity(e1);

	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_FALSE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(w.entityBridges.Size(), 3);
	CHECK_EQ(w.deadIndices.Size(), 1);
	CHECK_EQ(w.deadIndices[0], 1);

	CHECK_EQ(chunk.header->entityCount, 2);

	// Revalidate the entity data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());
		CHECK_EQ(arr.size, 2);

		CHECK_EQ(arr[0].id, 0);
		CHECK_EQ(arr[0].version, 1);

		CHECK_EQ(arr[1].id, 2);
		CHECK_EQ(arr[1].version, 1);

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		CHECK_EQ(posArr.size, 2);

		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}

	// Destroy another entity
	w.DestroyEntity(e0);

	CHECK_FALSE(w.IsEntityValid(e0));
	CHECK_FALSE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(w.entityBridges.Size(), 3);
	CHECK_EQ(w.deadIndices.Size(), 2);
	CHECK_EQ(w.deadIndices[0], 1);
	CHECK_EQ(w.deadIndices[1], 0);

	CHECK_EQ(chunk.header->entityCount, 1);

	// Revalidate the entity data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());
		CHECK_EQ(arr.size, 1);

		CHECK_EQ(arr[0].id, 2);
		CHECK_EQ(arr[0].version, 1);

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		CHECK_EQ(posArr.size, 1);

		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}
}

TEST(EntityCreateMultipleDestroyAll, EntityCreateDestroy)
{
	World w;

	Entity e0 = w.CreateEntity<Position>();
	Entity e1 = w.CreateEntity<Position>();
	Entity e2 = w.CreateEntity<Position>();

	CHECK_EQ(w.archetypes.Size(), 1);

	Archetype* archetype = w.archetypes[0].Get();
	CHECK_PTR(archetype);


	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_TRUE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(e0.id, 0);
	CHECK_EQ(e1.id, 1);
	CHECK_EQ(e2.id, 2);
	CHECK_EQ(e0.version, 1);
	CHECK_EQ(e1.version, 1);
	CHECK_EQ(e2.version, 1);

	CHECK_EQ(archetype->chunks.Size(), 1);

	ArchetypeChunk chunk = archetype->chunks[0];
	CHECK_EQ(chunk.header->entityCount, 3);

	// Validating chunk data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());

		for (u32 i = 0; i < arr.size; ++i)
		{
			CHECK_EQ(arr[i].id, i);
			CHECK_EQ(arr[i].version, 1);
		}

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}

	// Destroy one entity
	w.DestroyEntity(e1);

	CHECK_TRUE(w.IsEntityValid(e0));
	CHECK_FALSE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(w.entityBridges.Size(), 3);
	CHECK_EQ(w.deadIndices.Size(), 1);
	CHECK_EQ(w.deadIndices[0], 1);

	CHECK_EQ(chunk.header->entityCount, 2);

	// Revalidate the entity data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());
		CHECK_EQ(arr.size, 2);

		CHECK_EQ(arr[0].id, 0);
		CHECK_EQ(arr[0].version, 1);

		CHECK_EQ(arr[1].id, 2);
		CHECK_EQ(arr[1].version, 1);

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		CHECK_EQ(posArr.size, 2);

		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}

	// Destroy another entity
	w.DestroyEntity(e0);

	CHECK_FALSE(w.IsEntityValid(e0));
	CHECK_FALSE(w.IsEntityValid(e1));
	CHECK_TRUE(w.IsEntityValid(e2));

	CHECK_EQ(w.entityBridges.Size(), 3);
	CHECK_EQ(w.deadIndices.Size(), 2);
	CHECK_EQ(w.deadIndices[0], 1);
	CHECK_EQ(w.deadIndices[1], 0);

	CHECK_EQ(chunk.header->entityCount, 1);

	// Revalidate the entity data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());
		CHECK_EQ(arr.size, 1);

		CHECK_EQ(arr[0].id, 2);
		CHECK_EQ(arr[0].version, 1);

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		CHECK_EQ(posArr.size, 1);

		for (u32 i = 0; i < posArr.size; ++i)
		{
			CHECK_TRUE(posArr[i].position.IsZero());
		}
	}


	// Destroy last entity
	w.DestroyEntity(e2);

	CHECK_FALSE(w.IsEntityValid(e0));
	CHECK_FALSE(w.IsEntityValid(e1));
	CHECK_FALSE(w.IsEntityValid(e2));

	CHECK_EQ(w.entityBridges.Size(), 3);
	CHECK_EQ(w.deadIndices.Size(), 3);
	CHECK_EQ(w.deadIndices[0], 1);
	CHECK_EQ(w.deadIndices[1], 0);
	CHECK_EQ(w.deadIndices[2], 2);

	CHECK_EQ(chunk.header->entityCount, 0);

	// Revalidate the entity data
	{
		ChunkArray<Entity> arr = GetChunkArray<Entity>(chunk);
		CHECK_TRUE(arr.IsValid());
		CHECK_EQ(arr.size, 0);

		ChunkArray<Position> posArr = GetChunkArray<Position>(chunk);
		CHECK_TRUE(posArr.IsValid());
		CHECK_EQ(posArr.size, 0);
	}
}

TEST(EntityCreateForceNewChunk, EntityCreateDestroy)
{
	World w;

	Archetype* archetype = GetOrCreateArchetypeFrom<Position>(w);
	CHECK_PTR(archetype);

	Entity init = w.CreateEntity(*archetype);
	CHECK_TRUE(w.IsEntityValid(init));

	CHECK_EQ(init.id, 0);
	CHECK_EQ(init.version, 1);

	CHECK_EQ(archetype->chunks.Size(), 1);

	ArchetypeChunk firstChunk = archetype->chunks[0];
	CHECK_EQ(firstChunk.header->entityCount, 1);

	const u32 maxEntitiesPerChunk = archetype->entityCapacity;

	for (u32 i = 0; i < maxEntitiesPerChunk; ++i)
	{
		w.CreateEntity(*archetype);
	}

	CHECK_EQ(archetype->chunks.Size(), 2);

	CHECK_EQ(firstChunk.header->entityCount, maxEntitiesPerChunk);

	ArchetypeChunk secondChunk = archetype->chunks[1];
	CHECK_EQ(secondChunk.header->entityCount, 1);
}

TEST(EntityCreateDestroyMakeNotFull, EntityCreateDestroy)
{
	World w;

	Archetype* archetype = GetOrCreateArchetypeFrom<Position>(w);
	CHECK_PTR(archetype);

	Entity init = w.CreateEntity(*archetype);
	CHECK_TRUE(w.IsEntityValid(init));

	CHECK_EQ(init.id, 0);
	CHECK_EQ(init.version, 1);

	CHECK_EQ(archetype->chunks.Size(), 1);

	ArchetypeChunk firstChunk = archetype->chunks[0];
	CHECK_EQ(firstChunk.header->entityCount, 1);

	const u32 maxEntitiesPerChunk = archetype->entityCapacity;

	Entity e{};
	for (u32 i = 0; i < maxEntitiesPerChunk; ++i)
	{
		e = w.CreateEntity(*archetype);
	}

	CHECK_EQ(archetype->chunks.Size(), 2);
	CHECK_EQ(archetype->fullChunkCount, 1);

	CHECK_EQ(firstChunk.header->entityCount, maxEntitiesPerChunk);

	ArchetypeChunk secondChunk = archetype->chunks[1];
	CHECK_EQ(secondChunk.header->entityCount, 1);

	w.DestroyEntity(e);

	CHECK_FALSE(w.IsEntityValid(e));
	CHECK_EQ(secondChunk.header->entityCount, 0);

	--e.id;
	CHECK_TRUE(w.IsEntityValid(e));

	CHECK_EQ(firstChunk.header->entityCount, maxEntitiesPerChunk);

	w.DestroyEntity(e);
	CHECK_FALSE(w.IsEntityValid(e));

	CHECK_EQ(firstChunk.header->entityCount, maxEntitiesPerChunk-1);
}

