#pragma once
#include "VoxelChunk.h"
#include <glm/glm.hpp>

#define RING_DIM 6
#define INNDER_DIM 3
// 3 + 1 + 3
#define LEDGE_DIM 3
#define CORE_DIM 6
struct VCNode
{
	char activated;
	ivec3 pos;
	VoxelChunk* chunk;
	VCNode(void) : chunk(nullptr), activated(0){};
};
class ClipmapRing{
private:
	int lod;
	int unitSize;
	
	ivec3 innerPos;
	ivec3 start;
	VCNode ring[RING_DIM * RING_DIM * RING_DIM];
	inline int calcRingIndex(int x, int y, int z){
		return x + y * RING_DIM + z * RING_DIM * RING_DIM;
	}
	bool belongsTo(const ivec3& pos, const ivec3& oldOrigin, const ivec3& inner);
public:
	ClipmapRing(void);
	~ClipmapRing();
	void initPos(ivec3 _pos, ivec3 _innerPos, int _lod);
	void reposition(const ivec3& _pos, const ivec3& _innerPos);
};
class ClipmapCore{
private:
	int lod;
	VCNode core[CORE_DIM * CORE_DIM * CORE_DIM];
public:
	ClipmapCore(void){}
	~ClipmapCore();
	void initPos(ivec3 _pos);
	
};
