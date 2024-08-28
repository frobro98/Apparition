// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "ECS/Archetype.hpp"
#include "ECS/World.hpp"
#include "Memory/MemoryCore.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"


using namespace Musa;

TEST(ArchetypeCreateOne, ArchetypeTesting)
{
	World w;

	Archetype* archetype = GetOrCreateArchetypeFrom<Position>(w);
	const ComponentType* posType = GetComponentTypeFor<Position>();

	CHECK(archetype->world);
	CHECK_EQ(archetype->world, &w);

	CHECK_EQ(archetype->types.Size(), 1);
	CHECK_EQ(archetype->types[0], posType);
	CHECK_ZERO(archetype->chunks.Size());

	u32 archCap = archetype->entityCapacity;
	size_t calcOffset = sizeof(Entity) * archCap;
	calcOffset += (posType->alignment - (calcOffset % posType->alignment));
	size_t offset = archetype->offsets[0];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, posType->alignment));

	ComponentTypeHash hash = posType->hash;
	ComponentTypeHash archTypeHash = archetype->typeHashes[0];
	CHECK_EQ(hash, archTypeHash);

	CHECK_ZERO(archetype->fullChunkCount);
}

TEST(ArchetypeCreateTwo, ArchetypeTesting)
{
	World w;

	Archetype* archetype0 = GetOrCreateArchetypeFrom<Position>(w);
	const ComponentType* posType = GetComponentTypeFor<Position>();

	// First Archetype Creation

	CHECK(archetype0->world);
	CHECK_EQ(archetype0->world, &w);

	CHECK_EQ(archetype0->types.Size(), 1);
	CHECK_EQ(archetype0->types[0], posType);
	CHECK_ZERO(archetype0->chunks.Size());
	CHECK_ZERO(archetype0->fullChunkCount);

	// Position Offset Testing
	u32 archCap = archetype0->entityCapacity;
	size_t calcOffset = sizeof(Entity) * archCap;
	calcOffset += (posType->alignment - (calcOffset % posType->alignment));
	size_t offset = archetype0->offsets[0];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, posType->alignment));

	// Hash Testing
	ComponentTypeHash posHash = posType->hash;
	ComponentTypeHash archTypeHash = archetype0->typeHashes[0];
	CHECK_EQ(posHash, archTypeHash);


	// Second Archetype Creation
	Archetype* archetype1 = GetOrCreateArchetypeFrom<Position, Rotation>(w);
	const ComponentType* rotType = GetComponentTypeFor<Rotation>();

	CHECK(archetype1->world);
	CHECK_EQ(archetype1->world, &w);

	CHECK_EQ(archetype1->types.Size(), 2);
	CHECK_ZERO(archetype1->chunks.Size());
	CHECK_ZERO(archetype1->fullChunkCount);

	// Position Offset Testing
	archCap = archetype1->entityCapacity;
	calcOffset = sizeof(Entity) * archCap;
	calcOffset += (posType->alignment - (calcOffset % posType->alignment));
	offset = archetype1->offsets[0];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, posType->alignment));

	calcOffset += posType->size * archCap;

	// Rotation Offset Testing
	calcOffset += (rotType->alignment - (calcOffset % rotType->alignment));
	offset = archetype1->offsets[1];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, rotType->alignment));

	// Hash Testing
	ComponentTypeHash rotHash = rotType->hash;
	archTypeHash = archetype1->typeHashes[1];
	CHECK_EQ(rotHash, archTypeHash);
}

