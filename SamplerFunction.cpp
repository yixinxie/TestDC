#include "SamplerFunction.h"
#include "VoxelChunk.h"
#include <math.h>
SF_Box::SF_Box(){
}
// each time the box gets a new size, the bounding box should be refreshed as well.
void SF_Box::setSpecs(const vec3& min, const vec3& max){
	rectMin = min;
	rectMax = max;

	// recalculate the bounding box. there could be some numerical problems when the values are big.
	minBound.x = (int)floor(rectMin.x) - 1;
	minBound.y = (int)floor(rectMin.y) - 1;
	minBound.z = (int)floor(rectMin.z) - 1;
	minBound = clamp(minBound, ivec3(0, 0, 0), ivec3(VoxelConstants::UsableRange, VoxelConstants::UsableRange, VoxelConstants::UsableRange));

	maxBound.x = (int)floor(rectMax.x) + 1;
	maxBound.y = (int)floor(rectMax.y) + 1;
	maxBound.z = (int)floor(rectMax.z) + 1;
	maxBound = clamp(maxBound, ivec3(0, 0, 0), ivec3(VoxelConstants::UsableRange, VoxelConstants::UsableRange, VoxelConstants::UsableRange));
}
unsigned char SF_Box::materialFunc(const ivec3& pos, const int cellSize, vec3* intersects, vec3* outNormal0, vec3* outNormal1, vec3* outNormal2){
	unsigned char material = 0;
	if (pos.x >= rectMin.x &&
		pos.x <= rectMax.x &&
		pos.y >= rectMin.y &&
		pos.y <= rectMax.y &&
		pos.z >= rectMin.z &&
		pos.z <= rectMax.z){

		material = 1;
	}
	/*
	here, the method is we create three line segments, corresponding the three minimal edges
	of this cell, and see if they intersect with the surface of the primitive, in this case,
	the 6 faces of the cube.
	If one line segment intersects with one of the surfaces, we output that to the intersects and outNormalx pointers
	*/
	if (pos.y >= rectMin.y && pos.y <= rectMax.y &&
		pos.z >= rectMin.z && pos.z <= rectMax.z){
		if (pos.x <= rectMin.x && pos.x + cellSize >= rectMin.x){

			*outNormal0 = vec3(-1, 0, 0);
			intersects->x = (rectMin.x - pos.x) / (float)cellSize;
		}
		else if (pos.x <= rectMax.x && pos.x + cellSize >= rectMax.x){
			*outNormal0 = vec3(1, 0, 0);
			intersects->x = (rectMax.x - pos.x) / (float)cellSize;
		}
	}
	if (pos.x >= rectMin.x && pos.x <= rectMax.x &&
		pos.z >= rectMin.z && pos.z <= rectMax.z){
		if (pos.y <= rectMin.y && pos.y + cellSize >= rectMin.y){

			*outNormal1 = vec3(0, -1, 0);
			intersects->y = (rectMin.y - pos.y) / (float)cellSize;
		}
		else if (pos.y <= rectMax.y && pos.y + cellSize >= rectMax.y){
			*outNormal1 = vec3(0, 1, 0);
			intersects->y = (rectMax.y - pos.y) / (float)cellSize;
		}
	}
	if (pos.y >= rectMin.y && pos.y <= rectMax.y &&
		pos.x >= rectMin.x && pos.x <= rectMax.x){
		if (pos.z <= rectMin.z && pos.z + cellSize >= rectMin.z){

			*outNormal2 = vec3(0, 0, -1);
			intersects->z = (rectMin.z - pos.z) / (float)cellSize;
		}
		else if (pos.z <= rectMax.z && pos.z + cellSize >= rectMax.z){
			*outNormal2 = vec3(0, 0, 1);
			intersects->z = (rectMax.z - pos.z) / (float)cellSize;
		}
	}

	
	return material;
}

