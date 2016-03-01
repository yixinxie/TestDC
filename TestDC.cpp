#include <stdio.h>
#include "qef.h"
#include "Octree.h"
#include "VoxelChunk.h"
#include "VoxelManager.h"
#include "MeshSerializer.h"
#include "HeightMapSampler.h"
#include "DCT.h"
#include "DCT3D.h"
using namespace svd;
void main(void){
	if (false){
		const float QEF_ERROR = 1e-6f;
		const int QEF_SWEEPS = 4;

		QefSolver solver;
		Vec3 res;
		Vec3 n0(-0.5f, 0.5f, 0);
		Vec3 n1(0.5f, 0.5f, 0);
		n0.normalize();
		n1.normalize();
		solver.add(Vec3(0, 0.5f, 0), n0);
		solver.add(Vec3(1.0f, 0, 0), n1);
		solver.solve(res, QEF_ERROR, QEF_SWEEPS, QEF_ERROR);

		printf_s("%f, %f, %f", res.x, res.y, res.z);
		QefData qefData = solver.getData();
		const Vec3& massPoint = solver.getMassPoint();
		//printf_s("%f, %f, %f\n", res.x, res.y, res.z);
		printf_s("mass point :%f, %f, %f", massPoint.x, massPoint.y, massPoint.z);
	/*	OctreeNode root(16, 0, 0, 0);
		root.performSDF(sampler);
		OctreeNode::generateMinimizers(&root);*/
	}
	
	if(true){
		SF_Box sampler;// = new SF_Box();
		sampler.setSpecs(vec3(0.5f, 0.5f, 0.5f), vec3(7, 7, 7));
		//((SF_Box*)sampler)->setSpecs(vec3(1.5f, 1.5f, 1.5f), vec3(5.5f, 6.5f, 7.5f));


		/*SamplerFunction* heightmapSampler = new SF_Heightmap();
		((SF_Heightmap*)heightmapSampler)->loadPNG(ivec2(64, 64), "assets/dc2.png");
		((SF_Heightmap*)heightmapSampler)->setSpecs(ivec3(0, 0, 0), ivec3(62, 15, 62));*/

		VoxelManager vm;
		vm.initWorldSize(16, 16, 16);


		//vm.performSDF(sampler);
		vm.customSDF(0, 0, 0, 1, &sampler);
		vm.customSDF(4, 0, 0, 0, &sampler);
		vm.customSDF(4, 2, 0, 0, &sampler);

		vm.customSDF(4, 4, 0, 0, &sampler);
		vm.customSDF(0, 4, 0, 0, &sampler);
		vm.customSDF(2, 4, 0, 0, &sampler);
		//vm.customSDF(-4, 4, 0, 2, &sampler);
		vm.generateVertices();
		VoxelChunk* left = vm.readChunk(0, 0, 0, 1); // base chunk

		VoxelChunk* topRight = vm.readChunk(4, 2, 0, 0);
		VoxelChunk* bottomRight = vm.readChunk(4, 0, 0, 0);
		VoxelChunk* hinge = vm.readChunk(4, 4, 0, 0);
		VoxelChunk* leftTop = vm.readChunk(0, 4, 0, 0);
		VoxelChunk* rightTop = vm.readChunk(2, 4, 0, 0);
		
		topRight->createEdgeDesc2D(0, left, 0, 1, 1, 0);
		bottomRight->createEdgeDesc2D(0, left, 0, 0, 1, 0);
		hinge->createEdgeDesc1D(0, left, 0, 1, 5);

		leftTop->createEdgeDesc2D(0, left, 0, 0, 1, 1);
		rightTop->createEdgeDesc2D(0, left, 1, 0, 1, 1);

		vm.generateIndices();
		vm.exportJson();
		//delete heightmapSampler;
	}
	if (false){
		DCT3D dct;
		dct.init();
		dct.encode();
		dct.quantize(2);
		//dct.quantize(3);
		dct.decode();
		printf_s("error: %f", dct.calcError());
	}
	printf_s("done.");
	getchar();
}