#include "qef.h"
#include <glm/glm.hpp>
#include "SamplerFunction.h"
using namespace glm;
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
	void expand(void);
	void initData(void);

	~OctreeNode(){
		if (data != nullptr)delete data;
	}

	void writeDataToNode(const ivec3& pos, const NodeData* val);
	void performSDF(SamplerFunction* sampler);
	void generateMinimizers(void);
};