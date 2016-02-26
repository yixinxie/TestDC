#include "Octree.h"
// not used. use createChild instead!
void OctreeNode::expand(){
	int childSize = size / 2;
	for (int i = 0; i < 8; i++){
		children[i] = new OctreeNode(childSize, 
			min.x + childInc[i * 3] * childSize, 
			min.y + childInc[i * 3 + 1] * childSize, 
			min.z + childInc[i * 3 + 2] * childSize);

	}
}

void OctreeNode::performSDF(SamplerFunction* sampler){
	const ivec3 minBound = sampler->getMinBound();
	const ivec3 maxBound = sampler->getMaxBound();
	for (int xi = minBound.x; xi <= maxBound.x; xi++){
		for (int yi = minBound.y; yi <= maxBound.y; yi++){
			for (int zi = minBound.z; zi <= maxBound.z; zi++){
				ivec3 pos = ivec3(xi, yi, zi);
				NodeData val;
				int material = sampler->materialFunc(pos, 1, &val.intersects, &val.normal[0], &val.normal[1], &val.normal[2]);
				//if (val.intersects.x >= 0 || val.intersects.y >= 0 || val.intersects.z >= 0){
				//	printf_s("intersect: %f, %f, %f =(%f, %f, %f)(%f, %f, %f)(%f, %f, %f)\n",
				//		val.intersects.x, val.intersects.y, val.intersects.z,
				//		val.normal[0].x, val.normal[0].y, val.normal[0].z,
				//		val.normal[1].x, val.normal[1].y, val.normal[1].z,
				//		val.normal[2].x, val.normal[2].y, val.normal[2].z
				//		);
				//}

				/*
				There are two methods here.
				1, only write nodes that have at least one intersection on their minimal axis.
					in this case, when we generate the minimizers for each cell, we also need to 
					create and traverse	the negative-adjacent cells of all cells.
					so ultimately, the nodes that have intersections on their non-minimal axis will 
					still be generated regardless. this leads to method two.
				2, write nodes whenever there's a material change in any of their corners.
				*/
				if (val.intersects.x >= 0 || val.intersects.y >= 0 || val.intersects.z >= 0)writeDataToNode(pos, &val);
				//writeDataToNode(pos, &val);
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
	int c0 = (local.x >= halfSize) ? 1 : 0;
	int c1 = (local.y >= halfSize) ? 2 : 0;
	int c2 = (local.z >= halfSize) ? 4 : 0;
	int childIndex = c0 + c1 + c2;
	if (children[childIndex] == nullptr){
		//children[childIndex] = new OctreeNode()
		createChild(childIndex);
	}
	children[childIndex]->writeDataToNode(pos, val);

}
NodeData* OctreeNode::readLeafData(OctreeNode* curNode, const ivec3& pos){
	OctreeNode* ret = readLeafNode(curNode, pos);
	
	return (ret != nullptr) ? ret->data : nullptr;
}
OctreeNode* OctreeNode::readLeafNode(OctreeNode* curNode, const ivec3& pos){
	if (curNode->size == 1){
		return curNode;
	}

	vec3 local = pos - curNode->min;
	int halfSize = curNode->size / 2;
	int c0 = (local.x >= halfSize) ? 1 : 0;
	int c1 = (local.y >= halfSize) ? 2 : 0;
	int c2 = (local.z >= halfSize) ? 4 : 0;
	int childIndex = c0 + c1 + c2;
	if (curNode->children[childIndex] == nullptr)return nullptr;
	return readLeafNode(curNode->children[childIndex], pos);
}
void OctreeNode::generateMinimizers(OctreeNode* rootNode){
	std::vector<ivec3> adjacentList;
	_generateMinimizers(rootNode, rootNode, adjacentList);
	for (auto it = adjacentList.begin(); it != adjacentList.end(); it++){
		if (it->x < 0 || it->y < 0 || it->z < 0)continue;
		OctreeNode* node = rootNode->readLeafNode(rootNode, *it);
		if (node == nullptr){
			NodeData emptyData;

			rootNode->writeDataToNode(*it, &emptyData);
			node = rootNode->readLeafNode(rootNode, *it);
			generateLeafMinimizer(node, rootNode);
		}
	}
}
void OctreeNode::_generateMinimizers(OctreeNode* curNode, OctreeNode* rootNode, std::vector<glm::ivec3>& adjacentList){

	if (curNode->size != 1){
		for (int i = 0; i < 8; i++){
			if (curNode->children[i] != nullptr){
				
				_generateMinimizers(curNode->children[i], rootNode, adjacentList);
			}
		}
	}
	else{
		bool adjacentVisit = generateLeafMinimizer(curNode, rootNode);
		if (adjacentVisit){
			adjacentList.push_back(curNode->min + ivec3(-1, 0, 0));
			adjacentList.push_back(curNode->min + ivec3(0, -1, 0));
			adjacentList.push_back(curNode->min + ivec3(0, 0, -1));

			adjacentList.push_back(curNode->min + ivec3(-1, -1, 0));
			adjacentList.push_back(curNode->min + ivec3(0, -1, -1));
			adjacentList.push_back(curNode->min + ivec3(-1, 0, -1));
		}
	}
	
}
bool OctreeNode::generateLeafMinimizer(OctreeNode* curNode, OctreeNode* rootNode){
	
	const float QEF_ERROR = 1e-6f;
	const int QEF_SWEEPS = 4;
	QefSolver solver;
	// here we presume curNode is a leaf node.
	// find the intersection points on all 12 edges.
	int intersectionCount = 0;
	vec3 min = curNode->min;
	// starting from the minimal edges of this cell.
	
	curNode->solverAddX(curNode->data, solver, intersectionCount, 0, 0, 0);
	curNode->solverAddY(curNode->data, solver, intersectionCount, 0, 0, 0);
	curNode->solverAddZ(curNode->data, solver, intersectionCount, 0, 0, 0);
	/* at this point, if the intersection count is non-zero, that means one or more intersections
	exist on the minimal axis. they could be on the maximal axis of the negative-adjacent cells.
	and those cells need to be visited to calculate their minimizers;
	*/
	bool hasIntersectionOnMinAxis = intersectionCount > 0;
	// find edges that are minimal edges of positive-adjacent cells.
	NodeData* adjacentData = nullptr;
	adjacentData = curNode->readLeafData(rootNode, ivec3(min.x + 1, min.y, min.z));
	if (adjacentData != nullptr){
		curNode->solverAddY(adjacentData, solver, intersectionCount, 1, 0, 0);
		curNode->solverAddZ(adjacentData, solver, intersectionCount, 1, 0, 0);
	}
	adjacentData = curNode->readLeafData(rootNode, ivec3(min.x, min.y + 1, min.z));
	if (adjacentData != nullptr){
		curNode->solverAddX(adjacentData, solver, intersectionCount, 0, 1, 0);
		curNode->solverAddZ(adjacentData, solver, intersectionCount, 0, 1, 0);
	}
	adjacentData = curNode->readLeafData(rootNode, ivec3(min.x, min.y, min.z + 1));
	if (adjacentData != nullptr){
		curNode->solverAddX(adjacentData, solver, intersectionCount, 0, 0, 1);
		curNode->solverAddY(adjacentData, solver, intersectionCount, 0, 0, 1);
	}
	adjacentData = curNode->readLeafData(rootNode, ivec3(min.x + 1, min.y + 1, min.z));
	if (adjacentData != nullptr){
		curNode->solverAddZ(adjacentData, solver, intersectionCount, 1, 1, 0);
	}
	adjacentData = curNode->readLeafData(rootNode, ivec3(min.x + 1, min.y, min.z + 1));
	if (adjacentData != nullptr){
		curNode->solverAddY(adjacentData, solver, intersectionCount, 1, 0, 1);
	}
	adjacentData = curNode->readLeafData(rootNode, ivec3(min.x, min.y + 1, min.z + 1));
	if (adjacentData != nullptr){
		curNode->solverAddX(adjacentData, solver, intersectionCount, 0, 1, 1);
	}

	if (intersectionCount > 0)
	{
		Vec3 res;
		solver.solve(res, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);
		curNode->data->minimizer = vec3(res.x, res.y, res.z);
		curNode->data->minimizer += min;
		printf_s("%f, %f, %f\n", curNode->data->minimizer.x, curNode->data->minimizer.y, curNode->data->minimizer.z);
		//curNode->data->qefData = solver.getData();
	}
	curNode->data->processed = 1;
	return hasIntersectionOnMinAxis;
}