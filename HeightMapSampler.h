#include "SamplerFunction.h"
#include <glm/glm.hpp>
#include <vector>
using namespace glm;
class SF_Heightmap : public SamplerFunction
{
private:
	const int BytesPerPixel = 4;
	unsigned char* imageBuffer;
	std::vector<unsigned char> image;
	ivec3 dimension;
	// sample from
	ivec2 minBound;
	ivec2 maxBound;
	// world space parameters
	
	vec2 worldSize; // world size to scale the image to.

	float getHeight(vec2 pos);
	inline int getRGBSum(ivec2 pos){
		unsigned char height0 = imageBuffer[(pos.x + pos.y * dimension.y) * BytesPerPixel];
		unsigned char height1 = imageBuffer[(pos.x + pos.y * dimension.y) * BytesPerPixel + 1];
		unsigned char height2 = imageBuffer[(pos.x + pos.y * dimension.y) * BytesPerPixel + 2];
		return height0 + height1 + height2;
	}
public:
	SF_Heightmap(void) {}
	~SF_Heightmap(){}
	void setSpecs(const ivec3& _dim);
	void loadPNG(const ivec3& _dim, const char* fileName);
	unsigned char materialFunc(const ivec3& pos, vec3* intersects, vec3* outNormal0, vec3* outNormal1, vec3* outNormal2);
};