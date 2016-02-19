#pragma once
#include "VoxelData.h"
#include <vector>
#include <glm/glm.hpp>
#include "qef.h"
#include "SamplerFunction.h"
#define EDGE_SCALE 255

using namespace glm;
using namespace svd;
class VoxelChunk{
public:
	static const int UsableRangeShift = 4; // # of bit shift
	static const int UsableRange = 1 << UsableRangeShift;
	static const int TraverseRange = UsableRange + 1;
	static const int DataRange = UsableRange + 2;
	
	
private:
	VoxelData* _data;
	static unsigned int indexMap[DataRange * DataRange * DataRange];
	// mesh data, may move these to a different class in the future
	std::vector<vec3> tempVertices;
	std::vector<unsigned int> tempIndices;
	inline int calcIndex(int x, int y, int z){
		return x + y * DataRange + z * DataRange * DataRange;
	}

	inline VoxelData* read(const int x, const int y, const int z){
		int idx = calcIndex(x, y, z);
		return &_data[idx];
	}
	inline int readVertexIndex(const int x, const int y, const int z){
		return indexMap[calcIndex(x, y, z)];
	}
	inline void solverAddX(unsigned char baseMat, const VoxelData* _vdat, QefSolver& solver, int& intersectionCount, float _x, float _y, float _z){
		if (_vdat->material != baseMat){
			solver.add(
				Vec3(_x + ((float)_vdat->intersections[0]) / EDGE_SCALE, _y, _z),
				Vec3(_vdat->normal[0].x, _vdat->normal[0].y, _vdat->normal[0].z));
			intersectionCount++;
		}
	}
	inline void solverAddY(unsigned char baseMat, const VoxelData* _vdat, QefSolver& solver, int& intersectionCount, float _x, float _y, float _z){
		if (_vdat->material != baseMat){
			solver.add(
				Vec3(_x, _y + ((float)_vdat->intersections[1]) / EDGE_SCALE, _z),
				Vec3(_vdat->normal[1].x, _vdat->normal[1].y, _vdat->normal[1].z));
			intersectionCount++;
		}
	}
	inline void solverAddZ(unsigned char baseMat, const VoxelData* _vdat, QefSolver& solver, int& intersectionCount, float _x, float _y, float _z){
		if (_vdat->material != baseMat){
			solver.add(
				Vec3(_x, _y, _z + ((float)_vdat->intersections[2]) / EDGE_SCALE),
				Vec3(_vdat->normal[2].x, _vdat->normal[2].y, _vdat->normal[2].z));
			intersectionCount++;
		}
	}
public:
	VoxelChunk(void) :_data(nullptr){};
	~VoxelChunk(){ if (_data != nullptr) delete _data; };
	void performSDF(SamplerFunction* sf);
	void createDataArray(void);
	void generateMesh(void);
	inline const std::vector<vec3>& getVertices(void){ return tempVertices; };
	inline const std::vector<unsigned int>& getIndices(void){ return tempIndices; };
	void writeRaw(int x, int y, int z, const VoxelData& vData);
	
};