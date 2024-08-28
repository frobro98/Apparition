// Copyright 2020, Nathan Blane

#include "Framework/UnitTest.h"

#include "ECS/World.hpp"

#include "TestComponents/FloatArray.hpp"
#include "TestComponents/Movement.hpp"
#include "TestComponents/Position.hpp"
#include "TestComponents/Rotation.hpp"

using namespace Musa;

TEST(WorldCreate, WorldCreation)
{
	World w;

	CHECK_ZERO(w.archetypes.Size());
	CHECK_ZERO(w.entityBridges.Size());
	CHECK_ZERO(w.deadIndices.Size());
	CHECK_ZERO(w.archetypesByHash.size());
	CHECK_ZERO(w.archetypeHashIDs.Size());
}

