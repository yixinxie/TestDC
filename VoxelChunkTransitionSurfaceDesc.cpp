#include "VoxelChunkTransitionSurfaceDesc.h"
#include "VoxelChunk.h"
VoxelChunkTransitionSurfaceDesc::VoxelChunkTransitionSurfaceDesc() : indexMap(nullptr), seamEdges(nullptr){
	initialized = false;
}
VoxelChunkTransitionSurfaceDesc::~VoxelChunkTransitionSurfaceDesc(){
	if (indexMap != nullptr){
		delete indexMap;
	}
	if (seamEdges != nullptr){
		delete seamEdges;	
	}
}
//
void VoxelChunkTransitionSurfaceDesc::init(int _lodDiff){
	
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
	// two 2D arrays to store the indices. the first slice is for the base chunk.
	// the second slice is for the neighbouring chunk.
	if (indexMap == nullptr){
		indexMap = new int[dimInCells * dimInCells * 2];
		memset(indexMap, -1, dimInCells * dimInCells * 2 * sizeof(int));
	}
	int len = flagNumPerCell * dimInCells* dimInCells;
	seamEdges = new char[len];
	memset(seamEdges, -1, len);
}
void VoxelChunkTransitionSurfaceDesc::init1D(int _lodDiff, VoxelChunkTransitionSurfaceDesc* left, VoxelChunkTransitionSurfaceDesc* right){
	initialized = true;
	
	if (_lodDiff == -1){
		// if adjChunk covers more volume than this(less detailed),
		vertScale = 0.5f;
		dimInCells = VoxelConstants::UsableRange * 2;
	}
	else if (_lodDiff == 0){
		vertScale = 1;
		dimInCells = VoxelConstants::UsableRange;
	}
	else if (_lodDiff == 1){
		vertScale = 2;
		dimInCells = VoxelConstants::UsableRange;
	}
	if (left->dimInCells > dimInCells)dimInCells = left->dimInCells;
	if (right->dimInCells > dimInCells)dimInCells = right->dimInCells;

	// four 1D arrays to store the indices, for the base chunk, the two adjacent neighbouring chunks,
	// and the diagonally adjacent chunk.
	if (indexMap == nullptr){
		indexMap = new int[dimInCells * 4];
		memset(indexMap, -1, dimInCells * 4 * sizeof(int));
	}
	// the flag num per cell is different from init2D(init) because the each hinge cell only need 1 edge flag.
	int len = 1 * dimInCells;
	seamEdges = new char[len];
	memset(seamEdges, -1, len);
}
void VoxelChunkTransitionSurfaceDesc::gen2D(std::vector<unsigned int>* tempIndices, bool inverted){
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

				//if (ind0 < 0 || ind1 < 0 || ind2 < 0 || ind3 < 0){
				//	int sdf = 0;
				//}
				windQuad(edgeA, ind0, ind1, ind2, ind3, tempIndices, inverted);
				
			}
			/*char edgeB = getEdgeFlagB(x, y);
			if (edgeB != -1)
			{
				int ind3 = readIndex2D(x - 1, y, 0);
				int ind1 = readIndex2D(x, y, 1);
				int ind2 = readIndex2D(x - 1, y, 1);

				windQuad(edgeB, ind0, ind1, ind2, ind3, tempIndices, !inverted);
			}*/
		}
	}

	for (int y = 0; y < dimInCells; y++){
		for (int x = 1; x < dimInCells; x++){
			// first read the vertex index from the original index table.
			int ind0 = readIndex2D(x, y, 0);
			
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
void VoxelChunkTransitionSurfaceDesc::gen1D(std::vector<unsigned int>* tempIndices, bool inverted){
	for (int x = 0; x < dimInCells; x++){
		// first read the vertex index from the original index table.
		int ind0 = readIndex1D(x, 0);
		char edge = getEdgeFlag(x);
		if (edge != -1)
		{
			int ind3 = readIndex1D(x, 3);
			int ind1 = readIndex1D(x, 1);
			int ind2 = readIndex1D(x, 2);
			windQuad(edge, ind0, ind1, ind2, ind3, tempIndices, inverted);
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
int VoxelChunkTransitionSurfaceDesc::getDim(){
	return dimInCells;
}
// maxX is either equal to dimInCells, or twice as large.
// columnId is either 0 or 1.
int VoxelChunkTransitionSurfaceDesc::readIndex2D_X(int x, int maxX){
	int ret;

	if (maxX > dimInCells){
		int halfX = x >> 1;
		ret = readIndex2D(halfX, dimInCells - 1, 1);
	}
	else{
		ret = readIndex2D(x, dimInCells - 1, 1);
	}
	return ret;
}
int VoxelChunkTransitionSurfaceDesc::readIndex2D_Y(int y, int maxY){
	int ret;

	if (maxY> dimInCells){
		int halfY = y >> 1;
		ret = readIndex2D(dimInCells - 1, halfY, 1);
	}
	else{
		ret = readIndex2D(dimInCells - 1, y, 1);
	}
	return ret;
}
void VoxelChunkTransitionSurfaceDesc::writeIndex1D_Dupe2(int x, int maxX, int columnId, int index){
	int scaler = (dimInCells / maxX) - 1;
	indexMap[x * 4 + columnId] = index;
	indexMap[(x + scaler) * 4 + columnId] = index;
}

void VoxelChunkTransitionSurfaceDesc::writeIndex1D_Dupe(int x, int columnId, int index){
	int scaler = (dimInCells / VoxelConstants::UsableRange) - 1;
	indexMap[x * 4 + columnId] = index;
	indexMap[(x + scaler)* 4 + columnId] = index;

}