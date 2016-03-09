#pragma once
#include <glm/glm.hpp>
#include "VoxelChunk.h"
#include "IVoxelChunkManager.h"
#include "VCNode.h"
#define RING_DIM 6
#define INNDER_DIM 3

class ClipmapRing{
private:
	int lod;
	int unitSize;
	bool noInner;
	// both coordinates are in world space.
	ivec3 originPos, innerCubePos;
	// in local space.
	ivec3 start;
	VCNode ring[RING_DIM * RING_DIM * RING_DIM];
	inline int calcRingIndex(int x, int y, int z){
		return x + y * RING_DIM + z * RING_DIM * RING_DIM;
	}
	bool belongsTo(const ivec3& pos, const ivec3& origin, const ivec3& inner);
	bool belongsToNoInner(const ivec3& pos, const ivec3& origin);
	ivec3 ivec3_mod(const ivec3& val, const int mod);
	void reposition(const ivec3& _pos, const ivec3& _innerPos);
	IVoxelChunkManager* vcManager;

	// adjacency access related methods:
	// this function determines if the adjacent node of a particular position is inside the inner ring,
	// or in the outter ring. pos in world space
	int adjacencyTypes(ivec3 pos);
	// pos in world space
	VoxelChunk* getNodeByCoord(ivec3 pos);
public:
	ClipmapRing(IVoxelChunkManager* _manager);
	~ClipmapRing();
	void initPos(ivec3 _pos, ivec3 _innerPos, int _lod);
	void initPos(ivec3 _pos, int _lod);
	void update(const ivec3& _pos, const ivec3& _innerPos);
	void update(const ivec3& _pos);
	void createEdgeDescs(ClipmapRing* inner, ClipmapRing* outter);
};
