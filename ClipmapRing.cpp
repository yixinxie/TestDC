#include "ClipmapRing.h"
ClipmapRing::ClipmapRing(IVoxelChunkManager* _manager){
	vcManager = _manager;
}
ClipmapRing::~ClipmapRing(){

}
void ClipmapRing::initPos(ivec3 _pos, ivec3 _innerPos, int _lod){
	lod = _lod;
	unitSize = 1 << lod;
	noInner = false;
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){
				int idx = calcRingIndex(x, y, z);
				ivec3 thisPos = _pos + ivec3(x, y, z) * unitSize;
				ring[idx].pos = thisPos;
				if (belongsTo(thisPos, _pos, _innerPos)){
					ring[idx].activated = 1;
					vcManager->createChunk(ring[idx].pos, lod, &ring[idx]);
				}

			}
		}
	}
	originPos = _pos;
	innerCubePos = _innerPos;
	start = ivec3(0, 0, 0);
}
void ClipmapRing::initPos(ivec3 _pos, int _lod){
	lod = _lod;
	unitSize = 1 << lod;
	noInner = true;
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){
				int idx = calcRingIndex(x, y, z);
				ivec3 thisPos = _pos + ivec3(x, y, z) * unitSize;
				ring[idx].pos = thisPos;
				if (belongsToNoInner(thisPos, _pos)){
					ring[idx].activated = 1;
					vcManager->createChunk(ring[idx].pos, lod, &ring[idx]);
				}

			}
		}
	}
	originPos = _pos;
	start = ivec3(0, 0, 0);
}
void ClipmapRing::reposition(const ivec3& _pos, const ivec3& _innerPos){
	
	ivec3 origin = ring[calcRingIndex(start.x,start.y, start.z)].pos;
	ivec3 diff = _pos - origin;
	// we expect the values in diff should be either zero, or unitSize
	ivec3 normalizedDiff = diff / unitSize;
	start += normalizedDiff;
	ivec3 start = ivec3_mod(start, RING_DIM);

	//int z = 0; {
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){

				ivec3 thisPos = _pos + ivec3(x, y, z) * unitSize;
				// find the corresponding node.
				ivec3 nxyz = ivec3_mod(ivec3(x + start.x, y + start.y, z + start.z), RING_DIM);
				int idx = calcRingIndex(nxyz.x, nxyz.y, nxyz.z);
				ivec3 posFromNode = ring[idx].pos;
				bool inRing;
				if (noInner)inRing = belongsToNoInner(thisPos, _pos);
				else inRing = belongsTo(thisPos, _pos, _innerPos);
				if (inRing){
					if (ring[idx].activated == 1){
						if (posFromNode == thisPos){
							// keep
							printf_s("keep %d, %d, %d\n", thisPos.x, thisPos.y, thisPos.z);
						}
						else{
							// remove and regenerate
							printf_s("remove %d, %d, %d === generate %d, %d, %d\n", posFromNode.x, posFromNode.y, posFromNode.z, thisPos.x, thisPos.y, thisPos.z);
							ring[idx].pos = thisPos;
							vcManager->relocateChunk(&ring[idx], thisPos);
						}
					}
					else{
						// first allocation
						printf_s("claim %d, %d, %d\n", thisPos.x, thisPos.y, thisPos.z);
						ring[idx].activated = 1;
						ring[idx].pos = thisPos;
						vcManager->createChunk(thisPos, lod, &ring[idx]);
					}
				}
				else{
					if (ring[idx].activated == 1){
						printf_s("release %d, %d, %d\n", thisPos.x, thisPos.y, thisPos.z);
						ring[idx].activated = 0;
						vcManager->removeChunk(&ring[idx]);
					}
				}
			}
		}
	}
}
void ClipmapRing::update(const ivec3& _pos, const ivec3& _innerPos){
	reposition(_pos, _innerPos);
	originPos = _pos;
	innerCubePos = _innerPos;
}

