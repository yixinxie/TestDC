#include "VoxelChunk.h"
class VoxelManager{
private:
	VoxelChunk** chunks;
	int xCount, yCount, zCount; // in # of chunks.
	int worldBoundX, worldBoundY, worldBoundZ;
	inline int calcChunkIndex(int chunkPosX, int chunkPosY, int chunkPosZ){
		return chunkPosX + chunkPosY * xCount + chunkPosZ * xCount * yCount;
	}
	inline void createIfNeeded(int chunkIndex){
		if (chunks[chunkIndex] == nullptr){
			chunks[chunkIndex] = new VoxelChunk();
		}
	}
	void zeroChunkPointers(void);
public:
	VoxelManager(void):chunks(nullptr){
	}
	void initChunkSize(int _xcount, int _ycount, int _zcount);
	void initWorldSize(int _sizeX, int _sizeY, int _sizeZ);
	~VoxelManager(){
		delete[] chunks;
	}
	void write(const VoxelData& vData, const int x, const int y, const int z);
	void performSDF(SamplerFunction* sampler);
	void generateMeshes(void);
};