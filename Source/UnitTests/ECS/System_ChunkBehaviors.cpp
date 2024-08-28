// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "ECS/Archetype.hpp"
#include "ECS/World.hpp"
#include "ECS/System.hpp"
#include "ECS/ChunkComponentAccessor.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"

using namespace Musa;

static bool updatedSuccess = true;

//////////////////////////////////////////////////////////////////////////
// Iterate Entity System
//////////////////////////////////////////////////////////////////////////

class IterateChunkSystem : public System
{
public:
	IterateChunkSystem(u32 expectedEntityCount, const char* n, UnitData& data, UnitStats& stats)
		: name(n), _UnitData(data), _UnitStats(stats), numEntitiesExpected(expectedEntityCount)
	{}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Deinitialize() override;

	// This is here just to allow the UnitTestSystem be hooked into the unit test framework
	void teardown() {};

private:
	const char* name;
	UnitData& _UnitData;
	UnitStats& _UnitStats;
	u32 numEntitiesExpected;
};

void IterateChunkSystem::Initialize()
{
	updatedSuccess = false;
}

void IterateChunkSystem::Update()
{
	World& w = GetWorld();
	CHECK_REF(w);

	// Query gets all of the archetypes that match types asked for
	QueryDescription desc = DescribeQuery().OneOrMore<Position, Rotation>();
	Query& q0 = GetQueryFor(desc);

	// Need to get all entities from Query object
	ArrayView<Entity> entities = GetQueriedEntities(q0);
	CHECK_EQ(entities.Size(), numEntitiesExpected);

	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(q0);
	CHECK_GT(chunks.Size(), 0);

	u32 numEntitiesInChunks = 0;
	for (auto& chunk : chunks)
	{
		numEntitiesInChunks += chunk.GetEntityCount();
	}
	CHECK_EQ(numEntitiesInChunks, numEntitiesExpected);

	updatedSuccess = true;
}

void IterateChunkSystem::Deinitialize()
{
	updatedSuccess = false;
}

//////////////////////////////////////////////////////////////////////////
// Check Chunk Version System
//////////////////////////////////////////////////////////////////////////

class CheckChunkVersionSystem : public System
{
public:
	CheckChunkVersionSystem(u32 expectedEntityCount, const char* n, UnitData& data, UnitStats& stats)
		: name(n), _UnitData(data), _UnitStats(stats), numEntitiesExpected(expectedEntityCount)
	{}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Deinitialize() override;

	// This is here just to allow the UnitTestSystem be hooked into the unit test framework
	void teardown() {};

private:
	const char* name;
	UnitData& _UnitData;
	UnitStats& _UnitStats;
	u32 numEntitiesExpected;
	bool versionUpdate = false;
};

void CheckChunkVersionSystem::Initialize()
{
	updatedSuccess = false;
}

void CheckChunkVersionSystem::Update()
{
	World& w = GetWorld();
	CHECK_REF(w);

	// Query gets all of the archetypes that match types asked for
	QueryDescription desc = DescribeQuery().Require<Position, Rotation>();
	Query& q0 = GetQueryFor(desc);

	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(q0);
	CHECK_GT(chunks.Size(), 0);

	u32 numEntitiesInChunks = 0;
	for (auto& chunk : chunks)
	{
		numEntitiesInChunks += chunk.GetEntityCount();
	}
	CHECK_EQ(numEntitiesInChunks, numEntitiesExpected);

	if (!versionUpdate)
	{
		CHECK_TRUE(chunks[0].HasChanged<Position>(GetVersion()));
		CHECK_TRUE(chunks[0].HasChanged<Rotation>(GetVersion()));

		versionUpdate = true;
	}
	else
	{
		CHECK_FALSE(chunks[0].HasChanged<Position>(GetVersion()));
		CHECK_FALSE(chunks[0].HasChanged<Rotation>(GetVersion()));
	}

	updatedSuccess = true;
}

void CheckChunkVersionSystem::Deinitialize()
{
	updatedSuccess = false;
}

//////////////////////////////////////////////////////////////////////////
// Modify And Check Chunk System
//////////////////////////////////////////////////////////////////////////

class ModifyChunkSystem : public System
{
public:
	ModifyChunkSystem(u32 expectedEntityCount, const char* n, UnitData& data, UnitStats& stats)
		: name(n), _UnitData(data), _UnitStats(stats), numEntitiesExpected(expectedEntityCount)
	{}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Deinitialize() override;

	// This is here just to allow the UnitTestSystem be hooked into the unit test framework
	void teardown() {};

private:
	const char* name;
	UnitData& _UnitData;
	UnitStats& _UnitStats;
	u32 numEntitiesExpected;
	bool versionUpdated = false;
};

void ModifyChunkSystem::Initialize()
{
	updatedSuccess = false;
}

void ModifyChunkSystem::Update()
{
	World& w = GetWorld();
	CHECK_REF(w);

	// Query gets all of the archetypes that match types asked for
	QueryDescription desc = DescribeQuery().Require<Position, Rotation>();
	Query& q0 = GetQueryFor(desc);

	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(q0);
	CHECK_GT(chunks.Size(), 0);

	u32 numEntitiesInChunks = 0;
	for (auto& chunk : chunks)
	{
		numEntitiesInChunks += chunk.GetEntityCount();
	}
	CHECK_EQ(numEntitiesInChunks, numEntitiesExpected);


	if (!versionUpdated)
	{
		CHECK_TRUE(chunks[0].HasChanged<Position>(GetVersion()));
		CHECK_TRUE(chunks[0].HasChanged<Rotation>(GetVersion()));
		versionUpdated = true;
	}
	else
	{
		CHECK_FALSE(chunks[0].HasChanged<Position>(GetVersion()));
		CHECK_FALSE(chunks[0].HasChanged<Rotation>(GetVersion()));

		NOT_USED ChunkArray<Position> posArr = chunks[0].GetArrayOf<Position>();
		NOT_USED ChunkArray<const Rotation> rotArr = chunks[0].GetArrayOf<const Rotation>();
	}

	updatedSuccess = true;
}

