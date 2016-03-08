#include "VoxelManager.h"
#include "MeshSerializer.h"
#include "Visualize.h"
#include "VoxelConstants.h"
void VoxelManager::initChunkSize(int _xcount, int _ycount, int _zcount){
	xCount = _xcount;
	yCount = _ycount;
	zCount = _zcount;

	worldBoundX = xCount * VoxelConstants::UsableRange;
	worldBoundY = yCount * VoxelConstants::UsableRange;
	worldBoundZ = zCount * VoxelConstants::UsableRange;
}

void VoxelManager::initWorldSize(int _sizeX, int _sizeY, int _sizeZ){
	worldBoundX = _sizeX;
	worldBoundY = _sizeY;
	worldBoundZ = _sizeZ;

	xCount = worldBoundX / VoxelConstants::UsableRange;
	yCount = worldBoundY / VoxelConstants::UsableRange;
	zCount = worldBoundZ / VoxelConstants::UsableRange;
	if (xCount == 0)xCount = 1;
	if (yCount == 0)yCount = 1;
	if (zCount == 0)zCount = 1;
}
void VoxelManager::write(const VoxelData& vData, const int x, const int y, const int z, const int w){
	int chunkPosX = x >> VoxelConstants::UsableRangeShift;
	int chunkPosY = y >> VoxelConstants::UsableRangeShift;
	int chunkPosZ = z >> VoxelConstants::UsableRangeShift;
	int localX = x % VoxelConstants::UsableRange;
	int localY = y % VoxelConstants::UsableRange;
	int localZ = z % VoxelConstants::UsableRange;

	int chunkIndex = calcChunkIndex(chunkPosX, chunkPosY, chunkPosZ, 0);
	createIfNeeded(chunkIndex);
	chunks[chunkIndex]->writeRaw(localX, localY, localZ, vData);
	int chunkOffsetX = 0;
	int chunkOffsetY = 0;
	int chunkOffsetZ = 0;
	if (localX < VoxelConstants::DataRange - VoxelConstants::UsableRange){
		// a visit to the neighbour chunk is required.
		chunkOffsetX = -1;
	}
	if (localY < VoxelConstants::DataRange - VoxelConstants::UsableRange){
		chunkOffsetY = -1;
	}
	if (localZ < VoxelConstants::DataRange - VoxelConstants::UsableRange){
		chunkOffsetZ = -1;
	}
	// -1
	if (chunkOffsetX < 0 
		&& chunkPosX + chunkOffsetX >= 0){
		
		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY, chunkPosZ, w);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelConstants::UsableRange, localY, localZ, vData);
	}
	if (chunkOffsetY < 0 
		&& chunkPosY + chunkOffsetY >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX, chunkPosY + chunkOffsetY, chunkPosZ, w);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX, localY + VoxelConstants::UsableRange, localZ, vData);
	}
	if (chunkOffsetZ < 0
		&& chunkPosZ + chunkOffsetZ >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX, chunkPosY, chunkPosZ + chunkOffsetZ, w);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX, localY, localZ + VoxelConstants::UsableRange, vData);
	}
	// -1 -1
	if (chunkOffsetX < 0 && chunkOffsetY < 0 
		&& chunkPosX + chunkOffsetX >= 0 && chunkPosY + chunkOffsetY >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY + chunkOffsetY, chunkPosZ, w);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelConstants::UsableRange, localY + VoxelConstants::UsableRange, localZ, vData);
	}
	if (chunkOffsetY < 0 && chunkOffsetZ < 0 
		&& chunkPosY + chunkOffsetY >= 0 && chunkPosZ + chunkOffsetZ >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX, chunkPosY + chunkOffsetY, chunkPosZ + chunkOffsetZ, w);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX, localY + VoxelConstants::UsableRange, localZ + VoxelConstants::UsableRange, vData);
	}
	if (chunkOffsetX < 0 && chunkOffsetZ < 0 
		&& chunkPosX + chunkOffsetX >= 0 && chunkPosZ + chunkOffsetZ >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY, chunkPosZ + chunkOffsetZ, w);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelConstants::UsableRange, localY, localZ + VoxelConstants::UsableRange, vData);
	}
	// -1 -1 -1
	if (chunkOffsetX < 0 && chunkOffsetY < 0 && chunkOffsetZ < 0 
		&& chunkPosX + chunkOffsetX >= 0 && chunkPosY + chunkOffsetY >= 0 && chunkPosZ + chunkOffsetZ >= 0){
		
		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY + chunkOffsetY, chunkPosZ + chunkOffsetZ, w);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelConstants::UsableRange, localY + VoxelConstants::UsableRange, localZ + VoxelConstants::UsableRange, vData);
	}

}
void VoxelManager::generateVertices(){
	for (auto it = chunks.begin(); it != chunks.end(); it++){
		int key = it->first;
		VoxelChunk* voxelChunk = it->second;
		voxelChunk->generateVertices();
	}
}
void VoxelManager::generateIndices(){
	for (auto it = chunks.begin(); it != chunks.end(); it++){
		int key = it->first;
		VoxelChunk* voxelChunk = it->second;
		voxelChunk->generateIndices();
}
}
void VoxelManager::exportJson(){
	for (auto it = chunks.begin(); it != chunks.end(); it++){
		int key = it->first;
		int x, y, z, w;
		calcChunkXYZW(key, x, y, z, w);
		char tmp[128];
		sprintf_s(tmp, sizeof(tmp), "dcchunks_%d_%d_%d_%d.json", x, y, z, w);
		VoxelChunk* chunk = chunks.at(key);
		MeshSerializer::serialize(tmp, chunk->getVertices(), chunk->getIndices(), chunk->getNormals());
	}
}
void VoxelManager::performSDF(SamplerFunction* sampler){
	const ivec3 minBound = sampler->getMinBound();
	const ivec3 maxBound = sampler->getMaxBound();
	for (int xi = minBound.x; xi <= maxBound.x; xi++){
		for (int yi = minBound.y; yi <= maxBound.y; yi++){
			for (int zi = minBound.z; zi <= maxBound.z; zi++){
				ivec3 pos = ivec3(xi, yi, zi);
				vec3 intersections;
				VoxelData voxel;
				if (xi == 1 && yi == 0 && zi == 63){
					int sdf = 0;
				}
				voxel.material = sampler->materialFunc(pos, 1, &intersections, &voxel.normal[0], &voxel.normal[1], &voxel.normal[2]);
				if (voxel.normal[1].y < 0){
					int sdf = 0;
				}

				voxel.intersections[0] = (unsigned char)(intersections.x * EDGE_SCALE);
				voxel.intersections[1] = (unsigned char)(intersections.y * EDGE_SCALE);
				voxel.intersections[2] = (unsigned char)(intersections.z * EDGE_SCALE);
				/*if (voxel.material != 0){
					printf_s("intersect: %d, %d, %d - %d, %d, %d =(%f, %f, %f)(%f, %f, %f)(%f, %f, %f)\n",
					xi,yi,zi,
					voxel.intersections[0], voxel.intersections[1], voxel.intersections[2],
					voxel.normal[0].x, voxel.normal[0].y, voxel.normal[0].z,
					voxel.normal[1].x, voxel.normal[1].y, voxel.normal[1].z,
					voxel.normal[2].x, voxel.normal[2].y, voxel.normal[2].z
					);
				}*/
				
				write(voxel, xi, yi, zi);// probably inappropriate but shortcut for now.
				
			}
		}
	}
}

void VoxelManager::customSDF(int x, int y, int z, int w, SamplerFunction* sampler){
	int key = calcChunkIndex(x, y, z, w);
	createIfNeeded(key);
	VoxelChunk* voxelChunk = chunks.at(key);
	voxelChunk->customSDF(x, y, z, w, sampler);
}
void VoxelManager::createChunk(const ivec3& pos, const int size, VCNode* node){
}
void VoxelManager::removeChunk(VCNode* chunk){

}
void VoxelManager::relocateChunk(VCNode* chunk, const ivec3& pos){

}