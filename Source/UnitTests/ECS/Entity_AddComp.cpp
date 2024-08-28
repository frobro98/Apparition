// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"

#include "ECS/World.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"

using namespace Musa;

TEST(EntityAddOneComponent, EntityAddComponents)
{
	World w;

	Entity e = w.CreateEntity();

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Position>(e);

	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));
}

TEST(EntityAddTwoComponents, EntityAddComponents)
{
	World w;

	Entity e = w.CreateEntity();

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Rotation>(e);

	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Position>(e);

	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));
}

TEST(EntityAddThreeComponents, EntityAddComponents)
{
	World w;

	Entity e = w.CreateEntity();

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Movement>(e);

	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Position>(e);

	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Rotation>(e);

	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));
}

TEST(EntityAddFourComponents, EntityAddComponents)
{
	World w;

	Entity e = w.CreateEntity();

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_FALSE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<FloatArray>(e);

	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_TRUE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Rotation>(e);

	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));
	CHECK_TRUE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Movement>(e);

	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));
	CHECK_TRUE(w.HasComponent<FloatArray>(e));

	w.AddComponentTo<Position>(e);

	CHECK_EQ(w.archetypes.Size(), 5);
	CHECK_EQ(w.entityBridges.Size(), 1);

	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));
	CHECK_TRUE(w.HasComponent<FloatArray>(e));
}

