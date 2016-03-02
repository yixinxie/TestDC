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
	_data = new VoxelData[DataRange * DataRange * DataRange];
	memset(_data, 0, sizeof(VoxelData) * DataRange * DataRange * DataRange);
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
		int indexMapLength = UsableRange * UsableRange * UsableRange;
		for (int i = 0; i < indexMapLength; i++){
			indexMap[i] = -1;
		}
	}
	for (int z = 0; z < UsableRange; z++){
		for (int y = 0; y < UsableRange; y++){
			for (int x = 0; x < UsableRange; x++){
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
	for (int z = 0; z < UsableRange; z++){
		for (int y = 0; y < UsableRange; y++){
			for (int x = 0; x < UsableRange; x++){
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
	gen2DX(&edgeDescs[0], edgeDescs[0].getDim());
	gen2DY(&edgeDescs[1], edgeDescs[1].getDim());
	gen2DZ(&edgeDescs[2], edgeDescs[2].getDim());

	generateEdge1D(0);
}

void VoxelChunk::gen2DX(VoxelChunkEdgeDesc* edgeDesc, int dim){
	for (int y = 1; y < dim; y++){
		for (int z = 0; z < dim; z++){
			int baseZ = z >> 1, baseY = y >> 1;
			// first read the vertex index from the original index table.
			int ind0 = readVertexIndex(VoxelChunk::UsableRange - 1, baseY, baseZ);  // ifs
			int ind3 = readVertexIndex(VoxelChunk::UsableRange - 1, baseY - 1, baseZ);  // ifs
			if (y % 2 == 1){
				edgeDesc->gen2D_x_tri(z, y, &tempIndices, ind0, false);
			}
			else{
				edgeDesc->gen2D_x_quad(z, y, &tempIndices, ind0, ind3, false);
			}
		}
	}
	//???
	for (int y = 0; y < dim; y++){
		for (int z = 1; z < dim; z++){
			int baseZ = z >> 1, baseY = y >> 1;
			// first read the vertex index from the original index table.
			int ind0 = readVertexIndex(VoxelChunk::UsableRange - 1, baseY, baseZ);  // ifs
			int ind3 = readVertexIndex(VoxelChunk::UsableRange - 1, baseY, baseZ - 1);  // ifs
			if (z % 2 == 1){
				edgeDesc->gen2D_y_tri(z, y, &tempIndices, ind0, false);
			}
			else{
				edgeDesc->gen2D_y_quad(z, y, &tempIndices, ind0, ind3, false);
			}
		}
	}
}
void VoxelChunk::gen2DY(VoxelChunkEdgeDesc* edgeDesc, int dim){
	for (int z = 1; z < dim; z++){
		for (int x = 0; x < dim; x++){
			int baseX = x >> 1, baseZ = z >> 1;
			int ind0 = readVertexIndex(baseX, VoxelChunk::UsableRange - 1, baseZ);
			int ind3 = readVertexIndex(baseX, VoxelChunk::UsableRange - 1, baseZ - 1);
			if (z % 2 == 1){
				edgeDesc->gen2D_x_tri(x, z, &tempIndices, ind0, false);
			}
			else{
				edgeDesc->gen2D_x_quad(x, z, &tempIndices, ind0, ind3, false);
			}
		}
	}
	for (int z = 0; z < dim; z++){
		for (int x = 1; x < dim; x++){
			int baseX = x >> 1, baseZ = z >> 1;
			int ind0 = readVertexIndex(baseX, VoxelChunk::UsableRange - 1, baseZ);
			int ind3 = readVertexIndex(baseX - 1, VoxelChunk::UsableRange - 1, baseZ);
			if (x % 2 == 1){
				edgeDesc->gen2D_y_tri(x, z, &tempIndices, ind0, true);
			}
			else{
				edgeDesc->gen2D_y_quad(x, z, &tempIndices, ind0, ind3, true);
			}
		}
	}

}
void VoxelChunk::gen2DZ(VoxelChunkEdgeDesc* edgeDesc, int dim){
	//???
	for (int y = 1; y < dim; y++){
		for (int x = 0; x < dim; x++){
			int baseX = x >> 1, baseY = y >> 1;
			int ind0 = readVertexIndex(baseX, baseY, VoxelChunk::UsableRange - 1);
			int ind3 = readVertexIndex(baseX, baseY - 1, VoxelChunk::UsableRange - 1);
			if (y % 2 == 1){
				edgeDesc->gen2D_x_tri(x, y, &tempIndices, ind0, false);
			}
			else{
				edgeDesc->gen2D_x_quad(x, y, &tempIndices, ind0, ind3, false);
			}
		}
	}
	//???
	for (int y = 0; y < dim; y++){
		for (int x = 1; x < dim; x++){
			int baseX = x >> 1, baseY = y >> 1;
			int ind0 = readVertexIndex(baseX, baseY, VoxelChunk::UsableRange - 1);
			int ind3 = readVertexIndex(baseX - 1, baseY, VoxelChunk::UsableRange - 1);
			if (x % 2 == 1){
				edgeDesc->gen2D_y_tri(x, y, &tempIndices, ind0, false);
			}
			else{
				edgeDesc->gen2D_y_quad(x, y, &tempIndices, ind0, ind3, false);
			}
		}
	}

}

void VoxelChunk::generateEdge1D(int facing){
	const int mapping[] = {
		5, 1, 0, // hinge that lies paralelle to the z axis, x+ plane, y+ plane
		3, 1, 2,
		4, 2, 0,
	};
	VoxelChunkEdgeDesc* edgeDescHinge = &edgeDescs[mapping[facing * 3 + 0]];
	VoxelChunkEdgeDesc* edgeDescC0 = &edgeDescs[mapping[facing * 3 + 2]];
	VoxelChunkEdgeDesc* edgeDescC1 = &edgeDescs[mapping[facing * 3 + 1]];
	
	if (edgeDescHinge->lodDiff != -1 && edgeDescC0->lodDiff != -1 && edgeDescC1->lodDiff != -1){
		int incr = 0;
		int usableRangeMinusOne = UsableRange - 1;
		int* px;
		int* py;
		int* pz;
		if (facing == 0){
			px = &usableRangeMinusOne;
			py = &usableRangeMinusOne;
			pz = &incr;
		}
		else if (facing == 1){
			px = &incr;
			py = &usableRangeMinusOne;
			pz = &usableRangeMinusOne;
		}
		else if (facing == 2){
			px = &usableRangeMinusOne;
			py = &incr;
			pz = &usableRangeMinusOne;
		}
		for (; incr < UsableRange; incr++){
			int edgeCellIndex = edgeDescHinge->calcIndex(*pz * 2, 0);
			char zmin = edgeDescHinge->seamEdges[edgeCellIndex * 2];
			if (zmin == -1)continue;
			int ind3 = edgeDescHinge->indexMap[edgeCellIndex];
			// read the index from the base.
			int ind0 = readVertexIndex(*px, *py, *pz);

			int toIdx = edgeDescC0->calcIndex(incr * 2, UsableRange * 2 - 1);
			int ind1 = edgeDescC0->indexMap[toIdx];

			toIdx = edgeDescC1->calcIndex(UsableRange * 2 - 1, incr * 2);
			int ind2 = edgeDescC1->indexMap[toIdx];

			// this time we need to wind 2 triangles, or a quad.
			if (zmin == 0){
				tempIndices.push_back(ind0);
				tempIndices.push_back(ind1);
				tempIndices.push_back(ind3);

				tempIndices.push_back(ind3);
				tempIndices.push_back(ind2);
				tempIndices.push_back(ind0);
			}
			else if (zmin == 1){
				tempIndices.push_back(ind0);
				tempIndices.push_back(ind3);
				tempIndices.push_back(ind1);

				tempIndices.push_back(ind3);
				tempIndices.push_back(ind0);
				tempIndices.push_back(ind2);

			}
		}
	}
}

// adjChunk should be at the negative side of *this chunk.
// a generalized version
void VoxelChunk::createEdgeDesc2D(int thisLod, VoxelChunk* adjChunk, int loc0, int loc1, int adjLod, int facing){
	// this value should be derived from the positions and lod values of the two chunks.
	const int mapping[6] = {2, 1, 0, 2, 0, 1};
	VoxelChunkEdgeDesc* edgeDesc;
	edgeDesc = &(adjChunk->edgeDescs[facing]);
	if (edgeDesc->lodDiff == -1){
		edgeDesc->init(thisLod, adjLod);
	}

	vec3 vertTranslate;
	if (facing == 0){
		vertTranslate.x = UsableRange;
		vertTranslate.y = (float)(loc1 * UsableRange) * 0.5f;
		vertTranslate.z = (float)(loc0 * UsableRange) * 0.5f;
	}
	else if (facing == 1){
		vertTranslate.x = (float)(loc0 * UsableRange) * 0.5f;
		vertTranslate.y = UsableRange;
		vertTranslate.z = (float)(loc1 * UsableRange) * 0.5f;
	}
	else if (facing == 2){
		vertTranslate.x = (float)(loc0 * UsableRange) * 0.5f;
		vertTranslate.y = (float)(loc1 * UsableRange) * 0.5f;
		vertTranslate.z = UsableRange;
	}

	int vertIncre = adjChunk->tempVertices.size();

	for (int c1 = 0; c1 < UsableRange; c1++){
		for (int c0 = 0; c0 < UsableRange; c0++){
			int usableIndex = 0;
			if(facing == 0)usableIndex = calcUsableIndex(0, c1, c0);
			else if (facing == 1)usableIndex = calcUsableIndex(c0, 0, c1);
			else if (facing == 2)usableIndex = calcUsableIndex(c0, c1, 0);
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
			int idx = edgeDesc->calcIndex(c0 + loc0 * UsableRange, c1 + loc1 * UsableRange);
			edgeDesc->indexMap[idx] = vertIncre;
			vertIncre++;

			// now we copy the edge flags.
			// first x then z.
			edgeDesc->seamEdges[idx * 2] = edgeMap[usableIndex * 3 + mapping[facing * 2]];
			edgeDesc->seamEdges[idx * 2 + 1] = edgeMap[usableIndex * 3 + mapping[facing * 2 + 1]];
		}
	}
}
// to generate the 1D seam edge desc, we need to specify which of the three seams
// we define 3 being the edge that's parallel to the x axis, and so on.

void VoxelChunk::createEdgeDesc1D(int thisLod, VoxelChunk* adjChunk, int loc0, int adjLod, int facing){
	// this value should be derived from the positions and lod values of the two chunks.
	const int mapping[3] = { 0, 1, 2};
	VoxelChunkEdgeDesc* edgeDesc;
	edgeDesc = &(adjChunk->edgeDescs[facing]);
	if (edgeDesc->lodDiff == -1){
		edgeDesc->init(thisLod, adjLod);
	}

	vec3 vertTranslate;
	if (facing == 3){
		
		vertTranslate.x = (float)(loc0 * UsableRange) * 0.5f;
		vertTranslate.y = UsableRange;
		vertTranslate.z = UsableRange;
	}
	else if (facing == 4){
		vertTranslate.x = UsableRange;
		vertTranslate.y = (float)(loc0 * UsableRange) * 0.5f;
		vertTranslate.z = UsableRange;
	}
	else if (facing == 5){
		vertTranslate.x = UsableRange;
		vertTranslate.y = UsableRange;
		vertTranslate.z = (float)(loc0 * UsableRange) * 0.5f;
	}

	int vertIncre = adjChunk->tempVertices.size();

	for (int c0 = 0; c0 < UsableRange; c0++){
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
		int idx = edgeDesc->calcIndex(c0 + loc0 * UsableRange, 0);
		edgeDesc->indexMap[idx] = vertIncre;
		vertIncre++;

		// now we copy the edge flags.
		edgeDesc->seamEdges[idx * 2] = edgeMap[usableIndex * 3 + mapping[facing - 3]];
	}
}
void VoxelChunk::createEdgeDesc0D(int thisLod, VoxelChunk* adjChunk, int adjLod){
	// this value should be derived from the positions and lod values of the two chunks.
	VoxelChunkEdgeDesc* edgeDesc;
	edgeDesc = &(adjChunk->edgeDescs[6]);
	if (edgeDesc->lodDiff == -1){
		edgeDesc->init(thisLod, adjLod);
	}

	vec3 vertTranslate;
	vertTranslate.x = UsableRange;
	vertTranslate.y = UsableRange;
	vertTranslate.z = UsableRange;

	int vertIncre = adjChunk->tempVertices.size();

	int usableIndex = 0;
	usableIndex = calcUsableIndex(0, 0, 0);
	int vidx = indexMap[usableIndex];
	if (vidx == -1)return;
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
	int idx = edgeDesc->calcIndex(0, 0);
	edgeDesc->indexMap[idx] = vertIncre;
	vertIncre++;

	// since we dont really need the edge information here, we dont copy the flag.
	//edgeDesc->seamEdges[idx * 2] = edgeMap[usableIndex * 3 + mapping[facing]];
}
