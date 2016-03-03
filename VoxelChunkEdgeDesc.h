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
	int* baseIndexMap;
	/*
	level of detail difference,
	0: the neighbouring chunk has less details, covers 2x volume;
	1: on the same lod;
	2: the neighbouring chunk has more details, covers 0.5x volume.
	*/
	inline int calcIndex(int x, int y){
		return x + y * dimInCells;
	}
	inline char getEdgeFlagA(int x, int y){
		int edgeCellIndex = calcIndex(x, y);
		return seamEdges[edgeCellIndex * 2];
	}
	inline char getEdgeFlagB(int x, int y){
		int edgeCellIndex = calcIndex(x, y);
		return seamEdges[edgeCellIndex * 2 + 1];
	}
	inline char getVertexIndex(int x, int y){
		int edgeCellIndex = calcIndex(x, y);
		return indexMap[edgeCellIndex];
	}
	inline int getDim(void){ return dimInCells; }

	float vertScale;
	int lodDiff;
	VoxelChunkEdgeDesc(void);
	~VoxelChunkEdgeDesc();

	void init(int thisLod, int adjLod);

	void gen2D_x_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted);
	void gen2D_x_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted);

	void gen2D_y_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted);
	void gen2D_y_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted);
	// new experiment!
	

};