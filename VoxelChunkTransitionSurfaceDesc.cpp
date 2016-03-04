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
		dimInCells = VoxelConstants::UsableRange * 2;
		vertScale = 1;
	}
	else if (_lodDiff == 1){
		dimInCells = VoxelConstants::UsableRange * 2;
		vertScale = 2;
	}
	else{
		printf_s("irregular lod adjacency detected!\n");
	}
	
	if (baseIndexMap == nullptr){
		// regardless of the lod relationship, the adjacent slice should always be of its original size.
		baseIndexMap = new int[VoxelConstants::UsableRange * VoxelConstants::UsableRange];
		memset(baseIndexMap, -1, VoxelConstants::UsableRange * VoxelConstants::UsableRange * sizeof(int));
	}
	if (indexMap == nullptr){
		/* we build the index array on the edge desc to be twice in dimension, or four times as large as one slice
		in the original structure in VoxelChunk.
		This is to ensure we have enough storage for all transition scenarios.
		*/
		indexMap = new int[dimInCells * dimInCells];
		memset(indexMap, -1, dimInCells * dimInCells * sizeof(int));
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

void VoxelChunkTransitionSurfaceDesc::gen2D_x_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted){
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
void VoxelChunkTransitionSurfaceDesc::gen2D_x_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted){
	
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


void VoxelChunkTransitionSurfaceDesc::gen2D_y_tri(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, bool inverted){
	int ind1 = getVertexIndex(x, y);
	int ind2 = getVertexIndex(x - 1, y);

	char edgeB = getEdgeFlagB(x, y);
	char cond0 = (inverted) ? 1 : 0;
	char cond1 = (inverted) ? 0 : 1;
	if (edgeB == cond0){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind2);
	}
	else if (edgeB == cond1){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind2);
		tempIndices->push_back(ind1);

	}
}
void VoxelChunkTransitionSurfaceDesc::gen2D_y_quad(int x, int y, std::vector<unsigned int>* tempIndices, int ind0, int ind3, bool inverted){

	int ind1 = getVertexIndex(x, y);
	int ind2 = getVertexIndex(x - 1, y);
	char edgeB = getEdgeFlagB(x, y);
	char cond0 = (inverted) ? 1 : 0;
	char cond1 = (inverted) ? 0 : 1;
	if (edgeB == cond0){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind3);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);
		tempIndices->push_back(ind2);
	}
	else if (edgeB == cond1){
		tempIndices->push_back(ind0);
		tempIndices->push_back(ind3);
		tempIndices->push_back(ind1);

		tempIndices->push_back(ind3);
		tempIndices->push_back(ind2);
		tempIndices->push_back(ind1);
	}
}
void VoxelChunkTransitionSurfaceDesc::gen2DUni(std::vector<unsigned int>* tempIndices, bool inverted){
	for (int y = 1; y < dimInCells; y++){
		for (int x = 0; x < dimInCells; x++){
			int baseX = x >> 1, baseY = y >> 1;
			// first read the vertex index from the original index table.
			int ind0 = baseIndexMap[baseX + baseY * VoxelConstants::UsableRange];

			if (y % 2 == 1){
				gen2D_x_tri(x, y, tempIndices, ind0, inverted);
			}
			else{
				int ind3 = baseIndexMap[baseX + (baseY - 1) * VoxelConstants::UsableRange];
				gen2D_x_quad(x, y, tempIndices, ind0, ind3, inverted);
			}
		}
	}
	//???
	for (int y = 0; y < dimInCells; y++){
		for (int x = 1; x < dimInCells; x++){
			int baseX = x >> 1, baseY = y >> 1;
			if (x == 1 && y == 3){
				int sdf = 0;
			}
			// first read the vertex index from the original index table.
			int ind0 = baseIndexMap[baseX + baseY * VoxelConstants::UsableRange];

			if (x % 2 == 1){
				gen2D_y_tri(x, y, tempIndices, ind0, inverted);
			}
			else{
				int ind3 = baseIndexMap[baseX - 1 + (baseY) * VoxelConstants::UsableRange];
				gen2D_y_quad(x, y, tempIndices, ind0, ind3, inverted);
			}
		}
	}
}