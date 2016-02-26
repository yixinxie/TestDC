#include "VoxelManager.h"
#include "MeshSerializer.h"
#include "Visualize.h"
void VoxelManager::zeroChunkPointers(){
	for (int z = 0; z < zCount; z++){
		for (int y = 0; y < yCount; y++){
			for (int x = 0; x < xCount; x++){
				int chunkIndex = calcChunkIndex(x, y, z);
				chunks[chunkIndex] = nullptr;
			}
		}
	}
}
void VoxelManager::initChunkSize(int _xcount, int _ycount, int _zcount){
	xCount = _xcount;
	yCount = _ycount;
	zCount = _zcount;
	chunks = new VoxelChunk*[xCount * yCount * zCount];

	worldBoundX = xCount * VoxelChunk::UsableRange;
	worldBoundY = yCount * VoxelChunk::UsableRange;
	worldBoundZ = zCount * VoxelChunk::UsableRange;

	zeroChunkPointers();
}

void VoxelManager::initWorldSize(int _sizeX, int _sizeY, int _sizeZ){
	worldBoundX = _sizeX;
	worldBoundY = _sizeY;
	worldBoundZ = _sizeZ;

	xCount = worldBoundX / VoxelChunk::UsableRange;
	yCount = worldBoundY / VoxelChunk::UsableRange;
	zCount = worldBoundZ / VoxelChunk::UsableRange;
	if (xCount == 0)xCount = 1;
	if (yCount == 0)yCount = 1;
	if (zCount == 0)zCount = 1;
	chunks = new VoxelChunk*[xCount * yCount * zCount];
	zeroChunkPointers();
}
void VoxelManager::write(const VoxelData& vData, const int x, const int y, const int z){
	int chunkPosX = x >> VoxelChunk::UsableRangeShift;
	int chunkPosY = y >> VoxelChunk::UsableRangeShift;
	int chunkPosZ = z >> VoxelChunk::UsableRangeShift;
	int localX = x % VoxelChunk::UsableRange;
	int localY = y % VoxelChunk::UsableRange;
	int localZ = z % VoxelChunk::UsableRange;

	int chunkIndex = calcChunkIndex(chunkPosX, chunkPosY, chunkPosZ);
	createIfNeeded(chunkIndex);
	chunks[chunkIndex]->writeRaw(localX, localY, localZ, vData);
	int chunkOffsetX = 0;
	int chunkOffsetY = 0;
	int chunkOffsetZ = 0;
	if (localX < VoxelChunk::DataRange - VoxelChunk::UsableRange){
		// a visit to the neighbour chunk is required.
		chunkOffsetX = -1;
	}
	if (localY < VoxelChunk::DataRange - VoxelChunk::UsableRange){
		chunkOffsetY = -1;
	}
	if (localZ < VoxelChunk::DataRange - VoxelChunk::UsableRange){
		chunkOffsetZ = -1;
	}
	// -1
	if (chunkOffsetX < 0 
		&& chunkPosX + chunkOffsetX >= 0){
		
		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY, chunkPosZ);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelChunk::UsableRange, localY, localZ, vData);
	}
	if (chunkOffsetY < 0 
		&& chunkPosY + chunkOffsetY >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX, chunkPosY + chunkOffsetY, chunkPosZ);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX, localY + VoxelChunk::UsableRange, localZ, vData);
	}
	if (chunkOffsetZ < 0
		&& chunkPosZ + chunkOffsetZ >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX, chunkPosY, chunkPosZ + chunkOffsetZ);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX, localY, localZ + VoxelChunk::UsableRange, vData);
	}
	// -1 -1
	if (chunkOffsetX < 0 && chunkOffsetY < 0 
		&& chunkPosX + chunkOffsetX >= 0 && chunkPosY + chunkOffsetY >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY + chunkOffsetY, chunkPosZ);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelChunk::UsableRange, localY + VoxelChunk::UsableRange, localZ, vData);
	}
	if (chunkOffsetY < 0 && chunkOffsetZ < 0 
		&& chunkPosY + chunkOffsetY >= 0 && chunkPosZ + chunkOffsetZ >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX, chunkPosY + chunkOffsetY, chunkPosZ + chunkOffsetZ);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX, localY + VoxelChunk::UsableRange, localZ + VoxelChunk::UsableRange, vData);
	}
	if (chunkOffsetX < 0 && chunkOffsetZ < 0 
		&& chunkPosX + chunkOffsetX >= 0 && chunkPosZ + chunkOffsetZ >= 0){

		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY, chunkPosZ + chunkOffsetZ);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelChunk::UsableRange, localY, localZ + VoxelChunk::UsableRange, vData);
	}
	// -1 -1 -1
	if (chunkOffsetX < 0 && chunkOffsetY < 0 && chunkOffsetZ < 0 
		&& chunkPosX + chunkOffsetX >= 0 && chunkPosY + chunkOffsetY >= 0 && chunkPosZ + chunkOffsetZ >= 0){
		
		int neighbourChunkIndex = calcChunkIndex(chunkPosX + chunkOffsetX, chunkPosY + chunkOffsetY, chunkPosZ + chunkOffsetZ);
		createIfNeeded(neighbourChunkIndex);
		chunks[neighbourChunkIndex]->writeRaw(localX + VoxelChunk::UsableRange, localY + VoxelChunk::UsableRange, localZ + VoxelChunk::UsableRange, vData);
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
				voxel.material = sampler->materialFunc(pos, &intersections, &voxel.normal[0], &voxel.normal[1], &voxel.normal[2]);
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
void VoxelManager::generateMeshes(){
	for (int z = 0; z < zCount; z++){
		for (int y = 0; y < yCount; y++){
			for (int x = 0; x < xCount; x++){
				int chunkIndex = calcChunkIndex(x, y, z);
				if (x == 0 && y == 0 && z == 1){
					int sdf = 0;
				}
				if (chunks[chunkIndex] != nullptr){
					chunks[chunkIndex]->generateMesh();
					char tmp[256];
					
					sprintf_s(tmp, sizeof(tmp), "dcchunks_%d_%d_%d.json", x, y, z);
					MeshSerializer::serialize(tmp, chunks[chunkIndex]->getVertices(), chunks[chunkIndex]->getIndices(), chunks[chunkIndex]->getNormals());
				}
			}
		}

	}
}