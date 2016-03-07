#include "ClipmapRing.h"
ClipmapRing::ClipmapRing()
{

}
ClipmapRing::~ClipmapRing(){

}
void ClipmapRing::initPos(ivec3 _pos, ivec3 _innerPos, int _lod){
	lod = _lod;
	unitSize = 1 << lod;
	int maxPlane = unitSize * (RING_DIM + 1);
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){
				int idx = calcRingIndex(x, y, z);
				ivec3 thisPos = _pos + ivec3(x, y, z) * unitSize;
				ring[idx].pos = thisPos;
				if (belongsTo(thisPos, _pos, _innerPos)){
					ring[idx].activated = 1;
				}

			}
		}
	}
	start = ivec3(0, 0, 0);
	innerPos = _innerPos;
}
void ClipmapRing::reposition(const ivec3& _pos, const ivec3& _innerPos){
	
	ivec3 origin = ring[calcRingIndex(start.x, start.y, start.z)].pos;
	ivec3 diff = _pos - origin;
	// we expect the values in diff should be either zero, or unitSize
	ivec3 normalizedDiff = diff / unitSize;
	ivec3 newStart = start + normalizedDiff;
	int claimCount = 0;
	int releaseCount = 0;
	//int z = 0; {
	for (int z = 0; z < RING_DIM; z++){
		for (int y = 0; y < RING_DIM; y++){
			for (int x = 0; x < RING_DIM; x++){

				ivec3 thisPos = _pos + ivec3(x, y, z) * unitSize;
				// find the corresponding node.
				int nx = (x + newStart.x) % RING_DIM;
				int ny = (y + newStart.y) % RING_DIM;
				int nz = (z + newStart.z) % RING_DIM;
				int idx = calcRingIndex(nx, ny, nz);
				ivec3 posFromNode = ring[idx].pos;
				if (belongsTo(thisPos, _pos, _innerPos))
				{
					if (ring[idx].activated == 1){
						if (posFromNode == thisPos){
							// keep
							printf_s("keep %d, %d, %d\n", thisPos.x, thisPos.y, thisPos.z);
						}
						else{
							// remove and regenerate
							printf_s("remove %d, %d, %d === generate %d, %d, %d\n", posFromNode.x, posFromNode.y, posFromNode.z, thisPos.x, thisPos.y, thisPos.z);
							ring[idx].pos = thisPos;
						}
					}
					else{
						// first allocation
						printf_s("claim %d, %d, %d\n", thisPos.x, thisPos.y, thisPos.z);
						ring[idx].activated = 1;
						ring[idx].pos = thisPos;
						claimCount++;
					}
				}
				else
				{
					if (ring[idx].activated == 1){
						printf_s("release %d, %d, %d\n", thisPos.x, thisPos.y, thisPos.z);
						ring[idx].activated = 0;
						releaseCount++;
					}
				}
			}
		}
	}
	origin = _pos;
	innerPos = _innerPos;
	start += normalizedDiff;
	if (releaseCount != claimCount)printf_s("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! UNEVEN !!!!!!!!!!!!!!!!!!!!!!!!!!!");
	

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