#include <stdio.h>
#include "qef.h"
#include "Octree.h"
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
	}

	OctreeNode root(16, 0, 0, 0);
	SamplerFunction* sampler = new SF_Box();
	((SF_Box*)sampler)->setSpecs(vec3(0.5f,0.5f,0.5f), vec3(1.5f,1.5f,1.5f));
	root.performSDF(sampler);
	
	OctreeNode::generateMinimizers(&root, &root);
	
	getchar();
}