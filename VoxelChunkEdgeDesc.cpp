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