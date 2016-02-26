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
public:
	// possible values are 0, 1 for each face.
	int faceX, faceY, faceZ;
	int* indexMap;
	/*
	level of detail difference,
	0: the neighbouring chunk has less details, covers 2x volume;
	1: on the same lod;
	2: the neighbouring chunk has more details, covers 0.5x volume.
	*/
	int lodDiff;
	std::vector<vec3> seamVertices;
	std::vector<vec3> seamNormals; // normals on the vertices.
	VoxelChunkEdgeDesc(void);
	~VoxelChunkEdgeDesc();

	void init(int faceId, int lodd);
};