void ModifyChunkSystem::Deinitialize()
{
	updatedSuccess = false;
}

//////////////////////////////////////////////////////////////////////////
// Check After Modification Chunk System
//////////////////////////////////////////////////////////////////////////

class CheckAfterModificationkSystem : public System
{
public:
	CheckAfterModificationkSystem(u32 expectedEntityCount, const char* n, UnitData& data, UnitStats& stats)
		: name(n), _UnitData(data), _UnitStats(stats), numEntitiesExpected(expectedEntityCount)
	{}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Deinitialize() override;

	// This is here just to allow the UnitTestSystem be hooked into the unit test framework
	void teardown() {};

private:
	const char* name;
	UnitData& _UnitData;
	UnitStats& _UnitStats;
	u32 numEntitiesExpected;
	bool versionUpdated = false;
};

void CheckAfterModificationkSystem::Initialize()
{
	updatedSuccess = false;
}

void CheckAfterModificationkSystem::Update()
{
	World& w = GetWorld();
	CHECK_REF(w);

	// Query gets all of the archetypes that match types asked for
	QueryDescription desc = DescribeQuery().Require<Position, Rotation>();
	Query& q0 = GetQueryFor(desc);

	DynamicArray<ChunkComponentAccessor> chunks = GetQueryChunks(q0);
	CHECK_GT(chunks.Size(), 0);

	u32 numEntitiesInChunks = 0;
	for (auto& chunk : chunks)
	{
		numEntitiesInChunks += chunk.GetEntityCount();
	}
	CHECK_EQ(numEntitiesInChunks, numEntitiesExpected);

	if (!versionUpdated)
	{
		CHECK_TRUE(chunks[0].HasChanged<Position>(GetVersion()));
		CHECK_TRUE(chunks[0].HasChanged<Rotation>(GetVersion()));
		versionUpdated = true;
	}
	else
	{
		CHECK_TRUE(chunks[0].HasChanged<Position>(GetVersion()));
		CHECK_FALSE(chunks[0].HasChanged<Rotation>(GetVersion()));
	}


	updatedSuccess = true;
}

void CheckAfterModificationkSystem::Deinitialize()
{
	updatedSuccess = false;
}

//////////////////////////////////////////////////////////////////////////
// Unit Tests
//////////////////////////////////////////////////////////////////////////

TEST(IterateQueriedChunks, SystemChunkBehaviors)
{
	updatedSuccess = true;

	World w;
	CHECK_REF(w);

	constexpr u32 numEntitiesExpected = 50;
	for (u32 i = 0; i < numEntitiesExpected / 2; ++i)
	{
		NOT_USED Entity e = w.CreateEntity<Position>();
	}

	for (u32 i = 0; i < numEntitiesExpected / 2; ++i)
	{
		NOT_USED Entity e = w.CreateEntity<Rotation>();
	}

	IterateChunkSystem& s = w.CreateSystem<IterateChunkSystem>(numEntitiesExpected, name, _UnitData, _UnitStats);
	CHECK_REF(s);
	CHECK_EQ(w.systems.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<IterateChunkSystem>());

	CHECK_FALSE(updatedSuccess);
	w.Update();
	CHECK_TRUE(updatedSuccess);

}

TEST(CheckVersionQueriedChunk, SystemChunkBehaviors)
{
	updatedSuccess = true;

	World w;
	CHECK_REF(w);

	constexpr u32 numEntitiesExpected = 50;
	for (u32 i = 0; i < numEntitiesExpected; ++i)
	{
		NOT_USED Entity e = w.CreateEntity<Position, Rotation>();
	}

	CheckChunkVersionSystem& s = w.CreateSystem<CheckChunkVersionSystem>(numEntitiesExpected, name, _UnitData, _UnitStats);
	CHECK_REF(s);
	CHECK_EQ(w.systems.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<CheckChunkVersionSystem>());

	CHECK_FALSE(updatedSuccess);
	w.Update();
	w.Update();
	CHECK_TRUE(updatedSuccess);
}

TEST(ModifyAndCheckVersionQueriedChunk, SystemChunkBehaviors)
{
	updatedSuccess = true;

	World w;
	CHECK_REF(w);

	constexpr u32 numEntitiesExpected = 50;
	for (u32 i = 0; i < numEntitiesExpected; ++i)
	{
		NOT_USED Entity e = w.CreateEntity<Position, Rotation>();
	}

	ModifyChunkSystem& s = w.CreateSystem<ModifyChunkSystem>(numEntitiesExpected, name, _UnitData, _UnitStats);
	CHECK_REF(s);
	CHECK_EQ(w.systems.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<ModifyChunkSystem>());

	CheckAfterModificationkSystem& cs = w.CreateSystem<CheckAfterModificationkSystem>(numEntitiesExpected, name, _UnitData, _UnitStats);
	CHECK_REF(cs);
	CHECK_EQ(w.systems.Size(), 2);
	CHECK_EQ(w.systemTypesInWorld.Size(), 2);
	CHECK_EQ(w.systemTypesInWorld[1], GetSystemTypeFor<CheckAfterModificationkSystem>());

	CHECK_FALSE(updatedSuccess);
	w.Update();
	w.Update();
	CHECK_TRUE(updatedSuccess);
}

