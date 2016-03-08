#include "ClipmapRing.h"
ClipmapRing::ClipmapRing(IVoxelChunkManager* _manager){
	vcManager = _manager;
}
ClipmapRing::~ClipmapRing(){

}
void ClipmapRing::initPos(ivec3 _pos, ivec3 _innerPos, int _lod){
	lod = _lod;
	unitSize = 1 << lod;
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){
				int idx = calcRingIndex(x, y, z);
				ivec3 thisPos = _pos + ivec3(x, y, z) * unitSize;
				ring[idx].pos = thisPos;
				if (belongsTo(thisPos, _pos, _innerPos)){
					ring[idx].activated = 1;
					vcManager->createChunk(ring[idx].pos, unitSize, &ring[idx]);
				}

			}
		}
	}
	start = ivec3(0, 0, 0);
}
void ClipmapRing::initPos(ivec3 _pos, int _lod){
	lod = _lod;
	unitSize = 1 << lod;
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){
				int idx = calcRingIndex(x, y, z);
				ivec3 thisPos = _pos + ivec3(x, y, z) * unitSize;
				ring[idx].pos = thisPos;
				if (belongsToNoInner(thisPos, _pos)){
					ring[idx].activated = 1;
					vcManager->createChunk(ring[idx].pos, unitSize, &ring[idx]);
				}

			}
		}
	}
	start = ivec3(0, 0, 0);
}
void ClipmapRing::reposition(const ivec3& _pos, const ivec3& _innerPos, bool noInner){
	
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
						vcManager->createChunk(thisPos, unitSize, &ring[idx]);
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
	reposition(_pos, _innerPos, false);
}

void ClipmapRing::update(const ivec3& _pos){
	reposition(_pos, ivec3(0, 0, 0), true);
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