// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"

#include "ECS/World.hpp"
#include "ECS/Archetype.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"

using namespace Musa;

TEST(EntityAddOneRemoveOne, EntityRemoveComponent)
{
	World w;

	Entity e = w.CreateEntity<Position>();
	CHECK_TRUE(w.HasComponent<Position>(e));

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* posArchType = GetOrCreateArchetypeFrom<Position>(w);
	CHECK_PTR(posArchType);
	CHECK_EQ(w.archetypes.Size(), 1);

	CHECK_EQ(posArchType->types.Size(), 1);
	CHECK_EQ(posArchType->types[0], GetComponentTypeFor<Position>());

	w.RemoveComponent<Position>(e);

	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* nullArchetype = GetOrCreateArchetypeFrom(w);
	CHECK_PTR(nullArchetype);
	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(nullArchetype->types.Size(), 0);

	CHECK_FALSE(w.HasComponent<Position>(e));
}

TEST(EntityAddTwoRemoveOne, EntityRemoveComponent)
{
	World w;

	Entity e = w.CreateEntity<Position>();
	CHECK_TRUE(w.HasComponent<Position>(e));

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* posArchType = GetOrCreateArchetypeFrom<Position>(w);
	CHECK_PTR(posArchType);
	CHECK_EQ(w.archetypes.Size(), 1);

	CHECK_EQ(posArchType->types.Size(), 1);
	CHECK_EQ(posArchType->types[0], GetComponentTypeFor<Position>());

	w.AddComponentTo<Rotation>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));

	Archetype* twoType = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_PTR(twoType);
	CHECK_EQ(w.archetypes.Size(), 2);

	CHECK_EQ(twoType->types.Size(), 2);

	w.RemoveComponent<Position>(e);
	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));

	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(w.entityBridges.Size(), 1);
	
	Archetype* rotType = GetOrCreateArchetypeFrom<Rotation>(w);
	CHECK_PTR(rotType);
	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(rotType->types.Size(), 1);
}

TEST(EntityAddThreeRemoveOne, EntityRemoveComponent)
{
	World w;

	// Create Entity with Position Component
	Entity e = w.CreateEntity<Position>();
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* posArchType = GetOrCreateArchetypeFrom<Position>(w);
	CHECK_PTR(posArchType);
	CHECK_EQ(w.archetypes.Size(), 1);

	CHECK_EQ(posArchType->types.Size(), 1);
	CHECK_EQ(posArchType->types[0], GetComponentTypeFor<Position>());

	// Add Rotation
	w.AddComponentTo<Rotation>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));

	Archetype* twoType = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_PTR(twoType);
	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(twoType->types.Size(), 2);

	// Add Movement
	w.AddComponentTo<Movement>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	Archetype* threeType = GetOrCreateArchetypeFrom<Rotation, Position, Movement>(w);
	CHECK_PTR(threeType);
	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(threeType->types.Size(), 3);

	// Remove Position
	w.RemoveComponent<Position>(e);
	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* rotMoveType = GetOrCreateArchetypeFrom<Movement, Rotation>(w);
	CHECK_PTR(rotMoveType);
	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(rotMoveType->types.Size(), 2);
}

TEST(EntityAddThreeRemoveTwo, EntityRemoveComponent)
{
	World w;

	// Create Entity with Position Component
	Entity e = w.CreateEntity<Position>();
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* posArchType = GetOrCreateArchetypeFrom<Position>(w);
	CHECK_PTR(posArchType);
	CHECK_EQ(w.archetypes.Size(), 1);

	CHECK_EQ(posArchType->types.Size(), 1);
	CHECK_EQ(posArchType->types[0], GetComponentTypeFor<Position>());

	// Add Rotation
	w.AddComponentTo<Rotation>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));

	Archetype* twoType = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_PTR(twoType);
	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(twoType->types.Size(), 2);

	// Add Movement
	w.AddComponentTo<Movement>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	Archetype* threeType = GetOrCreateArchetypeFrom<Rotation, Position, Movement>(w);
	CHECK_PTR(threeType);
	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(threeType->types.Size(), 3);

	// Remove Rotation
	w.RemoveComponent<Rotation>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* posMoveType = GetOrCreateArchetypeFrom<Movement, Position>(w);
	CHECK_PTR(posMoveType);
	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(posMoveType->types.Size(), 2);

	// Remove Position
	w.RemoveComponent<Position>(e);
	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 5);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* moveType = GetOrCreateArchetypeFrom<Movement>(w);
	CHECK_PTR(moveType);
	CHECK_EQ(w.archetypes.Size(), 5);
	CHECK_EQ(moveType->types.Size(), 1);
}

TEST(EntityAddThreeRemoveThree, EntityRemoveComponent)
{
	World w;

	// Create Entity with Position Component
	Entity e = w.CreateEntity<Position>();
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 1);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* posArchType = GetOrCreateArchetypeFrom<Position>(w);
	CHECK_PTR(posArchType);
	CHECK_EQ(w.archetypes.Size(), 1);

	CHECK_EQ(posArchType->types.Size(), 1);
	CHECK_EQ(posArchType->types[0], GetComponentTypeFor<Position>());

	// Add Rotation
	w.AddComponentTo<Rotation>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));

	Archetype* twoType = GetOrCreateArchetypeFrom<Rotation, Position>(w);
	CHECK_PTR(twoType);
	CHECK_EQ(w.archetypes.Size(), 2);
	CHECK_EQ(twoType->types.Size(), 2);

	// Add Movement
	w.AddComponentTo<Movement>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_TRUE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	Archetype* threeType = GetOrCreateArchetypeFrom<Rotation, Position, Movement>(w);
	CHECK_PTR(threeType);
	CHECK_EQ(w.archetypes.Size(), 3);
	CHECK_EQ(threeType->types.Size(), 3);

	// Remove Rotation
	w.RemoveComponent<Rotation>(e);
	CHECK_TRUE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* posMoveType = GetOrCreateArchetypeFrom<Movement, Position>(w);
	CHECK_PTR(posMoveType);
	CHECK_EQ(w.archetypes.Size(), 4);
	CHECK_EQ(posMoveType->types.Size(), 2);

	// Remove Position
	w.RemoveComponent<Position>(e);
	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_TRUE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 5);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* moveType = GetOrCreateArchetypeFrom<Movement>(w);
	CHECK_PTR(moveType);
	CHECK_EQ(w.archetypes.Size(), 5);
	CHECK_EQ(moveType->types.Size(), 1);

	// Remove Movement
	w.RemoveComponent<Movement>(e);
	CHECK_FALSE(w.HasComponent<Position>(e));
	CHECK_FALSE(w.HasComponent<Rotation>(e));
	CHECK_FALSE(w.HasComponent<Movement>(e));

	CHECK_EQ(w.archetypes.Size(), 6);
	CHECK_EQ(w.entityBridges.Size(), 1);

	Archetype* nullType = GetOrCreateArchetypeFrom(w);
	CHECK_PTR(nullType);
	CHECK_EQ(w.archetypes.Size(), 6);
	CHECK_EQ(nullType->types.Size(), 0);
}

