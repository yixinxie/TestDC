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

	inline char getEdgeFlagA(int x, int y){
		int edgeCellIndex = calcIndex(x, y);
		return seamEdges[edgeCellIndex * 2];
	}
	inline char getEdgeFlagB(int x, int y){
		int edgeCellIndex = calcIndex(x, y);
		return seamEdges[edgeCellIndex * 2 + 1];
	}
	inline int calcIndex(int x, int y){
		return x + y * dimInCells;
	}
	void windQuad(int edge, int index0, int index1, int index2, int index3, std::vector<unsigned int>* tempIndices, bool inverted);
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
	

	
	inline int readIndex(int x){
		return indexMap[x];
	}

	inline void writeBaseIndex(int x, int index){
		baseIndexMap[x] = index;
	}
	inline void writeBaseIndex2D(int x, int y, int index){
		baseIndexMap[x + y * VoxelConstants::UsableRange] = index;
	}

	inline void writeIndex(int x, int index){
		indexMap[x] = index;
	}
	inline char readSeamEdge(int x, int y){
		return seamEdges[calcIndex(x, y)];
	}
	inline void writeSeamEdgeX(int x, char valX){
		seamEdges[x * 2] = valX;
	}
	///////////// NEW ATTEMPT
	/*
	this function writes the index value to the index map.
	surface (0) indicates it should write to the base index slice.
	surface (1) indicates the neighbour index slice.
	*/
	inline int readIndex2D(int x, int y, int surfaceId){
		return indexMap[calcIndex(x, y) * 2 + surfaceId];
	}
	inline void writeIndex2D(int x, int y, int surfaceId, int index){
		indexMap[calcIndex(x, y) * 2 + surfaceId] = index;
	}
	
	inline void writeSeamEdgeXY(int x, int y, char valX, char valY){
		int idx = calcIndex(x, y);
		seamEdges[idx * 2] = valX;
		seamEdges[idx * 2 + 1] = valY;
	}
	

	float vertScale;
	bool initialized;
	VoxelChunkTransitionSurfaceDesc(void);
	~VoxelChunkTransitionSurfaceDesc();

	void init(int lodDiff, int type);
	void init1D(int lodDiff);

	void gen2DUni(std::vector<unsigned int>* tempIndices, bool inverted);
	void gen2DUni2(std::vector<unsigned int>* tempIndices, bool inverted);

};
