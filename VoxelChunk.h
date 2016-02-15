#include "VoxelData.h"
#include <glm/glm.hpp>
#include "qef.h"
#include "SamplerFunction.h"
#define VOXELCHUNK_SIZE 9
#define VOXELCHUNK_USABLE_SIZE 8
#define EDGE_SCALE 256
#define VOXEL_SCALE 1.0f
using namespace glm;
using namespace svd;
class VoxelChunk{
private:
	VoxelData* _data;
	static unsigned int indexMap[VOXELCHUNK_SIZE * VOXELCHUNK_SIZE * VOXELCHUNK_SIZE];
	// mesh data
	vec3* vertices;
	unsigned int* indices;
	inline VoxelData* write(const ivec3& pos, const VoxelData& voxelData){
		int idx = pos.x + pos.y * VOXELCHUNK_SIZE + pos.z * VOXELCHUNK_SIZE * VOXELCHUNK_SIZE;

		return &_data[idx];
	}
	inline VoxelData* read(const ivec3& pos){
		int idx = pos.x + pos.y * VOXELCHUNK_SIZE + pos.z * VOXELCHUNK_SIZE * VOXELCHUNK_SIZE;
		return &_data[idx];
	}
	inline int readVertexIndex(const int x, const int y, const int z){
		return indexMap[x + y * VOXELCHUNK_SIZE + z * VOXELCHUNK_SIZE * VOXELCHUNK_SIZE];
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
	VoxelChunk(void){};
	void performSDF(SamplerFunction* sf);
	void createDataArray(void);
	void generateMesh(void);
	void printIndices(void);
	

};