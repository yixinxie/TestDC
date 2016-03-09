#pragma once
#include <unordered_map>
#include "VoxelChunk.h"
#include "IVoxelChunkManager.h"
#include "ClipmapRing.h"
#define MAX_LOD 3
class VoxelManager : public IVoxelChunkManager{
private:
	ClipmapRing** rings;
	int ringCount;
	std::unordered_map<int, VoxelChunk*> chunks;
	int xCount, yCount, zCount; // in # of chunks.
	int worldBoundX, worldBoundY, worldBoundZ;
	inline int calcChunkIndex(int chunkPosX, int chunkPosY, int chunkPosZ, int lod){
		return (chunkPosX + chunkPosY * xCount + chunkPosZ * xCount * yCount) * MAX_LOD + lod;
	}
	inline void calcChunkXYZW(const int key, int& x, int& y, int& z, int& w){
		w = key % MAX_LOD;
		int xyz = key / MAX_LOD;
		z = xyz / xCount / yCount;
		int xy = xyz - z * xCount * yCount;
		y = xy / xCount;
		x = xy - y * xCount;
	}
	inline VoxelChunk* createIfNeeded(int chunkIndex){
		VoxelChunk* voxelChunk = nullptr;
		auto it = chunks.find(chunkIndex);
		if (it == chunks.end()){
			voxelChunk = new VoxelChunk();
			chunks.insert({ chunkIndex, voxelChunk });
		}
		else{
			voxelChunk = it->second;
		}
		return voxelChunk;

	}
	void zeroChunkPointers(void);
public:
	SamplerFunction* dataSampler;
	VoxelManager(void);
	~VoxelManager();
	void initChunkSize(int _xcount, int _ycount, int _zcount);
	void initWorldSize(int _sizeX, int _sizeY, int _sizeZ);
	void initClipmap(int ringCount);
	
	void write(const VoxelData& vData, const int x, const int y, const int z, const int w = 0);
	void performSDF(SamplerFunction* sampler);
	void generateVertices(void);
	void generateIndices(void);
	void exportJson(void);
	inline VoxelChunk* readChunk(int x, int y, int z, int w){
		int key = calcChunkIndex(x, y, z, w);
		return chunks.at(key);
	}
	void customSDF(int x, int y, int z, int w, SamplerFunction* sampler);
	// interface implementations
	void createChunk(const ivec3& pos, const int lod, VCNode* node);
	void removeChunk(VCNode* chunk);
	void relocateChunk(VCNode* chunk, const ivec3& pos);
	void createEdgeDescs(void);
	
};