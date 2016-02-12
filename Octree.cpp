#include "Octree.h"
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
void OctreeNode::expand(){
	int childSize = size / 2;
	for (int i = 0; i < 8; i++){
		children[i] = new OctreeNode(childSize, 
			min.x + childInc[i * 3] * childSize, 
			min.y + childInc[i * 3 + 1] * childSize, 
			min.z + childInc[i * 3 + 2] * childSize);

	}
}
void OctreeNode::initData(){
	data = new NodeData();
}

void OctreeNode::performSDF(SamplerFunction* sampler){
	const ivec3 minBound = sampler->getMinBound();
	const ivec3 maxBound = sampler->getMaxBound();
	for (int xi = minBound.x; xi <= maxBound.x; xi++){
		for (int yi = minBound.y; yi <= maxBound.y; yi++){
			for (int zi = minBound.z; zi <= maxBound.z; zi++){
				ivec3 pos = ivec3(xi, yi, zi);
				NodeData val;
				int material = sampler->materialFunc(pos, &val.intersects, &val.normal[0], &val.normal[1], &val.normal[2]);
				//if (val.intersects.x >= 0 || val.intersects.y >= 0 || val.intersects.z >= 0){
				//	printf_s("intersect: %f, %f, %f =(%f, %f, %f)(%f, %f, %f)(%f, %f, %f)\n",
				//		val.intersects.x, val.intersects.y, val.intersects.z,
				//		val.normal[0].x, val.normal[0].y, val.normal[0].z,
				//		val.normal[1].x, val.normal[1].y, val.normal[1].z,
				//		val.normal[2].x, val.normal[2].y, val.normal[2].z
				//		);
				//}
				writeDataToNode(pos, &val);
			}
		}
	}
}
void OctreeNode::writeDataToNode(const ivec3& pos, const NodeData* val){
	if (size == 1){
		if (data == nullptr)data = new NodeData();
		data->material = val->material;
		data->intersects = val->intersects;
		data->normal[0] = val->normal[0];
		data->normal[1] = val->normal[1];
		data->normal[2] = val->normal[2];
		return;
	}
	vec3 local = pos - min;
	int halfSize = size / 2;
	int c0 = (local.x > halfSize) ? 1 : 0;
	int c1 = (local.y > halfSize) ? 2 : 0;
	int c2 = (local.z > halfSize) ? 4 : 0;
	int childIndex = c0 + c1 + c2;
	if (children[childIndex] == nullptr){
		//children[childIndex] = new OctreeNode()
		expand();
	}
	children[childIndex]->writeDataToNode(pos, val);

}
void OctreeNode::generateMinimizers(){

}