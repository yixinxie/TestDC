#include "VoxelChunk.h"
#include <vector>
using namespace std;
unsigned int VoxelChunk::indexMap[VOXELCHUNK_SIZE * VOXELCHUNK_SIZE * VOXELCHUNK_SIZE];
const int indexOrder[] = {
	-1, -1,
	0, -1,
	-1, 0,
	-1, -1,

	0, -1,
	-1, -1,
	-1, -1,
	-1, 0,
};
void VoxelChunk::createDataArray(){
	_data = new VoxelData[VOXELCHUNK_SIZE * VOXELCHUNK_SIZE * VOXELCHUNK_SIZE];
}
void VoxelChunk::generateMesh(){
	tempVertices.clear();
	tempIndices.clear();
	const float QEF_ERROR = 1e-6f;
	const int QEF_SWEEPS = 4;
	QefSolver solver;
	VoxelData* eight[8];
	int intersectionCount;
	int vertexId = 0;
	for (int z = 0; z < VOXELCHUNK_USABLE_SIZE; z++){
		for (int y = 0; y < VOXELCHUNK_USABLE_SIZE; y++){
			for (int x = 0; x < VOXELCHUNK_USABLE_SIZE; x++){
				int idx = x + y * VOXELCHUNK_SIZE + z * VOXELCHUNK_SIZE * VOXELCHUNK_SIZE;
				solver.reset();
				intersectionCount = 0;

				if (x == 1 && y == 1 && z == 1){
					int sdf = 0;
				}
				// step 1
				eight[0] = read(ivec3(x, y, z));
				eight[1] = read(ivec3(x + 1, y, z));
				eight[2] = read(ivec3(x, y + 1, z));
				eight[3] = read(ivec3(x + 1, y + 1, z));
				eight[4] = read(ivec3(x, y, z + 1));
				eight[5] = read(ivec3(x + 1, y, z + 1));
				eight[6] = read(ivec3(x, y + 1, z + 1));
				eight[7] = read(ivec3(x + 1, y + 1, z + 1));
				unsigned char baseMat = eight[0]->material;
				// step 2
				int xmin = -1, ymin = -1, zmin = -1;
				if (baseMat == 0 && eight[1]->material != 0)xmin = 1;
				else if (baseMat != 0 && eight[1]->material == 0)xmin = 0;
				if (baseMat == 0 && eight[2]->material != 0)ymin = 1;
				else if (baseMat != 0 && eight[2]->material == 0)ymin = 0;
				if (baseMat == 0 && eight[4]->material != 0)zmin = 1;
				else if (baseMat != 0 && eight[4]->material == 0)zmin = 0;

				// step 3
				solverAddX(eight[1]->material, eight[0], solver, intersectionCount, 0, 0, 0);
				solverAddY(eight[2]->material, eight[0], solver, intersectionCount, 0, 0, 0);
				solverAddZ(eight[4]->material, eight[0], solver, intersectionCount, 0, 0, 0);

				solverAddZ(eight[5]->material, eight[1], solver, intersectionCount, 1, 0, 0);
				solverAddY(eight[3]->material, eight[1], solver, intersectionCount, 1, 0, 0);

				solverAddX(eight[3]->material, eight[2], solver, intersectionCount, 0, 1, 0);
				solverAddZ(eight[6]->material, eight[2], solver, intersectionCount, 0, 1, 0);

				solverAddX(eight[5]->material, eight[4], solver, intersectionCount, 0, 0, 1);
				solverAddY(eight[6]->material, eight[4], solver, intersectionCount, 0, 0, 1);

				solverAddZ(eight[7]->material, eight[3], solver, intersectionCount, 1, 1, 0);
				solverAddY(eight[7]->material, eight[5], solver, intersectionCount, 1, 0, 1);
				solverAddX(eight[7]->material, eight[6], solver, intersectionCount, 0, 1, 1);


				// step 4 : calculate the minimizer position
				if (intersectionCount > 0)
				{
					Vec3 res;
					solver.solve(res, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);
					res.x += x;
					res.y += y;
					res.z += z;
					printf_s("%f, %f, %f\n", res.x, res.y, res.z);
					tempVertices.push_back(vec3(res.x, res.y, res.z));
					// store the vertex Id to the index map.
					indexMap[idx] = vertexId;
					vertexId++;
				}
				// step 5 : build a quad for any of the three minimal edge that has mismatching material pair, 
				// as indicated in step 2
				
				if (xmin != -1){
					tempIndices.push_back(readVertexIndex(x, y, z));
					tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8], z + indexOrder[xmin * 8 + 1]));
					tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 2], z + indexOrder[xmin * 8 + 3]));
					tempIndices.push_back(readVertexIndex(x, y, z));
					tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 4], z + indexOrder[xmin * 8 + 5]));
					tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 6], z + indexOrder[xmin * 8 + 7]));
				}
				if (ymin != -1){
					tempIndices.push_back(readVertexIndex(x, y, z));
					tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 1], y, z + indexOrder[ymin * 8]));
					tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 3], y, z + indexOrder[ymin * 8 + 2]));
					tempIndices.push_back(readVertexIndex(x, y, z));
					tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 5], y, z + indexOrder[ymin * 8 + 4]));
					tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 7], y, z + indexOrder[ymin * 8 + 6]));
				}
				if (zmin != -1){
					tempIndices.push_back(readVertexIndex(x, y, z));
					tempIndices.push_back(readVertexIndex(x + indexOrder[zmin * 8], y + indexOrder[zmin * 8 + 1], z));
					tempIndices.push_back(readVertexIndex(x + indexOrder[zmin * 8 + 2], y + indexOrder[zmin * 8 + 3], z));
					tempIndices.push_back(readVertexIndex(x, y, z));
					tempIndices.push_back(readVertexIndex(x + indexOrder[zmin * 8 + 4], y + indexOrder[zmin * 8 + 5], z));
					tempIndices.push_back(readVertexIndex(x + indexOrder[zmin * 8 + 6], y + indexOrder[zmin * 8 + 7], z));
				}
			}
		}
	}
	printf_s("\n");
	/*for (auto it = tempIndices.begin(); it != tempIndices.end; it++){
		printf_s("%d, ", *it);
	}*/
	for (int i = 0; i < tempIndices.size(); i++){
		printf_s("%d, ", tempIndices[i]);
	}
}

