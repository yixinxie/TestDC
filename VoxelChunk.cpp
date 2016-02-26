#include "VoxelChunk.h"
#include <vector>
#include "Visualize.h"
using namespace std;
//unsigned int VoxelChunk::indexMap[DataRange * DataRange * DataRange];
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
	_data = new VoxelData[DataRange * DataRange * DataRange];
	memset(_data, 0, sizeof(VoxelData) * DataRange * DataRange * DataRange);
}

void VoxelChunk::generateVertices(){
	tempVertices.clear();
	tempNormals.clear();
	const float QEF_ERROR = 1e-6f;
	const int QEF_SWEEPS = 4;
	QefSolver solver;
	VoxelData* eight[8];
	int intersectionCount;
	int vertexId = 0;
	{
		int indexMapLength = UsableRange * UsableRange * UsableRange;
		for (int i = 0; i < indexMapLength; i++){
			indexMap[i] = -1;
		}
	}
	for (int z = 0; z < UsableRange; z++){
		for (int y = 0; y < UsableRange; y++){
			for (int x = 0; x < UsableRange; x++){
				int idx = calcDataIndex(x, y, z);
				solver.reset();
				vec3 accumNormal;
				intersectionCount = 0;
				if (x == 1 && y == 1 && z == 0){
					int sdf = 0;
				}
				// step 1: get the pointers to the 8 neighbour cells.
				eight[0] = read(x, y, z);
				eight[1] = read(x + 1, y, z);
				eight[2] = read(x, y + 1, z);
				eight[3] = read(x + 1, y + 1, z);
				eight[4] = read(x, y, z + 1);
				eight[5] = read(x + 1, y, z + 1);
				eight[6] = read(x, y + 1, z + 1);
				eight[7] = read(x + 1, y + 1, z + 1);

				unsigned char baseMat = eight[0]->material;
				// step 2 : determine the indices from any crossing on the 3 minimal axis/edges.
				int xmin = -1, ymin = -1, zmin = -1;
				if (baseMat == 0 && eight[1]->material != 0)xmin = 1;
				else if (baseMat != 0 && eight[1]->material == 0)xmin = 0;
				if (baseMat == 0 && eight[2]->material != 0)ymin = 1;
				else if (baseMat != 0 && eight[2]->material == 0)ymin = 0;
				if (baseMat == 0 && eight[4]->material != 0)zmin = 1;
				else if (baseMat != 0 && eight[4]->material == 0)zmin = 0;

				// step 3 : add the intersections and their normals to the solver, up to 12 intersections/edges.
				// at the same time, increment the intersection count and normal accumulation.
				solverAddX(eight[1]->material, eight[0], solver, intersectionCount, accumNormal, 0, 0, 0);
				solverAddY(eight[2]->material, eight[0], solver, intersectionCount, accumNormal, 0, 0, 0);
				solverAddZ(eight[4]->material, eight[0], solver, intersectionCount, accumNormal, 0, 0, 0);

				solverAddZ(eight[5]->material, eight[1], solver, intersectionCount, accumNormal, 1, 0, 0);
				solverAddY(eight[3]->material, eight[1], solver, intersectionCount, accumNormal, 1, 0, 0);

				solverAddX(eight[3]->material, eight[2], solver, intersectionCount, accumNormal, 0, 1, 0);
				solverAddZ(eight[6]->material, eight[2], solver, intersectionCount, accumNormal, 0, 1, 0);

				solverAddX(eight[5]->material, eight[4], solver, intersectionCount, accumNormal, 0, 0, 1);
				solverAddY(eight[6]->material, eight[4], solver, intersectionCount, accumNormal, 0, 0, 1);

				solverAddZ(eight[7]->material, eight[3], solver, intersectionCount, accumNormal, 1, 1, 0);
				solverAddY(eight[7]->material, eight[5], solver, intersectionCount, accumNormal, 1, 0, 1);
				solverAddX(eight[7]->material, eight[6], solver, intersectionCount, accumNormal, 0, 1, 1);


				// step 4 : calculate the minimizer position
				if (intersectionCount > 0)
				{
					Vec3 res;
					solver.solve(res, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);
					if (res.x < 0 || res.x > 1.0f ||
						res.y < 0 || res.y > 1.0f ||
						res.z < 0 || res.z > 1.0f){
						VIS_DOT(x, y, z);
						VIS_VEC3(x, y + (float)eight[0]->intersections[1] / EDGE_SCALE, z, eight[0]->normal[1].x, eight[0]->normal[1].y, eight[0]->normal[1].z);
						VIS_VEC3(x + 1, y + (float)eight[1]->intersections[1] / EDGE_SCALE, z, eight[1]->normal[1].x, eight[1]->normal[1].y, eight[0]->normal[1].z);
						VIS_VEC3(x, y + (float)eight[4]->intersections[1] / EDGE_SCALE, z + 1, eight[4]->normal[1].x, eight[4]->normal[1].y, eight[0]->normal[1].z);
						VIS_VEC3(x + 1, y + (float)eight[5]->intersections[1] / EDGE_SCALE, z + 1, eight[5]->normal[1].x, eight[5]->normal[1].y, eight[0]->normal[1].z);
						
					}
					if (res.x != res.x || res.y != res.y || res.z != res.z){
						int sdf = 0;
					}
					res.x += x;
					res.y += y;
					res.z += z;
					
					tempVertices.push_back(vec3(res.x, res.y, res.z));
					// store the vertex Id to the index map.
					/*indexMap[idx] = vertexId;
					vertexId++;*/
					int idx2 = calcUsableIndex(x, y, z);
					indexMap[idx2] = vertexId;
					vertexId++;


					// average the normals accumulated from all the intersecting edges.
					accumNormal = glm::normalize(accumNormal);
					tempNormals.push_back(accumNormal);
					if (accumNormal.x != accumNormal.x || accumNormal.y != accumNormal.y || accumNormal.z != accumNormal.z){
						int sdf = 0;
					}

				}
			}
		}
	}
}

