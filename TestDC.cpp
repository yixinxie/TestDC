#include <stdio.h>
#include "qef.h"
#include "Octree.h"
#include "VoxelChunk.h"
#include "VoxelManager.h"
#include "MeshSerializer.h"
#include "HeightMapSampler.h"
#include "DCT.h"
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
	
	if(false){
		SamplerFunction* sampler = new SF_Box();
		//((SF_Box*)sampler)->setSpecs(vec3(0.5f, 0.5f, 0.5f), vec3(1.5f, 1.5f, 1.5f));
		((SF_Box*)sampler)->setSpecs(vec3(1.5f, 1.5f, 1.5f), vec3(5.5f, 6.5f, 7.5f));


		SamplerFunction* heightmapSampler = new SF_Heightmap();
		((SF_Heightmap*)heightmapSampler)->loadPNG(ivec2(64, 64), "assets/dc2.png");
		((SF_Heightmap*)heightmapSampler)->setSpecs(ivec3(0, 0, 0), ivec3(62, 15, 62));

		VoxelManager vm;
		vm.initWorldSize(64, 16, 64);

		vm.performSDF(heightmapSampler);
		vm.generateMeshes();
		getchar();
		delete sampler;
		delete heightmapSampler;
	}
	DCT dct;
	dct.init();
	dct.encode();
	//dct.quantize(6);
	dct.quantize(3);
	dct.decode();
	printf_s("error: %f", dct.calcError());
	getchar();
}