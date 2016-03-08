#include "VoxelChunk.h"
#pragma once
struct VCNode
{
	char activated;
	ivec3 pos;
	VoxelChunk* chunk;
	VCNode(void) : chunk(nullptr), activated(0){};
};