#pragma once
#include <glm/glm.hpp>

using namespace glm;
struct VoxelData{
public:
	// terrain material
	unsigned char material;

	/*
	this stores 3 intersections on the 3 minimal edges.
	when the value is 0 it means there's no intersection on that axis.
	also each channel only stores the magnitude on that axis.
	*/
	unsigned char intersections[3];
	// the 3 normals associated with the 3 points.
	glm::vec3 normal[3]; 
	VoxelData() :material(0){
		normal[0] = glm::vec3(0);
		normal[1] = glm::vec3(0);
		normal[2] = glm::vec3(0);
		intersections[0] = 0;
		intersections[1] = 0;
		intersections[2] = 0;
	}

};