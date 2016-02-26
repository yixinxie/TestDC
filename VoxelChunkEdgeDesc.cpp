#include "VoxelChunkEdgeDesc.h"
#include "VoxelChunk.h"
VoxelChunkEdgeDesc::VoxelChunkEdgeDesc() : indexMap(nullptr), lodDiff(-1){
}
VoxelChunkEdgeDesc::~VoxelChunkEdgeDesc(){
	if(indexMap != nullptr)delete indexMap;
}
void VoxelChunkEdgeDesc::init(int faceId, int lodd){
	if (lodDiff != lodd)
	{
		lodDiff = lodd;
		int mul = 1;
		mul = (lodDiff == 0) ? mul / 2 : mul;
		mul = (lodDiff == 2) ? mul * 2 : mul;
		int dataSize = VoxelChunk::DataRange;
		if (faceId == 1)
			dataSize *= dataSize;

		if (indexMap != nullptr)delete indexMap;
		indexMap = new int[dataSize * mul];
	}
}