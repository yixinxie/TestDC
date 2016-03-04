#include "VoxelChunkTransitionSurfaceDesc.h"
#include "VoxelChunk.h"
VoxelChunkTransitionSurfaceDesc::VoxelChunkTransitionSurfaceDesc() : indexMap(nullptr), baseIndexMap(nullptr), seamEdges(nullptr){
	initialized = false;
}
VoxelChunkTransitionSurfaceDesc::~VoxelChunkTransitionSurfaceDesc(){
	if(indexMap != nullptr)delete indexMap;
	if (baseIndexMap != nullptr)delete baseIndexMap;
	if (seamEdges != nullptr)delete seamEdges;
}
//
void VoxelChunkTransitionSurfaceDesc::init(int _lodDiff, int type){
	
	initialized = true;
	if (_lodDiff == -1){
		// if adjChunk covers more volume than this(less detailed),
		dimInCells = VoxelConstants::UsableRange * 2;
		vertScale = 0.5f;

	}
	else if (_lodDiff == 0){
		dimInCells = VoxelConstants::UsableRange;
		vertScale = 1;
	}
	else if (_lodDiff == 1){
		dimInCells = VoxelConstants::UsableRange;
		vertScale = 2;
	}
	else{
		printf_s("irregular lod adjacency detected!\n");
	}
	
	if (indexMap == nullptr){
		/* we build the index array on the edge desc to be twice in dimension, or four times as large as one slice
		in the original structure in VoxelChunk.
		This is to ensure we have enough storage for all transition scenarios.
		*/
		indexMap = new int[dimInCells * dimInCells * 2];
		memset(indexMap, -1, dimInCells * dimInCells * 2 * sizeof(int));
	}
	int len = flagNumPerCell * dimInCells* dimInCells;
	seamEdges = new char[len];
	memset(seamEdges, -1, len);
}
void VoxelChunkTransitionSurfaceDesc::init1D(int _lodDiff){
	dimInCells = VoxelConstants::UsableRange * 2;
	initialized = true;
	if (_lodDiff == -1){
		// if adjChunk covers more volume than this(less detailed),
		vertScale = 0.5f;
	}
	else if (_lodDiff == 0){
		vertScale = 1;
	}
	else if (_lodDiff == 1){
		vertScale = 2;
	}

	if (baseIndexMap == nullptr){
		baseIndexMap = new int[VoxelConstants::UsableRange];
		memset(baseIndexMap, -1, VoxelConstants::UsableRange * sizeof(int));
	}
	if (indexMap == nullptr){
		indexMap = new int[dimInCells * 3];
		memset(indexMap, -1, dimInCells * 3 * sizeof(int));
	}
	
	int len = flagNumPerCell * dimInCells;
	seamEdges = new char[len];
	memset(seamEdges, -1, len);
}
void VoxelChunkTransitionSurfaceDesc::gen2DUni2(std::vector<unsigned int>* tempIndices, bool inverted){
	for (int y = 1; y < dimInCells; y++){
		for (int x = 0; x < dimInCells; x++){
			// first read the vertex index from the original index table.
			int ind0 = readIndex2D(x, y, 0);
			char edgeA = getEdgeFlagA(x, y);
			if (edgeA != -1)
			{
				int ind3 = readIndex2D(x, y - 1, 0);
				int ind1 = readIndex2D(x, y, 1);
				int ind2 = readIndex2D(x, y - 1, 1);
				windQuad(edgeA, ind0, ind1, ind2, ind3, tempIndices, inverted);
				
			}
			char edgeB = getEdgeFlagB(x, y);
			if (edgeB != -1)
			{
				int ind3 = readIndex2D(x - 1, y, 0);
				int ind1 = readIndex2D(x, y, 1);
				int ind2 = readIndex2D(x - 1, y, 1);

				windQuad(edgeB, ind0, ind1, ind2, ind3, tempIndices, !inverted);
			}
		}
	}
}
void VoxelChunkTransitionSurfaceDesc::windQuad(int edge, int ind0, int ind1, int ind2, int ind3, std::vector<unsigned int>* tempIndices, bool inverted){
	char cond0 = (inverted) ? 1 : 0;
	char cond1 = (inverted) ? 0 : 1;
	if (edge == cond0){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind2);
		tempIndices->push_back(ind1);
	}
	else if (edge == cond1){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind3);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind2);
	}
}