void ClipmapRing::update(const ivec3& _pos){
	reposition(_pos, ivec3(0, 0, 0));
	originPos = _pos;
}
bool ClipmapRing::belongsTo(const ivec3& pos, const ivec3& origin, const ivec3& inner){
	bool insideCube = false;
	bool insideInnerCube = false;
	if (pos.x >= origin.x  && pos.x < origin.x + unitSize * RING_DIM &&
		pos.y >= origin.y  && pos.y < origin.y + unitSize * RING_DIM &&
		pos.z >= origin.z  && pos.z < origin.z + unitSize * RING_DIM){
		insideCube = true;
	}

	if (pos.x >= inner.x  && pos.x < inner.x + unitSize * INNDER_DIM &&
		pos.y >= inner.y  && pos.y < inner.y + unitSize * INNDER_DIM &&
		pos.z >= inner.z  && pos.z < inner.z + unitSize * INNDER_DIM){
		insideInnerCube = true;
	}
	return insideCube == true && insideInnerCube == false;
}
bool ClipmapRing::belongsToNoInner(const ivec3& pos, const ivec3& origin){
	bool insideCube = false;
	if (pos.x >= origin.x  && pos.x < origin.x + unitSize * RING_DIM &&
		pos.y >= origin.y  && pos.y < origin.y + unitSize * RING_DIM &&
		pos.z >= origin.z  && pos.z < origin.z + unitSize * RING_DIM){
		insideCube = true;
	}
	return insideCube == true;
}
ivec3 ClipmapRing::ivec3_mod(const ivec3& val, const int mod){
	ivec3 ret = val;
	ret += mod * 256;
	ret.x = ret.x % mod;
	ret.y = ret.y % mod;
	ret.z = ret.z % mod;
	return ret;

}
void ClipmapRing::createEdgeDescs(ClipmapRing* innerRing, ClipmapRing* outterRing){
	const ivec3 adjacencyInfo[6] = {
		ivec3(-unitSize, 0, 0), ivec3(0, -unitSize, 0), ivec3(0, 0, -unitSize),
		ivec3(0, -unitSize, -unitSize), ivec3(-unitSize, 0, -unitSize), ivec3(-unitSize, -unitSize, 0)
	};
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){
				bool inRing;
				ivec3 thisPos = originPos + ivec3(x, y, z) * unitSize;
				if (noInner)inRing = belongsToNoInner(thisPos, originPos);
				else inRing = belongsTo(thisPos, originPos, innerCubePos);
				if (inRing){
					ivec3 nxyz = ivec3_mod(ivec3(x + start.x, y + start.y, z + start.z), RING_DIM);
					int idx = calcRingIndex(nxyz.x, nxyz.y, nxyz.z);
					VoxelChunk* curChunk = ring[idx].chunk;
					for (int i = 0; i < 6; i++){
						ivec3 adjPos = ring[idx].pos + adjacencyInfo[i];
						int adjType = adjacencyTypes(adjPos);
						if (i < 3){
							if (adjType == 0){
								// a normal 1 to 1 relationship.
								const VCNode* adjNode = getNodeByCoord(adjPos);
								curChunk->createEdgeDesc2D(adjType, 0, 0, adjNode->chunk, i);
							}
							else if(adjType == 1){
								// now there are 4 chunks that are adjacent to curChunk.
								// they lie in the ring inside this clipmap ring.
								VCNode node0, node1, node2, node3;
								innerRing->getNodesFromInner(adjPos, i, &node0, &node1, &node2, &node3);
								curChunk->createEdgeDesc2D(adjType, 0, 0, node0.chunk, i);
								curChunk->createEdgeDesc2D(adjType, 1, 0, node1.chunk, i);
								curChunk->createEdgeDesc2D(adjType, 0, 1, node2.chunk, i);
								curChunk->createEdgeDesc2D(adjType, 1, 1, node3.chunk, i);
							}
							else if (adjType == -1){
								// the adjacent chunk is larger than curChunk.
								// curChunk only has enough data to describe 1 / 4 of its edge desc.
								ivec3 local = ivec3_mod(adjPos, unitSize);
								ivec3 tempAdjPos = adjPos - local;
								const VCNode* adjNode = outterRing->getNodeByCoord(tempAdjPos);
								if ( i == 0)
									curChunk->createEdgeDesc2D(adjType, local.z, local.y, adjNode->chunk, i);
							}
						}
						else{

						}
					}
				}
			}
		}
	}
}
// pos in world space coordinate.
int ClipmapRing::adjacencyTypes(ivec3 pos){
	int res = 0;
	if (pos.x < originPos.x || pos.y < originPos.y || pos.z < originPos.z ||
		pos.x >= originPos.x + RING_DIM * unitSize ||
		pos.y >= originPos.y + RING_DIM * unitSize ||
		pos.z >= originPos.z + RING_DIM * unitSize){
		res = -1; // outside
	}
	if (pos.x >= innerCubePos.x && pos.x < innerCubePos.x + unitSize * INNDER_DIM &&
		pos.y >= innerCubePos.y && pos.y < innerCubePos.y + unitSize * INNDER_DIM &&
		pos.z >= innerCubePos.z && pos.z < innerCubePos.z + unitSize * INNDER_DIM){
		res = 1; // inside
	}
	return res;
}
const VCNode* ClipmapRing::getNodeByCoord(ivec3 pos){
	ivec3 local = (pos - originPos) / unitSize;
	ivec3 nxyz = ivec3_mod(local + start, RING_DIM);
	int idx = calcRingIndex(nxyz.x, nxyz.y, nxyz.z);
	return &ring[idx];
}
// dir being 0, 1 or 2.
void ClipmapRing::getNodesFromInner(ivec3 pos, int dir, VCNode* _node0, VCNode* _node1, VCNode* _node2, VCNode* _node3){
	const ivec3 offsets[] = {
		ivec3(unitSize, 0, 0), ivec3(unitSize, 0, unitSize), ivec3(unitSize, unitSize, 0), ivec3(unitSize, unitSize, unitSize),

		ivec3(0, unitSize, 0), ivec3(unitSize, unitSize, 0), ivec3(0, unitSize, unitSize), ivec3(unitSize, unitSize, unitSize),

		ivec3(0, 0, unitSize), ivec3(unitSize, 0, unitSize), ivec3(0, unitSize, unitSize), ivec3(unitSize, unitSize, unitSize)
	};
	const VCNode* node0 = getNodeByCoord(pos + offsets[dir * 4]);
	const VCNode* node1 = getNodeByCoord(pos + offsets[dir * 4 + 1]);
	const VCNode* node2 = getNodeByCoord(pos + offsets[dir * 4 + 2]);
	const VCNode* node3 = getNodeByCoord(pos + offsets[dir * 4 + 3]);
	_node0->pos = node0->pos;
	_node1->pos = node1->pos;
	_node2->pos = node2->pos;
	_node3->pos = node3->pos;
}