TEST(ArchetypeCreateThree, ArchetypeTesting)
{
	World w;

	Archetype* archetype0 = GetOrCreateArchetypeFrom<Position>(w);
	const ComponentType* posType = GetComponentTypeFor<Position>();

	// First Archetype Creation

	CHECK(archetype0->world);
	CHECK_EQ(archetype0->world, &w);

	CHECK_EQ(archetype0->types.Size(), 1);
	CHECK_EQ(archetype0->types[0], posType);
	CHECK_ZERO(archetype0->chunks.Size());
	CHECK_ZERO(archetype0->fullChunkCount);

	// Position Offset Testing
	u32 archCap = archetype0->entityCapacity;
	size_t calcOffset = sizeof(Entity) * archCap;
	calcOffset += (posType->alignment - (calcOffset % posType->alignment));
	size_t offset = archetype0->offsets[0];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, posType->alignment));

	// Hash Testing
	ComponentTypeHash posHash = posType->hash;
	ComponentTypeHash archTypeHash = archetype0->typeHashes[0];
	CHECK_EQ(posHash, archTypeHash);


	// Second Archetype Creation
	Archetype* archetype1 = GetOrCreateArchetypeFrom<Position, Rotation>(w);
	const ComponentType* rotType = GetComponentTypeFor<Rotation>();

	CHECK(archetype1->world);
	CHECK_EQ(archetype1->world, &w);

	CHECK_EQ(archetype1->types.Size(), 2);
	CHECK_ZERO(archetype1->chunks.Size());
	CHECK_ZERO(archetype1->fullChunkCount);

	// Position Offset Testing
	archCap = archetype1->entityCapacity;
	calcOffset = sizeof(Entity) * archCap;
	calcOffset += (posType->alignment - (calcOffset % posType->alignment));
	offset = archetype1->offsets[0];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, posType->alignment));

	calcOffset += posType->size * archCap;

	// Hash Testing
	posHash = posType->hash;
	archTypeHash = archetype0->typeHashes[0];
	CHECK_EQ(posHash, archTypeHash);

	// Rotation Offset Testing
	calcOffset += (rotType->alignment - (calcOffset % rotType->alignment));
	offset = archetype1->offsets[1];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, rotType->alignment));

	// Hash Testing
	ComponentTypeHash rotHash = rotType->hash;
	archTypeHash = archetype1->typeHashes[1];
	CHECK_EQ(rotHash, archTypeHash);


	// Third Archetype Creation
	Archetype* archetype2 = GetOrCreateArchetypeFrom<Position, Rotation, Movement>(w);
	const ComponentType* moveType = GetComponentTypeFor<Movement>();

	CHECK(archetype2->world);
	CHECK_EQ(archetype2->world, &w);

	CHECK_EQ(archetype2->types.Size(), 3);
	CHECK_ZERO(archetype2->chunks.Size());
	CHECK_ZERO(archetype2->fullChunkCount);

	// Position Offset Testing
	archCap = archetype2->entityCapacity;
	calcOffset = sizeof(Entity) * archCap;
	calcOffset += (posType->alignment - (calcOffset % posType->alignment));
	offset = archetype2->offsets[0];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, posType->alignment));

	calcOffset += posType->size * archCap;

	// Hash Testing
	posHash = posType->hash;
	archTypeHash = archetype0->typeHashes[0];
	CHECK_EQ(posHash, archTypeHash);

	// Rotation Offset Testing
	calcOffset += (rotType->alignment - (calcOffset % rotType->alignment));
	offset = archetype2->offsets[1];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, rotType->alignment));

	calcOffset += rotType->size * archCap;

	// Hash Testing
	rotHash = rotType->hash;
	archTypeHash = archetype2->typeHashes[1];
	CHECK_EQ(rotHash, archTypeHash);

	// Movement Offset Testing
	calcOffset += (moveType->alignment - (calcOffset % moveType->alignment));
	offset = archetype2->offsets[2];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, moveType->alignment));

	// Hash Testing
	ComponentTypeHash moveHash = moveType->hash;
	archTypeHash = archetype2->typeHashes[2];
	CHECK_EQ(moveHash, archTypeHash);

}

TEST(ArchetypeGetExisting, ArchetypeTesting)
{
	World w;

	Archetype* archetype = GetOrCreateArchetypeFrom<Position>(w);
	const ComponentType* posType = GetComponentTypeFor<Position>();

	CHECK(archetype->world);
	CHECK_EQ(archetype->world, &w);

	CHECK_EQ(archetype->types.Size(), 1);
	CHECK_EQ(archetype->types[0], posType);
	CHECK_ZERO(archetype->chunks.Size());
	CHECK_ZERO(archetype->fullChunkCount);

	u32 archCap = archetype->entityCapacity;
	size_t calcOffset = sizeof(Entity) * archCap;
	calcOffset += (posType->alignment - (calcOffset % posType->alignment));
	size_t offset = archetype->offsets[0];

	CHECK_EQ(calcOffset, offset);
	CHECK_TRUE(IsAligned(offset, posType->alignment));

	ComponentTypeHash hash = posType->hash;
	ComponentTypeHash archTypeHash = archetype->typeHashes[0];
	CHECK_EQ(hash, archTypeHash);

	Archetype* existingArchetype = GetOrCreateArchetypeFrom<Position>(w);

	CHECK_EQ(archetype, existingArchetype);
}

TEST(ArchetypeNullType, ArchetypeTesting)
{
	World w;

	Archetype* archetype = GetOrCreateArchetypeFrom(w, nullptr, 0);

	CHECK_PTR(archetype->world);
	CHECK_EQ(archetype->world, &w);

	CHECK_ZERO(archetype->types.Size());
	CHECK_ZERO(archetype->chunks.Size());
	CHECK_ZERO(archetype->offsets.Size());
	CHECK_ZERO(archetype->typeHashes.Size());
	CHECK_ZERO(archetype->fullChunkCount);
}

