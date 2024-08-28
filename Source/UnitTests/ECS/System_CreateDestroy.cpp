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

class UnitTestSystem : public System
{
public:
	UnitTestSystem(const char* n, UnitData& data, UnitStats& stats)
		: name(n), _UnitData(data), _UnitStats(stats)
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
};

void UnitTestSystem::Initialize()
{
	updatedSuccess = false;
}

void UnitTestSystem::Update()
{
	World& w = GetWorld();
	CHECK_REF(w);

	updatedSuccess = true;
}

void UnitTestSystem::Deinitialize()
{
	updatedSuccess = false;
}

TEST(SystemCreateOne, SystemCreateDestroy)
{
	updatedSuccess = true;

 	World w;
 	CHECK_ZERO(w.systems.Size());
 
 	UnitTestSystem& s = w.CreateSystem<UnitTestSystem>(name, _UnitData, _UnitStats);
	CHECK_REF(s);
 	CHECK_EQ(w.systems.Size(), 1);
 	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
 	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<UnitTestSystem>());

	CHECK_FALSE(updatedSuccess);
	CHECK_TRUE(s.UnitTestUpdate());
}

TEST(SystemCreateDestroyOne, SystemCreateDestroy)
{
	updatedSuccess = true;

	World w;
	CHECK_ZERO(w.systems.Size());

	UnitTestSystem& s = w.CreateSystem<UnitTestSystem>(name, _UnitData, _UnitStats);
	CHECK_REF(s);
	CHECK_EQ(w.systems.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld.Size(), 1);
	CHECK_EQ(w.systemTypesInWorld[0], GetSystemTypeFor<UnitTestSystem>());

	CHECK_FALSE(updatedSuccess);
	CHECK_TRUE(s.UnitTestUpdate());

	w.DestroySystem<UnitTestSystem>();
	CHECK_EQ(w.systems.Size(), 0);
	CHECK_EQ(w.systemTypesInWorld.Size(), 0);


	CHECK_FALSE(updatedSuccess)
}