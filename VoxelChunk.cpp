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
VoxelChunk::VoxelChunk() :_data(nullptr){
}
void VoxelChunk::createDataArray(){
	_data = new VoxelData[VoxelConstants::DataRange * VoxelConstants::DataRange * VoxelConstants::DataRange];
	memset(_data, 0, sizeof(VoxelData) * VoxelConstants::DataRange * VoxelConstants::DataRange * VoxelConstants::DataRange);
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
	for (int zi = 0; zi < VoxelConstants::DataRange; zi++){
		for (int yi = 0; yi < VoxelConstants::DataRange; yi++){
			for (int xi = 0; xi < VoxelConstants::DataRange; xi++){
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
/*
we would like to experiment with a method that attempts to solve the seams between different lod chunks.
we need to first separate the generation of vertices and indices(quads) into two phases.
the first phase would include the generation of vertices, minimizers using the qef method.
in addition to this, we also duplicate the vertices that are on the minimal planes of a chunk
to the chunk before it.
so in the second phase in which we need to generate the indices(quads), we can generate quads
between two chunks that are on different lod levels.
*/
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
		int indexMapLength = VoxelConstants::UsableRange * VoxelConstants::UsableRange * VoxelConstants::UsableRange;
		memset(indexMap, -1, sizeof(indexMap));
	}
	for (int z = 0; z < VoxelConstants::UsableRange; z++){
		for (int y = 0; y < VoxelConstants::UsableRange; y++){
			for (int x = 0; x < VoxelConstants::UsableRange; x++){
				int idx = calcDataIndex(x, y, z);
				int usableIndex = calcUsableIndex(x, y, z);
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
				char xmin = -1, ymin = -1, zmin = -1;
				if (baseMat == 0 && eight[1]->material != 0)xmin = 1;
				else if (baseMat != 0 && eight[1]->material == 0)xmin = 0;
				if (baseMat == 0 && eight[2]->material != 0)ymin = 1;
				else if (baseMat != 0 && eight[2]->material == 0)ymin = 0;
				if (baseMat == 0 && eight[4]->material != 0)zmin = 1;
				else if (baseMat != 0 && eight[4]->material == 0)zmin = 0;

				edgeMap[usableIndex * 3] = xmin;
				edgeMap[usableIndex * 3 + 1] = ymin;
				edgeMap[usableIndex * 3 + 2] = zmin;

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
					
					indexMap[usableIndex] = vertexId;
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
	for (int z = 0; z < VoxelConstants::UsableRange; z++){
		for (int y = 0; y < VoxelConstants::UsableRange; y++){
			for (int x = 0; x < VoxelConstants::UsableRange; x++){
				int idx = calcDataIndex(x, y, z);
				int usableIndex = calcUsableIndex(x, y, z);
				if (x == 1 && y == 1 && z == 0){
					int sdf = 0;
				}

				int xmin = -1, ymin = -1, zmin = -1;

				xmin = edgeMap[usableIndex * 3];
				ymin = edgeMap[usableIndex * 3 + 1];
				zmin = edgeMap[usableIndex * 3 + 2];

				/* build a quad for any of the three minimal edge that has mismatching material ends,
				as indicated in step 2
				xmin, ymin, zmin somewhat indicate the normal at the intersection, as in,
				is the axis shooting into the surface, or shooting out of the surface?
				-1: the x axis/edge has no intersection;
				0: the x axis/edge is pointing into a surface.
				1: the x axis/edge is pointing from the inside of a surface.
				the same applies to the y and z axis/edges.
				pointing inwards/outwards a surface would dictate the order we wind the triangles.
				*/


				if (xmin != -1){
					/*
					we also skip generating the indices(quad) when the running cell is at one of
					the minimal surfaces.
					*/
					//if (y != 0 && z != 0 && x != 0){
					if (y != 0 && z != 0){
						tempIndices.push_back(readVertexIndex(x, y, z));
						tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8], z + indexOrder[xmin * 8 + 1]));
						tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 2], z + indexOrder[xmin * 8 + 3]));
						tempIndices.push_back(readVertexIndex(x, y, z));
						tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 4], z + indexOrder[xmin * 8 + 5]));
						tempIndices.push_back(readVertexIndex(x, y + indexOrder[xmin * 8 + 6], z + indexOrder[xmin * 8 + 7]));
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

	edgeDescs[0].gen2D(&tempIndices, false);
	edgeDescs[1].gen2D(&tempIndices, false);
	edgeDescs[2].gen2D(&tempIndices, true);

	/*generateEdge1D(0);
	generateEdge1D(1);
	generateEdge1D(2);*/
}

// to generate the 1D seam edge desc, we need to specify which of the three seams
// we define 3 being the edge that's parallel to the x axis,
// 4: parallel to the y axis
// 5: parallel to the z axis

void VoxelChunk::createEdgeDesc1D(int lodDiff, int loc0, VoxelChunk* adjChunk, int facing){
	int type = facing - 3;
	const int mapping[3] = { 0, 1, 2};
	const int _1D2DAdjacency[] = {
		2, 1,
		0, 2,
		1, 0,
	};
	VoxelChunkTransitionSurfaceDesc* edgeDesc;
	edgeDesc = &(adjChunk->edgeDescs[facing]);
	if (edgeDesc->initialized == false){
		
		// gather the two 2D transition desc
		VoxelChunkTransitionSurfaceDesc* left = &(adjChunk->edgeDescs[_1D2DAdjacency[type * 2]]);
		VoxelChunkTransitionSurfaceDesc* right = &(adjChunk->edgeDescs[_1D2DAdjacency[type * 2 + 1]]);
		edgeDesc->init1D(lodDiff, left, right);
		// now form the left and right column of the index map structure.
		if (type == 2){
			// here, because of the way we have determined dimInCells in edgeDesc,
			// it is guaranteed that length is at least as large as leftLength and rightLength.
			int length = edgeDesc->getDim();
			int leftLength = left->getDim();
			int leftScaler = (length > leftLength) ? 2 : 1;
			int leftLengthMinusOne = leftLength - 1;

			int rightLength = right->getDim();
			int rightScaler = (length > rightLength) ? 2 : 1;
			int rightLengthMinusOne = rightLength - 1;
			for (int c0 = 0; c0 < length; c0++){
				int leftScaledC0 = c0 / leftScaler;
				int leftIndex = left->readIndex2D(leftLengthMinusOne, leftScaledC0, 1);
				
				edgeDesc->writeIndex1D(c0, 1, leftIndex);
				// right
				int rightScaledC0 = c0 / rightScaler;
				int rightIndex = right->readIndex2D(rightScaledC0, rightLengthMinusOne, 1);
				edgeDesc->writeIndex1D(c0, 2, rightIndex);
			}
			// write from the base(adjChunk) index map.
			for (int c0 = 0; c0 < VoxelConstants::UsableRange; c0++){
				int adjUsableIndex = 0;
				if (type == 0)adjUsableIndex = calcUsableIndex(VoxelConstants::UsableRange - 1, VoxelConstants::UsableRange - 1, c0);
				else if (type == 1)adjUsableIndex = calcUsableIndex(c0, VoxelConstants::UsableRange - 1, VoxelConstants::UsableRange - 1);
				else if (type == 2)adjUsableIndex = calcUsableIndex(c0, VoxelConstants::UsableRange - 1, VoxelConstants::UsableRange - 1);

				edgeDesc->writeIndex1D(c0, 0, adjChunk->indexMap[adjUsableIndex]);
			}
		}
		
	}

	vec3 vertTranslate;
	if (facing == 3){
		vertTranslate.x = (float)(loc0 * VoxelConstants::UsableRange) * 0.5f;
		vertTranslate.y = VoxelConstants::UsableRange;
		vertTranslate.z = VoxelConstants::UsableRange;
	}
	else if (facing == 4){
		vertTranslate.x = VoxelConstants::UsableRange;
		vertTranslate.y = (float)(loc0 * VoxelConstants::UsableRange) * 0.5f;
		vertTranslate.z = VoxelConstants::UsableRange;
	}
	else if (facing == 5){
		vertTranslate.x = VoxelConstants::UsableRange;
		vertTranslate.y = VoxelConstants::UsableRange;
		vertTranslate.z = (float)(loc0 * VoxelConstants::UsableRange) * 0.5f;
	}

	int vertIncre = adjChunk->tempVertices.size();

	for (int c0 = 0; c0 < VoxelConstants::UsableRange; c0++){
		int usableIndex = 0;
		if (facing == 3)usableIndex = calcUsableIndex(c0, 0, 0);
		else if (facing == 4)usableIndex = calcUsableIndex(0, c0, 0);
		else if (facing == 5)usableIndex = calcUsableIndex(0, 0, c0);
		int vidx = indexMap[usableIndex];
		if (vidx == -1)continue;
		// first we get the vertex from this chunk's vertex list.
		vec3 vert = tempVertices[vidx];
		// transform this vertex with respect to the two lod values.
		vert *= edgeDesc->vertScale;
		vert += vertTranslate;
		// copy the vertex to adjChunk's vertex array,(and the normal too)
		adjChunk->tempVertices.push_back(vert);
		vec3 normal = tempNormals[vidx];
		adjChunk->tempNormals.push_back(normal);
		// then store the index in the edge desc structure.
		edgeDesc->writeIndex1D(c0 + loc0 * VoxelConstants::UsableRange, 3, vertIncre);
		vertIncre++;

		// now we copy the edge flags.
		edgeDesc->writeSeamEdge1D(c0 + loc0 * VoxelConstants::UsableRange, edgeMap[usableIndex * 3 + mapping[type]]);
		
	}
}

void VoxelChunk::createEdgeDesc2D(int lodDiff, int loc0, int loc1, VoxelChunk* adjChunk, int facing)
{
	// this value should be derived from the positions and lod values of the two chunks.

	VoxelChunkTransitionSurfaceDesc* edgeDesc;
	edgeDesc = &(adjChunk->edgeDescs[facing]);
	if (edgeDesc->initialized == false){
		edgeDesc->init(lodDiff, 0);
		int scaler = edgeDesc->getDim() / VoxelConstants::UsableRange;
		// copy the original index from the adjacent chunk to the edge desc structure.
		for (int c1 = 0; c1 < VoxelConstants::UsableRange; c1++){
			for (int c0 = 0; c0 < VoxelConstants::UsableRange; c0++){

				int otherIdx = c0 + c1 * VoxelConstants::UsableRange;
				int adjUsableIndex = 0;
				if (facing == 0)adjUsableIndex = calcUsableIndex(VoxelConstants::UsableRange - 1, c1, c0);
				else if (facing == 1)adjUsableIndex = calcUsableIndex(c0, VoxelConstants::UsableRange - 1, c1);
				else if (facing == 2)adjUsableIndex = calcUsableIndex(c0, c1, VoxelConstants::UsableRange - 1);

				int indexValue = adjChunk->indexMap[adjUsableIndex];
				//edgeDesc->writeBaseIndex2D(c0, c1, );
				if (scaler == 1){
					edgeDesc->writeIndex2D(c0, c1, 0, indexValue);
				}
				else{
					edgeDesc->writeIndex2D(c0 * 2, c1 * 2, 0, indexValue);
					edgeDesc->writeIndex2D(c0 * 2 + 1, c1 * 2, 0, indexValue);
					edgeDesc->writeIndex2D(c0 * 2, c1 * 2 + 1, 0, indexValue);
					edgeDesc->writeIndex2D(c0 * 2 + 1, c1 * 2 + 1, 0, indexValue);
				}
			}
		}
	}

	vec3 vertTranslate;
	if (facing == 0){
		vertTranslate.x = VoxelConstants::UsableRange;
		vertTranslate.y = (float)(loc1 * VoxelConstants::UsableRange) * 0.5f;
		vertTranslate.z = (float)(loc0 * VoxelConstants::UsableRange) * 0.5f;
	}
	else if (facing == 1){
		vertTranslate.x = (float)(loc0 * VoxelConstants::UsableRange) * 0.5f;
		vertTranslate.y = VoxelConstants::UsableRange;
		vertTranslate.z = (float)(loc1 * VoxelConstants::UsableRange) * 0.5f;
	}
	else if (facing == 2){
		vertTranslate.x = (float)(loc0 * VoxelConstants::UsableRange) * 0.5f;
		vertTranslate.y = (float)(loc1 * VoxelConstants::UsableRange) * 0.5f;
		vertTranslate.z = VoxelConstants::UsableRange;
	}
}
void VoxelChunk::copyIndicesAndEdgeFlags(const vec3& vertTranslate, VoxelChunk* adjChunk, int loc0, int loc1, VoxelChunkTransitionSurfaceDesc* edgeDesc, int facing){
	int vertIncre = adjChunk->tempVertices.size();
	int scaler = edgeDesc->getDim() / VoxelConstants::UsableRange;
	const int mapping[6] = { 2, 1, 0, 2, 0, 1 };
	for (int c1 = 0; c1 < VoxelConstants::UsableRange; c1++){
		for (int c0 = 0; c0 < VoxelConstants::UsableRange; c0++){
			int usableIndex = 0;
			if (facing == 0)usableIndex = calcUsableIndex(0, c1, c0);
			else if (facing == 1)usableIndex = calcUsableIndex(c0, 0, c1);
			else if (facing == 2)usableIndex = calcUsableIndex(c0, c1, 0);

			int vidx = indexMap[usableIndex];
			if (vidx == -1)continue;
			// first we get the vertex from this chunk's vertex list.
			vec3 vert = tempVertices[vidx];
			// transform this vertex with respect to the two lod values.
			vert *= edgeDesc->vertScale;
			vert += vertTranslate;
			// step 1: copy the vertex to adjChunk's vertex array,(and the normal too)
			adjChunk->tempVertices.push_back(vert);
			vec3 normal = tempNormals[vidx];
			adjChunk->tempNormals.push_back(normal);
			// step 2: then store the index in the edge desc structure.
			edgeDesc->writeIndex2D(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange, 1, vertIncre);
			vertIncre++;
			
			// now we copy the edge flags.
			// first x then z.
			edgeDesc->writeSeamEdgeXY(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange, 
				edgeMap[usableIndex * 3 + mapping[facing * 2]],
				edgeMap[usableIndex * 3 + mapping[facing * 2 + 1]]);
		}
	}
}
void VoxelChunk::duplicateIndicesAndEdgeFlags(const vec3& vertTranslate, VoxelChunk* adjChunk ,int loc0, int loc1, VoxelChunkTransitionSurfaceDesc* edgeDesc, int facing){
	const int mapping[6] = { 2, 1, 0, 2, 0, 1 };
	int halfUsableRange = VoxelConstants::UsableRange / 2;

	int vertIncre = adjChunk->tempVertices.size();
	int scaler = edgeDesc->getDim() / VoxelConstants::UsableRange;
	const int mapping[6] = { 2, 1, 0, 2, 0, 1 };
	for (int c1 = 0; c1 < halfUsableRange; c1++){
		for (int c0 = 0; c0 < halfUsableRange; c0++){
			int usableIndex = 0;
			if (facing == 0)usableIndex = calcUsableIndex(0, c1 + loc1 * halfUsableRange, c0 + loc0 * halfUsableRange);
			else if (facing == 1)usableIndex = calcUsableIndex(c0 + loc0 * halfUsableRange, 0, c1 + loc1 * halfUsableRange);
			else if (facing == 2)usableIndex = calcUsableIndex(c0 + loc0 * halfUsableRange, c1 + loc1 * halfUsableRange, 0);

			int vidx = indexMap[usableIndex];
			if (vidx == -1)continue;
			// first we get the vertex from this chunk's vertex list.
			vec3 vert = tempVertices[vidx];
			// transform this vertex with respect to the two lod values.
			vert *= edgeDesc->vertScale;
			vert += vertTranslate;
			// step 1: copy the vertex to adjChunk's vertex array,(and the normal too)
			adjChunk->tempVertices.push_back(vert);
			vec3 normal = tempNormals[vidx];
			adjChunk->tempNormals.push_back(normal);
			// step 2: then store the index in the edge desc structure.
			edgeDesc->writeIndex2D(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange, 1, vertIncre);
			edgeDesc->writeIndex2D(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange, 1, vertIncre);
			edgeDesc->writeIndex2D(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange, 1, vertIncre);
			edgeDesc->writeIndex2D(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange, 1, vertIncre);
			vertIncre++;

			// now we copy the edge flags.
			// first x then z.
			char edgeX = edgeMap[usableIndex * 3 + mapping[facing * 2]];
			char edgeY = edgeMap[usableIndex * 3 + mapping[facing * 2 + 1]];
			edgeDesc->writeSeamEdgeXY(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange,
				edgeX, edgeY);
			edgeDesc->writeSeamEdgeXY(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange,
				edgeX, edgeY);
			edgeDesc->writeSeamEdgeXY(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange,
				edgeX, edgeY);
			edgeDesc->writeSeamEdgeXY(c0 + loc0 * VoxelConstants::UsableRange, c1 + loc1 * VoxelConstants::UsableRange,
				edgeX, edgeY);
		}

	}
}