#include "VoxelChunkEdgeDesc.h"
#include "VoxelChunk.h"
VoxelChunkEdgeDesc::VoxelChunkEdgeDesc() : indexMap(nullptr), seamEdges(nullptr), lodDiff(-1){
}
VoxelChunkEdgeDesc::~VoxelChunkEdgeDesc(){
	if(indexMap != nullptr)delete indexMap;
	if (seamEdges != nullptr)delete seamEdges;
}
void VoxelChunkEdgeDesc::init(int thisLod, int adjLod){

	if (thisLod == adjLod - 1){
		dimInCells = VoxelChunk::UsableRange * 2;
		vertScale = 0.5f;
		lodDiff = 2;

	}
	else if (thisLod == adjLod){
		dimInCells = VoxelChunk::UsableRange;
		vertScale = 1;
		lodDiff = 1;
	}
	else if (thisLod == adjLod + 1){
		dimInCells = VoxelChunk::UsableRange / 2;
		vertScale = 2;
		lodDiff = 0;
	}
	else{
		printf_s("irregular lod adjacency detected!\n");
	}

	if (indexMap == nullptr){
		// if adjChunk covers more volume than this(less detailed),
		// we want to make the indexMap twice as large as its original slice.
		indexMap = new int[dimInCells * dimInCells];
		memset(indexMap, 0, dimInCells * dimInCells * sizeof(int));
	}

	const int flagNumPerCell = 2;
	int len = flagNumPerCell * dimInCells* dimInCells;
	seamEdges = new char[len];
	memset(seamEdges, -1, len);
}
