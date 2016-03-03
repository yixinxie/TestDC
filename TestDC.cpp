#include <stdio.h>
#include "qef.h"
#include "Octree.h"
#include "VoxelChunk.h"
#include "VoxelManager.h"
#include "MeshSerializer.h"
#include "HeightMapSampler.h"
#include "DCT.h"
#include "DCT3D.h"
#include "Orihsay/misc/Timer.h"
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
		sampler.setSpecs(vec3(0.5f, 0.5f, 0.5f), vec3(5.5f, 5.5f, 5.5f));
		//((SF_Box*)sampler)->setSpecs(vec3(1.5f, 1.5f, 1.5f), vec3(5.5f, 6.5f, 7.5f));


		/*SamplerFunction* heightmapSampler = new SF_Heightmap();
		((SF_Heightmap*)heightmapSampler)->loadPNG(ivec2(64, 64), "assets/dc2.png");
		((SF_Heightmap*)heightmapSampler)->setSpecs(ivec3(0, 0, 0), ivec3(62, 15, 62));*/

		VoxelManager vm;
		vm.initWorldSize(16, 16, 16);


		//vm.performSDF(sampler);
		vm.customSDF(0, 0, 0, 1, &sampler);
		// x+
		vm.customSDF(4, 0, 0, 0, &sampler);
		vm.customSDF(4, 2, 0, 0, &sampler);
		vm.customSDF(4, 0, 2, 0, &sampler);
		vm.customSDF(4, 2, 2, 0, &sampler);
		// hinge Z
		vm.customSDF(4, 4, 0, 0, &sampler);
		vm.customSDF(4, 4, 2, 0, &sampler);
		// y+
		vm.customSDF(0, 4, 0, 0, &sampler);
		vm.customSDF(2, 4, 0, 0, &sampler);
		vm.customSDF(0, 4, 2, 0, &sampler);
		vm.customSDF(2, 4, 2, 0, &sampler);
		// hinge X
		vm.customSDF(0, 4, 4, 0, &sampler);
		vm.customSDF(2, 4, 4, 0, &sampler);
		// z+
		vm.customSDF(0, 0, 4, 0, &sampler);
		vm.customSDF(2, 0, 4, 0, &sampler);
		vm.customSDF(0, 2, 4, 0, &sampler);
		vm.customSDF(2, 2, 4, 0, &sampler);
		// hinge Y
		vm.customSDF(4, 0, 4, 0, &sampler);
		vm.customSDF(4, 2, 4, 0, &sampler);
		ori::Timer t;
		vm.generateVertices();
		
		VoxelChunk* base = vm.readChunk(0, 0, 0, 1); // base chunk

		VoxelChunk* chunk0; 
		VoxelChunk* chunk1;
		VoxelChunk* chunk2;
		VoxelChunk* chunk3;
		{
			chunk0 = vm.readChunk(4, 0, 0, 0);
			chunk1 = vm.readChunk(4, 0, 2, 0);
			chunk2 = vm.readChunk(4, 2, 0, 0);
			chunk3 = vm.readChunk(4, 2, 2, 0);
			chunk0->createEdgeDesc2DUni(0, 0, 0, base, 1, 0);
			chunk1->createEdgeDesc2DUni(0, 1, 0, base, 1, 0);
			chunk2->createEdgeDesc2DUni(0, 0, 1, base, 1, 0);
			chunk3->createEdgeDesc2DUni(0, 1, 1, base, 1, 0);
		}
		{
			chunk0 = vm.readChunk(0, 4, 0, 0);
			chunk1 = vm.readChunk(2, 4, 0, 0);
			chunk2 = vm.readChunk(0, 4, 2, 0);
			chunk3 = vm.readChunk(2, 4, 2, 0);
			chunk0->createEdgeDesc2DUni(0, 0, 0, base, 1, 1);
			chunk1->createEdgeDesc2DUni(0, 1, 0, base, 1, 1);
			chunk2->createEdgeDesc2DUni(0, 0, 1, base, 1, 1);
			chunk3->createEdgeDesc2DUni(0, 1, 1, base, 1, 1);
		}
		{
			chunk0 = vm.readChunk(0, 0, 4, 0);
			chunk1 = vm.readChunk(2, 0, 4, 0);
			chunk2 = vm.readChunk(0, 2, 4, 0);
			chunk3 = vm.readChunk(2, 2, 4, 0);
			chunk0->createEdgeDesc2DUni(0, 0, 0, base, 1, 2);
			chunk1->createEdgeDesc2DUni(0, 1, 0, base, 1, 2);
			chunk2->createEdgeDesc2DUni(0, 0, 1, base, 1, 2);
			chunk3->createEdgeDesc2DUni(0, 1, 1, base, 1, 2);
		}
		// hinges
		{
			chunk0 = vm.readChunk(4, 4, 0, 0);
			chunk1 = vm.readChunk(4, 4, 2, 0);
			chunk0->createEdgeDesc1D(0, base, 0, 1, 5);
			chunk1->createEdgeDesc1D(0, base, 1, 1, 5);
		}
		{
			chunk0 = vm.readChunk(0, 4, 4, 0);
			chunk1 = vm.readChunk(2, 4, 4, 0);
			chunk0->createEdgeDesc1D(0, base, 0, 1, 3);
			chunk1->createEdgeDesc1D(0, base, 1, 1, 3);
		}
		{
			chunk0 = vm.readChunk(4, 0, 4, 0);
			chunk1 = vm.readChunk(4, 2, 4, 0);
			chunk0->createEdgeDesc1D(0, base, 0, 1, 4);
			chunk1->createEdgeDesc1D(0, base, 1, 1, 4);
		}

		/*VoxelChunk* leftTop = vm.readChunk(0, 4, 0, 0);
		VoxelChunk* rightTop = vm.readChunk(2, 4, 0, 0);
		VoxelChunk* hinge = vm.readChunk(4, 4, 0, 0);
		VoxelChunk* topRight = vm.readChunk(4, 2, 0, 0);
		VoxelChunk* bottomRight = vm.readChunk(4, 0, 0, 0);

		bottomRight->createEdgeDesc2DUni(0, 0, 0, base, 1, 0);
		topRight->createEdgeDesc2DUni(0, 0, 1, base, 1, 0);
		hinge->createEdgeDesc1D(0, base, 0, 1, 5);

		leftTop->createEdgeDesc2DUni(0, 0, 0, base, 1, 1);
		rightTop->createEdgeDesc2DUni(0, 1, 0, base, 1, 1);*/

		vm.generateIndices();
		long timeTaken = t.mark();
		printf_s("time elapsed: %d\n", timeTaken);
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