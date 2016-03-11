#pragma once
#include "VoxelData.h"
#include "VoxelChunkTransitionSurfaceDesc.h"
#include <vector>
#include "VoxelConstants.h"
#include <glm/glm.hpp>
#include "qef.h"
#include "SamplerFunction.h"
#define EDGE_SCALE 255

using namespace glm;
using namespace svd;
class VoxelChunk{
public:
	
private:
	VoxelData* _data;

	// tables required in the vertex generation.
	int indexMap[VoxelConstants::UsableRange * VoxelConstants::UsableRange * VoxelConstants::UsableRange];
	// this can be optimized to use only 3 * 3 * VoxelConstants::UsableRange ^ 2 chars.
	char edgeMap[VoxelConstants::UsableRange * VoxelConstants::UsableRange * VoxelConstants::UsableRange * 3];
	VoxelChunkTransitionSurfaceDesc edgeDescs[6];
	// mesh data, may move these to a different class in the future
	std::vector<vec3> tempVertices;
	std::vector<unsigned int> tempIndices;
	std::vector<vec3> tempNormals;
	inline int calcDataIndex(int x, int y, int z){
		return x + y * VoxelConstants::DataRange + z * VoxelConstants::DataRange * VoxelConstants::DataRange;
	}
	inline int calcUsableIndex(int x, int y, int z){
		return x + y * VoxelConstants::UsableRange + z * VoxelConstants::UsableRange * VoxelConstants::UsableRange;
	}

	inline VoxelData* read(const int x, const int y, const int z){
		int idx = calcDataIndex(x, y, z);
		return &_data[idx];
	}
	inline int readVertexIndex(const int x, const int y, const int z){
		return indexMap[calcUsableIndex(x, y, z)];
	}
	
	inline void solverAddX(unsigned char baseMat, const VoxelData* _vdat, QefSolver& solver, int& intersectionCount, vec3& accumNormal, float _x, float _y, float _z){
		if (_vdat->material != baseMat){
			solver.add(
				Vec3(_x + ((float)_vdat->intersections[0]) / EDGE_SCALE, _y, _z),
				Vec3(_vdat->normal[0].x, _vdat->normal[0].y, _vdat->normal[0].z));
			intersectionCount++;
			accumNormal += _vdat->normal[0];
		}
	}
	inline void solverAddY(unsigned char baseMat, const VoxelData* _vdat, QefSolver& solver, int& intersectionCount, vec3& accumNormal, float _x, float _y, float _z){
		if (_vdat->material != baseMat){
			solver.add(
				Vec3(_x, _y + ((float)_vdat->intersections[1]) / EDGE_SCALE, _z),
				Vec3(_vdat->normal[1].x, _vdat->normal[1].y, _vdat->normal[1].z));
			intersectionCount++;
			accumNormal += _vdat->normal[1];
		}
	}
	inline void solverAddZ(unsigned char baseMat, const VoxelData* _vdat, QefSolver& solver, int& intersectionCount, vec3& accumNormal, float _x, float _y, float _z){
		if (_vdat->material != baseMat){
			solver.add(
				Vec3(_x, _y, _z + ((float)_vdat->intersections[2]) / EDGE_SCALE),
				Vec3(_vdat->normal[2].x, _vdat->normal[2].y, _vdat->normal[2].z));
			intersectionCount++;
			accumNormal += _vdat->normal[2];
		}
	}
	// helper functions for createEdgeDesc2D
	void duplicateIndicesAndEdgeFlags(const vec3& vertTranslate, VoxelChunk* adjChunk, int loc0, int loc1, VoxelChunkTransitionSurfaceDesc* edgeDesc, int facing);
	void copyIndicesAndEdgeFlags(const vec3& vertTranslate, VoxelChunk* adjChunk, int loc0, int loc1, VoxelChunkTransitionSurfaceDesc* edgeDesc, int facing);
	
public:

	VoxelChunk(void);
	~VoxelChunk();
	void performSDF(SamplerFunction* sf);

	void createDataArray(void);
	void generateVertices(void);
	void generateIndices(void);
	inline const std::vector<vec3>& getVertices(void){ return tempVertices; };
	inline const std::vector<unsigned int>& getIndices(void){ return tempIndices; };
	inline const std::vector<vec3>& getNormals(void){ return tempNormals; };
	
	void writeRaw(int x, int y, int z, const VoxelData& vData);
	
	// experimental
	
	void customSDF(int x, int y, int z, int w, SamplerFunction* sampler);
	void createEdgeDesc1D(const int lodDiff, const int loc0, VoxelChunk* adjChunk, const int facing);

	void createEdgeDesc2D(const int lodDiff, const int loc0, const int loc1, VoxelChunk* adjChunk, const int facing);
};