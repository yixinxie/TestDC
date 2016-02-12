#include "qef.h"
#include <glm/glm.hpp>
#include "SamplerFunction.h"
/*
#define SOLVER_ADD_X(_param, _p, _q, _r) if((_param)->intersects.x >= 0){solver.add(Vec3((_param)->intersects.x + min.x + _p, min.y + _q, min.z + _r), Vec3((_param)->normal[0].x, (_param)->normal[0].y, (_param)->normal[0].z));intersects++;}
#define SOLVER_ADD_Y(_param, _p, _q, _r) if((_param)->intersects.y >= 0){solver.add(Vec3(min.x + _p, (_param)->intersects.y + min.y + _q, min.z + _r), Vec3((_param)->normal[1].x, (_param)->normal[1].y, (_param)->normal[1].z));intersects++;}
#define SOLVER_ADD_Z(_param, _p, _q, _r) if((_param)->intersects.z >= 0){solver.add(Vec3(min.x + _p, min.y + _q, (_param)->intersects.z + min.z + _r), Vec3((_param)->normal[2].x, (_param)->normal[2].y, (_param)->normal[2].z));intersects++;}
*/
using namespace glm;
using namespace svd;
struct NodeData{
public:
	svd::QefData qefData;
	glm::vec3 minimizer;

	// terrain data
	unsigned char material;

	/*
	this stores 3 intersections on the 3 minimal edges.
	when the value is negative it means there's no intersection on that axis.
	also each channel only stores the magnitude on that axis. 
	so if intersects has a value of 0.2,0.6,-1
	it means it's intersections 0.2,0,0 on the x axis, and 0,0.6,0 on the y axis.
	no intersection on the z axis.
	*/
	glm::vec3 intersects; 
	glm::vec3 normal[3]; // the 3 normals associated with the 3 points.
	NodeData() :intersects(glm::vec3(-1)), material(0){
		normal[0] = glm::vec3(0);
		normal[1] = glm::vec3(0);
		normal[2] = glm::vec3(0);
	}
	/* 
		in terms of the data structure we want to use to store the octree,
		I'm contemplating the following:
		we still use an octree-based structure to index the cells where 'things' are
		happening. in addition, each cell stores the intersections on the three minimal axis.
	*/

};
const int childInc[24] = {
	0, 0, 0,
	1, 0, 0,
	0, 1, 0,
	1, 1, 0,

	0, 0, 1,
	1, 0, 1,
	0, 1, 1,
	1, 1, 1,
};
class OctreeNode{
private:
	OctreeNode* children[8];
	int size;
	glm::ivec3 min;
public:
	NodeData* data;
	OctreeNode(int _size, int minX, int minY, int minZ) :size(_size), min(minX, minY, minZ){
		for (int i = 0; i < 8; i++){
			children[i] = nullptr;
		}
		data = nullptr;
	}
	~OctreeNode(){
		if (data != nullptr)delete data;
	}
	void expand(void);
	inline void createChild(int childIndex){
		int childSize = size / 2;
		children[childIndex] = new OctreeNode(childSize,
			min.x + childInc[childIndex * 3] * childSize,
			min.y + childInc[childIndex * 3 + 1] * childSize,
			min.z + childInc[childIndex * 3 + 2] * childSize);
	}
	inline void solverAddX(NodeData* _dat, QefSolver& solver, int& intersectionCount, float _x, float _y, float _z){
		if (_dat->intersects.x >= 0){
			solver.add(
				Vec3(_x + _dat->intersects.x, _y, _z),
				Vec3(_dat->normal[0].x, _dat->normal[0].y, _dat->normal[0].z));
			intersectionCount++;
		}
	}
	inline void solverAddY(NodeData* _dat, QefSolver& solver, int& intersectionCount, float _x, float _y, float _z){
		if (_dat->intersects.y >= 0){
			solver.add(
				Vec3(_x, _y + _dat->intersects.y, _z),
				Vec3(_dat->normal[1].x, _dat->normal[1].y, _dat->normal[1].z));
			intersectionCount++;
		}
	}
	inline void solverAddZ(NodeData* _dat, QefSolver& solver, int& intersectionCount, float _x, float _y, float _z){
		if (_dat->intersects.z >= 0){
			solver.add(
				Vec3(_x, _y, _z + _dat->intersects.z),
				Vec3(_dat->normal[2].x, _dat->normal[2].y, _dat->normal[2].z));
			intersectionCount++;
		}
	}

	NodeData* readLeafData(const OctreeNode* curNode, const ivec3& pos);
	void writeDataToNode(const ivec3& pos, const NodeData* val);
	void performSDF(SamplerFunction* sampler);
	static void generateMinimizers(OctreeNode* curNode, OctreeNode* rootNode);
};