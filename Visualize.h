#pragma once
#include <vector>
#include <glm/glm.hpp>
#define VIS_DOT(x, y, z) Visualize::inst()->add((VisualizeShape*)new VShape_Dot((x),(y),(z)))
#define VIS_VEC3(x, y, z, dx, dy, dz) Visualize::inst()->add((VisualizeShape*)new VShape_Vector3((x),(y),(z), (dx), (dy), (dz)))
#define VIS_FLUSH Visualize::inst()->flush()
using namespace glm;
class VisualizeShape{
private:
public:

	virtual void getType32(char* desc) = 0;
	virtual void getDesc128(char* desc) = 0;
};
class VShape_Dot : public VisualizeShape{
public:

	vec3 pos;
	VShape_Dot(float x, float y, float z):pos(vec3(x, y, z)){

	}
	void getType32(char* desc){
		strcpy_s(desc, 32, "dot");
	}
	void getDesc128(char* desc){
		sprintf_s(desc, 128, "%f,%f,%f", pos.x, pos.y, pos.z);
	}
};
class VShape_Vector3 : public VisualizeShape{
public:
	vec3 pos;
	vec3 dir;
	VShape_Vector3(float x, float y, float z, float dx, float dy, float dz) : pos(vec3(x, y, z)), dir(vec3(dx, dy, dz)){

	}
	void getType32(char* desc){
		strcpy_s(desc, 10, "vec3");
	}
	void getDesc128(char* desc){
		sprintf_s(desc, 128, "%f,%f,%f,%f,%f,%f", pos.x, pos.y, pos.z, dir.x, dir.y, dir.z);
	}
};
class Visualize{
private:
	// renderables:
	std::vector<VisualizeShape*> shapes;
	Visualize(){}
	static Visualize* _instance;
public:
	inline static Visualize* inst(){
		if (_instance == nullptr){
			_instance = new Visualize();
		}
		return _instance;
	}
	void add(VisualizeShape* shape);
	void flush(void);
	~Visualize();
};