void VoxelChunk::performSDF(SamplerFunction* sampler){
	const ivec3 minBound = sampler->getMinBound();
	const ivec3 maxBound = sampler->getMaxBound();
	for (int xi = minBound.x; xi <= maxBound.x; xi++){
		for (int yi = minBound.y; yi <= maxBound.y; yi++){
			for (int zi = minBound.z; zi <= maxBound.z; zi++){
				ivec3 pos = ivec3(xi, yi, zi);
				vec3 intersections;
				VoxelData voxel;
				if (xi == 0 && yi == 1 && zi == 1){
					int sdf = 0;
				}
				voxel.material = sampler->materialFunc(pos, &intersections, &voxel.normal[0], &voxel.normal[1], &voxel.normal[2]);
				voxel.intersections[0] = (unsigned char)(intersections.x * EDGE_SCALE);
				voxel.intersections[1] = (unsigned char)(intersections.y * EDGE_SCALE);
				voxel.intersections[2] = (unsigned char)(intersections.z * EDGE_SCALE);
				/*if (voxel.material != 0){
					printf_s("intersect: %d, %d, %d - %d, %d, %d =(%f, %f, %f)(%f, %f, %f)(%f, %f, %f)\n",
						xi,yi,zi,
						voxel.intersections[0], voxel.intersections[1], voxel.intersections[2],
						voxel.normal[0].x, voxel.normal[0].y, voxel.normal[0].z,
						voxel.normal[1].x, voxel.normal[1].y, voxel.normal[1].z,
						voxel.normal[2].x, voxel.normal[2].y, voxel.normal[2].z
						);
				}*/
				VoxelData* readVoxel = read(pos);// probably inappropriate but shortcut for now.
				readVoxel->material = voxel.material;
				readVoxel->normal[0] = voxel.normal[0];
				readVoxel->normal[1] = voxel.normal[1];
				readVoxel->normal[2] = voxel.normal[2];
				readVoxel->intersections[0] = voxel.intersections[0];
				readVoxel->intersections[1] = voxel.intersections[1];
				readVoxel->intersections[2] = voxel.intersections[2];
			}
		}
	}
}