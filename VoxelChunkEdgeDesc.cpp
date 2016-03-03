#include "VoxelChunkEdgeDesc.h"
#include "VoxelChunk.h"
VoxelChunkEdgeDesc::VoxelChunkEdgeDesc() : indexMap(nullptr), baseIndexMap(nullptr), seamEdges(nullptr), lodDiff(-1){
}
VoxelChunkEdgeDesc::~VoxelChunkEdgeDesc(){
	if(indexMap != nullptr)delete indexMap;
	if (baseIndexMap != nullptr)delete baseIndexMap;
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
		memset(indexMap, -1, dimInCells * dimInCells * sizeof(int));
	}
	if (baseIndexMap == nullptr){
		// regardless of the lod relationship, the adjacent slice should always be of its original size.
		baseIndexMap = new int[VoxelChunk::UsableRange * VoxelChunk::UsableRange];
		memset(baseIndexMap, -1, VoxelChunk::UsableRange * VoxelChunk::UsableRange * sizeof(int));
	}

	const int flagNumPerCell = 2;
	int len = flagNumPerCell * dimInCells* dimInCells;
	seamEdges = new char[len];
	memset(seamEdges, -1, len);
}

void VoxelChunkEdgeDesc::gen2D_x_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted){
	// now we read the four vertex indices from the table in edge desc.
	int ind1 = getVertexIndex(x, y);
	int ind2 = getVertexIndex(x, y - 1); // ifs

	char edgeA = getEdgeFlagA(x, y);
	// only need to wind a single triangle.
	char cond0 = (inverted) ? 1 : 0;
	char cond1 = (inverted) ? 0 : 1;
	if (edgeA == cond0){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind2);
		tempIndices->push_back(ind1);
	}
	else if (edgeA == cond1){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind2);
		
	}
}
void VoxelChunkEdgeDesc::gen2D_x_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted){
	
	int ind1 = getVertexIndex(x, y);
	int ind2 = getVertexIndex(x, y - 1);
	char edgeA = getEdgeFlagA(x, y);
	char cond0 = (inverted) ? 1 : 0;
	char cond1 = (inverted) ? 0 : 1;
	if (edgeA == cond1){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind3);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind2);
	}
	else if (edgeA == cond0){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind2);
		tempIndices->push_back(ind1);
		
	}
}


void VoxelChunkEdgeDesc::gen2D_y_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted){
	int ind1 = getVertexIndex(x, y);
	int ind2 = getVertexIndex(x - 1, y);

	char edgeB = getEdgeFlagB(x, y);
	char cond0 = (inverted) ? 1 : 0;
	char cond1 = (inverted) ? 0 : 1;
	if (edgeB == cond0){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind2);
		tempIndices->push_back(ind1);
	}
	else if (edgeB == cond1){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind2);

	}
}
void VoxelChunkEdgeDesc::gen2D_y_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted){

	int ind1 = getVertexIndex(x, y);
	int ind2 = getVertexIndex(x - 1, y);
	char edgeB = getEdgeFlagB(x, y);
	char cond0 = (inverted) ? 1 : 0;
	char cond1 = (inverted) ? 0 : 1;
	if (edgeB == cond0){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind2);
		tempIndices->push_back(ind1);
	}
	else if (edgeB == cond1){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind3);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind2);
	}
}
void VoxelChunkEdgeDesc::gen2DUni(std::vector<unsigned int>* tempIndices){
	for (int y = 1; y < dimInCells; y++){
		for (int x = 0; x < dimInCells; x++){
			int baseX = x >> 1, baseY = y >> 1;
			// first read the vertex index from the original index table.
			int ind0 = baseIndexMap[baseX + baseY * VoxelChunk::UsableRange];

			if (y % 2 == 1){
				gen2D_x_tri(x, y, tempIndices, ind0, false);
			}
			else{
				int ind3 = baseIndexMap[baseX + (baseY - 1) * VoxelChunk::UsableRange];
				gen2D_x_quad(x, y, tempIndices, ind0, ind3, false);
			}
		}
	}
	//???
	for (int y = 0; y < dimInCells; y++){
		for (int x = 1; x < dimInCells; x++){
			int baseX = x >> 1, baseY = y >> 1;
			// first read the vertex index from the original index table.
			int ind0 = baseIndexMap[baseX + baseY * VoxelChunk::UsableRange];

			if (x % 2 == 1){
				gen2D_y_tri(x, y, tempIndices, ind0, false);
			}
			else{
				int ind3 = baseIndexMap[baseX - 1 + (baseY) * VoxelChunk::UsableRange];
				gen2D_y_quad(x, y, tempIndices, ind0, ind3, false);
			}
		}
	}
}