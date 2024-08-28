// Copyright 2020, Nathan Blane

#pragma once

#include "BasicTypes/Intrinsics.hpp"
#include "Containers/DynamicArray.hpp"

struct Vertex;
struct Face;

void GenerateMeshTangents(DynamicArray<Vertex>& verts, const DynamicArray<Face>& faces);