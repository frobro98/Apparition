// Copyright 2020, Nathan Blane

#pragma once

//#include "ECS/Component.hpp"

struct InputContext;

// Input comes into ECS and gets processed. What does that processing look like?
// Goes through all of the events that exist this frame. If an event will be consumed
// by the game, then it gets processed. But what tells an event to get processed? Currently
// the way that input is told to be consumed is through InputContext data. However, there could
// be multiple objects requesting input, so potentially multiple InputContext objects.
// 
// The main thing that needs to be supported is having all information stored within some 
// component so it can be used by the game later. It gets processed once, for everything
// that needs input in the entire game.

// Wherever the information is stored, all information except for state is cleaned up before
// processing begins. 

// GameInput should contain all it needs to, which would be the context it will be reading in, 
// and the storage for all of the states and actions that correspond to the context.

//struct InputContextComponent : Musa::Component
//{
//
//	// Associated InputContext
//	InputContext* inputConext;
//};
