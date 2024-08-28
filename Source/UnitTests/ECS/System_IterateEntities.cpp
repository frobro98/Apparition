// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"
#include "ECS/Archetype.hpp"
#include "ECS/World.hpp"
#include "ECS/System.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"

using namespace Musa;

static bool updatedSuccess = true;

//////////////////////////////////////////////////////////////////////////
// Iterate Entity System
//////////////////////////////////////////////////////////////////////////

class IterateEntitySystem : public System
{
public:
	IterateEntitySystem(u32 expectedEntityCount, const char* n, UnitData& data, UnitStats& stats)
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

void IterateEntitySystem::Initialize()
{
	updatedSuccess = false;
}

void IterateEntitySystem::Update()
{
	World& w = GetWorld();
	CHECK_REF(w);

	// Query gets all of the archetypes that match types asked for
	Query& q0 = GetQueryFor<Position>();

	// Need to get all entities from Query object
	ArrayView<Entity> entities = GetQueriedEntities(q0);
	CHECK_EQ(entities.Size(), numEntitiesExpected);

	updatedSuccess = true;
}

void IterateEntitySystem::Deinitialize()
{
	updatedSuccess = false;
}

//////////////////////////////////////////////////////////////////////////
// Iterate And Check System
//////////////////////////////////////////////////////////////////////////

class IterateAndCheckSystem : public System
{
public:
	IterateAndCheckSystem(u32 expectedEntityCount, const char* n, UnitData& data, UnitStats& stats)
		: name(n), _UnitData(data), _UnitStats(stats), numEntitiesExpected(expectedEntityCount)
	{}

	virtual void Initialize() override;
	virtual void Update() override;
	virtual void Deinitialize() override;

	bool UnitTestUpdate()
	{
		Update();
		return updatedSuccess;
	}

	// This is here just to allow the UnitTestSystem be hooked into the unit test framework
	void teardown() {};

private:
	const char* name;
	UnitData& _UnitData;
	UnitStats& _UnitStats;
	u32 numEntitiesExpected;
};

void IterateAndCheckSystem::Initialize()
{
	updatedSuccess = false;
}

void IterateAndCheckSystem::Update()
{
	World& w = GetWorld();
	CHECK_REF(w);

	// Query gets all of the archetypes that match types asked for
	Query& q0 = GetQueryFor<Position>();

	// Need to get all entities from Query object
	ArrayView<Entity> entities = GetQueriedEntities(q0);
	CHECK_EQ(entities.Size(), numEntitiesExpected);

	// Iterate over and check values
	for (auto& entity : entities)
	{
		Vector4 v = Vector4(1, 2, 500, 32456.543f);

		Position& p = w.GetComponentDataOn<Position>(entity);
		CHECK_EQ(p.position, v);
	}

	updatedSuccess = true;
}

void IterateAndCheckSystem::Deinitialize()
{
	updatedSuccess = false;
}

//////////////////////////////////////////////////////////////////////////
// Unit Tests
//////////////////////////////////////////////////////////////////////////

TEST(SystemIterateOver50Entities, SystemIterateEntities)
{
	World w;
	CHECK_REF(w);

	constexpr u32 numEntitiesExpected = 50;
	for (u32 i = 0; i < numEntitiesExpected; ++i)
	{
		NOT_USED Entity e = w.CreateEntity<Position>();
	}

	IterateEntitySystem& s = w.CreateSystem<IterateEntitySystem>(numEntitiesExpected, name, _UnitData, _UnitStats);
	CHECK_REF(s);
	CHECK_EQ(w.systems.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<IterateEntitySystem>());

	CHECK_FALSE(updatedSuccess);
	w.Update();
	CHECK_TRUE(updatedSuccess);
}

TEST(SystemIterateOver40EntitiesDiffArchetypes, SystemIterateEntities)
{
	World w;
	CHECK_REF(w);

	constexpr u32 numEntitiesToCreate = 40;
	constexpr u32 numEntitiesExpected = 30;
	for (u32 i = 0; i < numEntitiesToCreate / 4; ++i)
	{
		Entity e = w.CreateEntity<Position>();
		e = w.CreateEntity<Movement>();
		e = w.CreateEntity<Position, Rotation>();
		e = w.CreateEntity<Rotation, Position, Movement>();
	}

	IterateEntitySystem& s = w.CreateSystem<IterateEntitySystem>(numEntitiesExpected, name, _UnitData, _UnitStats);
	CHECK_REF(s);
	CHECK_EQ(w.systems.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<IterateEntitySystem>());

	CHECK_FALSE(updatedSuccess);
	w.Update();
	CHECK_TRUE(updatedSuccess);
}

TEST(SystemIterateOver40EntitiesCheckData, SystemIterateEntities)
{
	World w;
	CHECK_REF(w);

	constexpr u32 numEntitiesToCreate = 40;
	constexpr u32 numEntitiesExpected = 30;
	for (u32 i = 0; i < numEntitiesToCreate / 4; ++i)
	{
		Vector4 v = Vector4(1, 2, 500, 32456.543f);

		{
			Entity e = w.CreateEntity<Position>();
			w.SetComponentDataOn(e, Position{ {}, v });
		}

		{
			Entity e = w.CreateEntity<Movement>();
		}

		{
			Entity e = w.CreateEntity<Position, Rotation>();
			w.SetComponentDataOn(e, Position{ {}, v });
		}

		{
			Entity e = w.CreateEntity<Rotation, Position, Movement>();
			w.SetComponentDataOn(e, Position{ {}, v });
		}
	}

	IterateAndCheckSystem& s = w.CreateSystem<IterateAndCheckSystem>(numEntitiesExpected, name, _UnitData, _UnitStats);
	CHECK_REF(s);
	CHECK_EQ(w.systems.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<IterateAndCheckSystem>());

	CHECK_FALSE(updatedSuccess);
	w.Update();
	CHECK_TRUE(updatedSuccess);
}

