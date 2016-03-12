#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "VoxelConstants.h"
using namespace glm;
/*
this class contains the data that describe how a chunk should generate vertices
to cover up the seam between chunks.
a voxel chunk should have 6 of this.
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
	// 1D
	inline char getEdgeFlag(int x){
		return seamEdges[x];
	}
	inline int calcIndex(int x, int y){
		return x + y * dimInCells;
	}
	inline int readIndex1D(int x, int surfaceId){
		return indexMap[x * 2 + surfaceId];
	}
	void windQuad(int edge, int index0, int index1, int index2, int index3, std::vector<unsigned int>* tempIndices, bool inverted);

	/* this index map holds the indices in the neighbouring chunk's related data slice.
	in the case of a 1D structure, this also holds the data columns of the two adjacent 2D structures.
	*/
	int* indexMap;
	char* seamEdges;
public:
	//
	//inline void writeIndex(int x, int index){
	//	indexMap[x] = index;
	//}
	inline char readSeamEdge(int x, int y){
		return seamEdges[calcIndex(x, y)];
	}
	inline void writeSeamEdge1D(int x, char valX){
		seamEdges[x] = valX;
	}
	///////////// NEW ATTEMPT
	/*
	this function writes the index value to the index map.
	surface (0) indicates it should write to the base index slice.
	surface (1) indicates the neighbour index slice.
	*/
	// 2D
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
	// 1D
	/*	columnId(0): the base index column
		columnId(1): the index column from the left surface
		columnId(2): the index column from the right surface
		columnId(3): the adjacent index column

	*/
	inline void writeIndex1D(int x, int columnId, int index){
		indexMap[x * 4 + columnId] = index;
	}
	
	int getDim(void);

	float vertScale;
	bool initialized;
	VoxelChunkTransitionSurfaceDesc(void);
	~VoxelChunkTransitionSurfaceDesc();

	void init(int _lodDiff);
	void init1D(int _lodDiff, VoxelChunkTransitionSurfaceDesc* left, VoxelChunkTransitionSurfaceDesc* right);

	void gen2D(std::vector<unsigned int>* tempIndices, bool inverted);

	// this generate quad indices from a 1D transition descriptor
	void gen1D(std::vector<unsigned int>* tempIndices, bool inverted);

};