void VoxelChunk::generateIndices(){
	tempIndices.clear();
	VoxelData* eight[8];
	int intersectionCount;
	int vertexId = 0;
	for (int z = 0; z < UsableRange; z++){
		for (int y = 0; y < UsableRange; y++){
			for (int x = 0; x < UsableRange; x++){
				int idx = calcDataIndex(x, y, z);
				intersectionCount = 0;
				if (x == 1 && y == 1 && z == 0){
					int sdf = 0;
				}
				// step 1: get the pointers to the 8 voxels surrounding this cell.
				eight[0] = read(x, y, z);
				eight[1] = read(x + 1, y, z);
				eight[2] = read(x, y + 1, z);
				eight[3] = read(x + 1, y + 1, z);
				eight[4] = read(x, y, z + 1);
				eight[5] = read(x + 1, y, z + 1);
				eight[6] = read(x, y + 1, z + 1);
				eight[7] = read(x + 1, y + 1, z + 1);

				unsigned char baseMat = eight[0]->material;
				// step 2 : determine the indices from any crossing on the 3 minimal axis/edges.
				int xmin = -1, ymin = -1, zmin = -1;
				if (baseMat == 0 && eight[1]->material != 0)xmin = 1;
				else if (baseMat != 0 && eight[1]->material == 0)xmin = 0;
				if (baseMat == 0 && eight[2]->material != 0)ymin = 1;
				else if (baseMat != 0 && eight[2]->material == 0)ymin = 0;
				if (baseMat == 0 && eight[4]->material != 0)zmin = 1;
				else if (baseMat != 0 && eight[4]->material == 0)zmin = 0;


				if (intersectionCount > 0)
				{
					// store the vertex Id to the index map.
					/*int idx2 = calcIndex(x, y, z);
					indexMap[idx2] = vertexId;
					vertexId++;*/

				}
				/* step 5 : build a quad for any of the three minimal edge that has mismatching material ends,
				as indicated in step 2
				xmin, ymin, zmin somewhat indicate the normal at the intersection, as in,
				is the axis shooting into the surface, or shooting out of the surface?
				-1: the x axis/edge has no intersection;
				0: the x axis/edge is pointing into a surface.
				1: the x axis/edge is pointing from the inside of a surface.
				the same applies to the y and z axis/edges.
				pointing inwards/outwards a surface would dictate the order we wind the triangles.
				*/
				/*
				a note on the octree implementation:
				we would like to experiment with a method that attempts to solve the seams between different lod chunks.
				we need to first separate the generation of vertices and indices(quads) into two phases.
				the first phase would include the generation of vertices, minimizers using the qef method.
				in addition to this, we also duplicate the vertices that are on the minimal planes of a chunk
				to the chunk before it.
				so in the second phase in which we need to generate the indices(quads), we can generate quads
				between two chunks that are on different lod levels.

				*/

				if (xmin != -1){
					/*
					we also skip generating the indices(quad) when the running cell is at one of
					the minimal surfaces.
					*/
					//if (y != 0 && z != 0 && x != 0){
					if (y != 0 && z != 0){
						/*if (x == UsableRange){
						if (zmin != -1){
						// find the corresponding vc edge desc
						int v0 = readVertexIndex(x - 1, y, z);
						int v1 = readVertexIndex(x - 1, y - 1, z);
						VoxelChunkEdgeDesc* edgeDesc = &edgeDescs[0];
						if (edgeDesc->lodDiff == 2){

						}

						}
						}
						else*/{
							tempIndices.push_back(readVertexIndex(x, y, z));
							tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8], z + indexOrder[xmin * 8 + 1]));
							tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 2], z + indexOrder[xmin * 8 + 3]));
							tempIndices.push_back(readVertexIndex(x, y, z));
							tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 4], z + indexOrder[xmin * 8 + 5]));
							tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 6], z + indexOrder[xmin * 8 + 7]));
						}
					}
				}
				if (ymin != -1){
					//if (x != 0 && z != 0 && y != 0){
					if (x != 0 && z != 0){
						tempIndices.push_back(readVertexIndex(x, y, z));
						tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 1], y, z + indexOrder[ymin * 8]));
						tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 3], y, z + indexOrder[ymin * 8 + 2]));
						tempIndices.push_back(readVertexIndex(x, y, z));
						tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 5], y, z + indexOrder[ymin * 8 + 4]));
						tempIndices.push_back(readVertexIndex(x + indexOrder[ymin * 8 + 7], y, z + indexOrder[ymin * 8 + 6]));
					}
				}
				if (zmin != -1){
					//if (x != 0 && y != 0 && z != 0){
					if (x != 0 && y != 0){
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
	}

	//printf_s("vertices:");

	//for (int i = 0; i < tempVertices.size(); i++){
	//	printf_s("\n%f, %f, %f", tempVertices[i].x, tempVertices[i].y, tempVertices[i].z);
	//}
	//printf_s("\nindices:");

	//for (int i = 0; i < tempIndices.size(); i++){
	//	printf_s("%d, ", tempIndices[i]);
	//}
	//printf_s("\n");
}
void VoxelChunk::performSDF(SamplerFunction* sampler){
	const ivec3 minBound = sampler->getMinBound();
	const ivec3 maxBound = sampler->getMaxBound();
	// todo: change the loop order.
	for (int zi = minBound.z; zi <= maxBound.z; zi++){
		for (int yi = minBound.y; yi <= maxBound.y; yi++){
			for (int xi = minBound.x; xi <= maxBound.x; xi++){
				ivec3 pos = ivec3(xi, yi, zi);
				vec3 intersections;
				VoxelData voxel;

				voxel.material = sampler->materialFunc(pos, 1, &intersections, &voxel.normal[0], &voxel.normal[1], &voxel.normal[2]);
				voxel.intersections[0] = (unsigned char)(intersections.x * EDGE_SCALE);
				voxel.intersections[1] = (unsigned char)(intersections.y * EDGE_SCALE);
				voxel.intersections[2] = (unsigned char)(intersections.z * EDGE_SCALE);

				writeRaw(xi, yi, zi, voxel);
			}
		}
	}
}
void VoxelChunk::customSDF(int x, int y, int z, int w, SamplerFunction* sampler){
	ivec3 minBound = ivec3(x, y, z);
	int step = 1 << (w);
	for (int zi = 0; zi < VoxelChunk::DataRange; zi++){
		for (int yi = 0; yi < VoxelChunk::DataRange; yi++){
			for (int xi = 0; xi < VoxelChunk::DataRange; xi++){
				ivec3 pos = minBound + ivec3(xi, yi, zi) * step;
				vec3 intersections;
				VoxelData voxel;
				if (xi == 1 && yi == 1 && zi == 1){
					int sdf = 0;
				}

				voxel.material = sampler->materialFunc(pos, step, &intersections, &voxel.normal[0], &voxel.normal[1], &voxel.normal[2]);
				if (voxel.material != 0){
					int sdf = 0;
				}
				voxel.intersections[0] = (unsigned char)(intersections.x * EDGE_SCALE);
				voxel.intersections[1] = (unsigned char)(intersections.y * EDGE_SCALE);
				voxel.intersections[2] = (unsigned char)(intersections.z * EDGE_SCALE);

				writeRaw(xi, yi, zi, voxel);
			}
		}
	}
}
void VoxelChunk::writeRaw(int x, int y, int z, const VoxelData& vData){
	int idx = calcDataIndex(x, y, z);
	if (_data == nullptr)createDataArray();
	//_data[idx] = vData;

	_data[idx].intersections[0] = vData.intersections[0];
	_data[idx].intersections[1] = vData.intersections[1];
	_data[idx].intersections[2] = vData.intersections[2];

	_data[idx].material = vData.material;

	_data[idx].normal[0] = vData.normal[0];
	_data[idx].normal[1] = vData.normal[1];
	_data[idx].normal[2] = vData.normal[2];

}
void VoxelChunk::setAdjacentLod(int faceId, int alod){
	edgeDescs[faceId].init(faceId, alod);
}
// adjChunk should be at the negative side of *this chunk.
void VoxelChunk::createEdgeDesc(VoxelChunk* adjChunk, int loc0, int loc1){
	// this value should be derived from the positions and lod values of the two chunks.
	int edgeDescIdx = 0; 
	
	VoxelChunkEdgeDesc* edgeDesc;
	edgeDesc = &(adjChunk->edgeDescs[edgeDescIdx]);
	int indexWidth = UsableRange * 2;
	if (edgeDesc->indexMap == nullptr){
		// if adjChunk covers more volume than this(less detailed),
		// we want to make the indexMap twice as large as its original slice.
		edgeDesc->indexMap = new int[indexWidth * indexWidth];
		memset(edgeDesc->indexMap, 0, indexWidth * indexWidth * sizeof(int));
	}
	int vertIncre = adjChunk->tempVertices.size();
	// first: go with the z-plane
	for (int y = 0; y < UsableRange; y++){
		for (int x = 0; x < UsableRange; x++){
			int vidx = readVertexIndex(x, y, 0);
			if (vidx == -1)continue;
			vec3 vert = tempVertices[vidx];
			adjChunk->tempVertices.push_back(vert);
			vec3 normal = tempNormals[vidx];
			adjChunk->tempNormals.push_back(normal);
			int idx = x + loc0 + (y + loc1) * indexWidth;
			edgeDesc->indexMap[idx] = vertIncre;

			vertIncre++;
		}

	}
	
}