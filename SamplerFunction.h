#pragma once
#include <glm/glm.hpp>
using namespace glm;
class SamplerFunction{
protected:
	ivec3 minBound, maxBound; // inclusive
public:
	SamplerFunction(void){}
	inline const ivec3& getMinBound(void){ return minBound; };
	inline const ivec3& getMaxBound(void){ return maxBound; };
	virtual unsigned char materialFunc(const ivec3& pos, const int cellSize, vec3* intersects, vec3* outNormal0, vec3* outNormal1, vec3* outNormal2) = 0;
};
class SF_Box : public SamplerFunction
{
public:
	vec3 rectMin, rectMax;
	SF_Box();
	
	void setSpecs(const vec3& min, const vec3& max);
	unsigned char materialFunc(const ivec3& pos, const int cellSize, vec3* intersects, vec3* outNormal0, vec3* outNormal1, vec3* outNormal2);
};

