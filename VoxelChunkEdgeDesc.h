#pragma once
#include <vector>
#include <glm/glm.hpp>
using namespace glm;
/*
this class contains the data that describe how a chunk should generate vertices
to cover up the seam with a neighbour chunk.
a regular voxel chunk should have 7 of this.
*/
class VoxelChunkEdgeDesc{
private:
	int dimInCells;
public:
	// possible values are 0, 1 for each face.
	int faceX, faceY, faceZ;

	// length of an axis in number of cells. this takes lod into account.
	
	int* indexMap;
	char* seamEdges;
	/*
	level of detail difference,
	0: the neighbouring chunk has less details, covers 2x volume;
	1: on the same lod;
	2: the neighbouring chunk has more details, covers 0.5x volume.
	*/
	float vertScale;
	int lodDiff;
	VoxelChunkEdgeDesc(void);
	~VoxelChunkEdgeDesc();

	void init(int thisLod, int adjLod);
	inline int calcIndex(int x, int y){
		return x + y * dimInCells;
	}
};