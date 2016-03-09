#pragma once
//#include "VoxelChunk.h"
#include "VCNode.h"
#include <glm/glm.hpp>
class IVoxelChunkManager{
public:
	virtual void createChunk(const ivec3& pos, const int lod, VCNode* node) = 0;
	virtual void removeChunk(VCNode* chunk) = 0;
	virtual void relocateChunk(VCNode* chunk, const ivec3& pos) = 0;
};
