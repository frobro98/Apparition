// Copyright 2020, Nathan Blane

#pragma once

struct ShaderStage
{
	enum Type
	{
		Vertex = 0,
		Fragment = 1,
		Geometry = 2,
		TessalationEval = 3,
		TessalationControl = 4,
		Compute = 5,

		GfxStageCount = 5,
		StageCount = 6,

		MAX_WORD_SIZE = 0x7FFF
	};
};
