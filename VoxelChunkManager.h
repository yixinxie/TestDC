#pragma once
#include "IVoxelChunkManager.h"
#include <glm/glm.hpp>
#include "VCNode.h"
class VoxelChunkManager : public IVoxelChunkManager{
private:

public:
	VoxelChunkManager(void);
	void createChunk(const ivec3& pos, const int size, VCNode* node);
	void removeChunk(VCNode* chunk);
	void relocateChunk(VCNode* chunk, const ivec3& pos);
};