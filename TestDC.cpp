#include <stdio.h>
#include "qef.h"
#include "Orihsay/misc/Timer.h"
#include "Octree.h"
#include "VoxelChunk.h"
#include "VoxelManager.h"
#include "MeshSerializer.h"
#include "HeightMapSampler.h"
#include "ClipmapRing.h"
#include "DCT.h"
#include "DCT3D.h"
#include "VoxelChunkManager.h"

using namespace svd;
void testQEF(void){
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
void testDCT(void){
	DCT3D dct;
	dct.init();
	dct.encode();
	dct.quantize(2);
	//dct.quantize(3);
	dct.decode();
	printf_s("error: %f", dct.calcError());
}
void testClipmap(void){
	ClipmapRing ring(nullptr);
	ring.initPos(ivec3(0, 0, 0), ivec3(4, 0, 0), 1);
	ring.update(ivec3(2, 0, 0), ivec3(4, 0, 0));
	printf_s("-----------------------------------------------\n %d", (-5 % 3));
	//ring.reposition(ivec3(2, 2, 0), ivec3(4, 2, 0));
}
void main(void){
	// third experiment
	char* tt = new char[100];
	for (int i = 0; i < 100; i++){
		tt[i] = -1;
	}
	delete tt;
	if (true){
		SF_Box sampler;// = new SF_Box();
		sampler.setSpecs(vec3(2.25f, 2.25f, 2.25f), vec3(6.5f, 6.5f, 6.5f));
		//((SF_Box*)sampler)->setSpecs(vec3(1.5f, 1.5f, 1.5f), vec3(5.5f, 6.5f, 7.5f));


		/*SamplerFunction* heightmapSampler = new SF_Heightmap();
		((SF_Heightmap*)heightmapSampler)->loadPNG(ivec2(64, 64), "assets/dc2.png");
		((SF_Heightmap*)heightmapSampler)->setSpecs(ivec3(0, 0, 0), ivec3(62, 15, 62));*/

		VoxelManager vm;
		vm.initWorldSize(16, 16, 16);


		//vm.performSDF(sampler);
		vm.customSDF(2, 2, 2, 0, &sampler);
		// x+
		vm.customSDF(4, 0, 0, 1, &sampler);
		// hinge Z
		vm.customSDF(4, 4, 0, 1, &sampler);
		// y+
		vm.customSDF(0, 4, 0, 1, &sampler);
		// hinge X
		vm.customSDF(0, 4, 4, 1, &sampler);
		// z+
		vm.customSDF(0, 0, 4, 1, &sampler);
		// hinge Y
		vm.customSDF(4, 0, 4, 1, &sampler);
		// diagonal
		//vm.customSDF(4, 4, 4, 1, &sampler);
		ori::Timer t;
		vm.generateVertices();

		VoxelChunk* base = vm.readChunk(2, 2, 2, 0); // base chunk

		VoxelChunk* chunk0;
		VoxelChunk* chunk1;
		VoxelChunk* chunk2;

		{
			chunk0 = vm.readChunk(4, 0, 0, 1);
			chunk1 = vm.readChunk(0, 4, 0, 1);
			chunk2 = vm.readChunk(0, 0, 4, 1);
			chunk0->createEdgeDesc2D(1, 1, 1, base, 0);
			chunk1->createEdgeDesc2D(1, 1, 1, base, 1);
			chunk2->createEdgeDesc2D(1, 1, 1, base, 2);
		}
		// hinges
		if (false)
		{
			// paralelle to the z-axis
			chunk0 = vm.readChunk(4, 4, 0, 1);
			chunk0->createEdgeDesc1D(1, 1, base, 5);
			// paralelle to the x-axis
			chunk0 = vm.readChunk(0, 4, 4, 1);
			chunk0->createEdgeDesc1D(1, 1, base, 3);
			// paralelle to the y-axis
			chunk0 = vm.readChunk(4, 0, 4, 1);
			chunk0->createEdgeDesc1D(1, 1, base, 4);
		}
		vm.generateIndices();
		long timeTaken = t.mark();
		printf_s("time elapsed: %d\n", timeTaken);
		vm.exportJson();
		//delete heightmapSampler;
	}
	// 1:1
	if (false){
		SF_Box sampler;// = new SF_Box();
		sampler.setSpecs(vec3(0.5f, 0.5f, 0.5f), vec3(5.5f, 5.5f, 5.5f));

		VoxelManager vm;
		vm.initWorldSize(16, 16, 16);

		//vm.performSDF(sampler);
		vm.customSDF(0, 0, 0, 1, &sampler);
		// x+
		vm.customSDF(4, 0, 0, 1, &sampler);
		// hinge Z
		vm.customSDF(4, 4, 0, 1, &sampler);
		// y+
		vm.customSDF(0, 4, 0, 1, &sampler);
		// hinge X
		vm.customSDF(0, 4, 4, 1, &sampler);
		// z+
		vm.customSDF(0, 0, 4, 1, &sampler);
		// hinge Y
		vm.customSDF(4, 0, 4, 1, &sampler);
		ori::Timer t;
		vm.generateVertices();

		VoxelChunk* base = vm.readChunk(0, 0, 0, 1); // base chunk

		VoxelChunk* chunk0;
		{
			chunk0 = vm.readChunk(4, 0, 0, 1);
			chunk0->createEdgeDesc2D(0, 0, 0, base, 0);
		}
		{
			chunk0 = vm.readChunk(0, 4, 0, 1);
			chunk0->createEdgeDesc2D(0, 0, 0, base, 1);
		}
		{
			chunk0 = vm.readChunk(0, 0, 4, 1);
			chunk0->createEdgeDesc2D(0, 0, 0, base, 2);
		}
		// hinges
		{
			// paralelle to the z-axis
			chunk0 = vm.readChunk(4, 4, 0, 1);
			chunk0->createEdgeDesc1D(0, 0, base, 5);
		}
		{
			// paralelle to the x-axis
			chunk0 = vm.readChunk(0, 4, 4, 1);
			chunk0->createEdgeDesc1D(0, 0, base, 3);
		}
		{
			// paralelle to the y-axis
			chunk0 = vm.readChunk(4, 0, 4, 1);
			chunk0->createEdgeDesc1D(0, 0, base, 4);
		}
		vm.generateIndices();
		long timeTaken = t.mark();
		printf_s("time elapsed: %d\n", timeTaken);
		vm.exportJson();
		//delete heightmapSampler;
	}
	// first experiment
	if(false){
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
			chunk0->createEdgeDesc2D(-1, 0, 0, base, 0);
			chunk1->createEdgeDesc2D(-1, 1, 0, base, 0);
			chunk2->createEdgeDesc2D(-1, 0, 1, base, 0);
			chunk3->createEdgeDesc2D(-1, 1, 1, base, 0);
		}
		{
			chunk0 = vm.readChunk(0, 4, 0, 0);
			chunk1 = vm.readChunk(2, 4, 0, 0);
			chunk2 = vm.readChunk(0, 4, 2, 0);
			chunk3 = vm.readChunk(2, 4, 2, 0);
			chunk0->createEdgeDesc2D(-1, 0, 0, base, 1);
			chunk1->createEdgeDesc2D(-1, 1, 0, base, 1);
			chunk2->createEdgeDesc2D(-1, 0, 1, base, 1);
			chunk3->createEdgeDesc2D(-1, 1, 1, base, 1);
		}
		{
			chunk0 = vm.readChunk(0, 0, 4, 0);
			chunk1 = vm.readChunk(2, 0, 4, 0);
			chunk2 = vm.readChunk(0, 2, 4, 0);
			chunk3 = vm.readChunk(2, 2, 4, 0);
			chunk0->createEdgeDesc2D(-1, 0, 0, base, 2);
			chunk1->createEdgeDesc2D(-1, 1, 0, base, 2);
			chunk2->createEdgeDesc2D(-1, 0, 1, base, 2);
			chunk3->createEdgeDesc2D(-1, 1, 1, base, 2);
		}
		// hinges
		{
			// paralelle to the z-axis
			chunk0 = vm.readChunk(4, 4, 0, 0);
			chunk1 = vm.readChunk(4, 4, 2, 0);
			chunk0->createEdgeDesc1D(-1, 0, base, 5);
			chunk1->createEdgeDesc1D(-1, 1, base, 5);
		}
		{
			// paralelle to the x-axis
			chunk0 = vm.readChunk(0, 4, 4, 0);
			chunk1 = vm.readChunk(2, 4, 4, 0);
			chunk0->createEdgeDesc1D(-1, 0, base, 3);
			chunk1->createEdgeDesc1D(-1, 1, base, 3);
		}
		{
			// paralelle to the y-axis
			chunk0 = vm.readChunk(4, 0, 4, 0);
			chunk1 = vm.readChunk(4, 2, 4, 0);
			chunk0->createEdgeDesc1D(-1, 0, base, 4);
			chunk1->createEdgeDesc1D(-1, 1, base, 4);
		}
		vm.generateIndices();
		long timeTaken = t.mark();
		printf_s("time elapsed: %d\n", timeTaken);
		vm.exportJson();
		//delete heightmapSampler;
	}
	
	if (false){
		SF_Box sampler;// = new SF_Box();
		sampler.setSpecs(vec3(0.5f, 0.5f, 0.5f), vec3(5.5f, 3.5f, 5.5f));

		VoxelManager vm;
		vm.initChunkSize(12, 12, 12);
		vm.dataSampler = &sampler;
		// also do the data sampling
		vm.initClipmap(2);
		vm.generateVertices();
		// create edge descs
		vm.createEdgeDescs();
		vm.generateIndices();
		vm.exportJson();
	}
	
	printf_s("done.");
	getchar();
}