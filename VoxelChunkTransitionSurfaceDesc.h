#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "VoxelConstants.h"
using namespace glm;
/*
this class contains the data that describe how a chunk should generate vertices
to cover up the seam with a neighbour chunk.
a regular voxel chunk should have 7 of this.
*/

class VoxelChunkTransitionSurfaceDesc{
private:
	const int flagNumPerCell = 2;

	// length of an axis in number of cells. this takes lod into account.
	int dimInCells;

	void gen2D_x_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted);
	void gen2D_x_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted);

	void gen2D_y_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted);
	void gen2D_y_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted);
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
	inline int calcIndex(int x, int y){
		return x + y * dimInCells;
	}
	int* baseIndexMap;
	int* indexMap;
	char* seamEdges;
public:
	/* this index map holds the indices in the neighbouring chunk's related data slice.
		in the case of a 1D structure, this also holds the data columns of the two adjacent 2D structures.
	*/
	int validRange;
	
	/* this index map mirrors the slice in the adjacent chunk.
	*/
	
	inline void writeAdjIndex(int x, int loc, int index){
		indexMap[x * 3 + loc] = index;
	}
	inline void writeBaseIndex(int x, int index){
		baseIndexMap[x] = index;
	}
	inline void writeBaseIndex2D(int x, int y, int index){
		baseIndexMap[x + y * VoxelConstants::UsableRange] = index;
	}

	inline int readIndex2D(int x, int y){
		return indexMap[calcIndex(x,y)];
	}
	inline int readIndex(int x){
		return indexMap[x];
	}
	inline void writeIndex(int x, int index){
		indexMap[x] = index;
	}
	inline void writeIndex2D(int x, int y, int index){
		indexMap[calcIndex(x, y)] = index;
	}
	inline void writeSeamEdgeX(int x, char valX){
		seamEdges[x * 2] = valX;
	}
	inline void writeSeamEdgeXY(int x, int y, char valX, char valY){
		int idx = calcIndex(x, y);
		seamEdges[idx * 2] = valX;
		seamEdges[idx * 2 + 1] = valY;
	}
	inline char readSeamEdge(int x, int y){
		return seamEdges[calcIndex(x, y)];
	}

	float vertScale;
	bool initialized;
	VoxelChunkTransitionSurfaceDesc(void);
	~VoxelChunkTransitionSurfaceDesc();

	void init(int lodDiff, int type);
	void init1D(int lodDiff);

	// new experiment!
	void gen2DUni(std::vector<unsigned int>* tempIndices, bool inverted);

};
