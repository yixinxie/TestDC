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

	// dimension of the raw image.
	ivec2 dimension;
	const float sampleScale;

	
	float getHeight(vec2 pos);
	vec3 getNormal(vec2 pos, float height);
	inline float getRGBSum(ivec2 pos){
		unsigned char height0 = imageBuffer[(pos.x + pos.y * dimension.y) * BytesPerPixel];
		unsigned char height1 = imageBuffer[(pos.x + pos.y * dimension.y) * BytesPerPixel + 1];
		unsigned char height2 = imageBuffer[(pos.x + pos.y * dimension.y) * BytesPerPixel + 2];
		return (float)(height0 + height1 + height2) / 3.0f / 1.0f;
	}
public:
	SF_Heightmap(void) : sampleScale(1){}
	~SF_Heightmap(){}
	void setSpecs(const ivec3& from, const ivec3& to);
	void loadPNG(const ivec2& _dim, const char* fileName);
	unsigned char materialFunc(const ivec3& pos, vec3* intersects, vec3* outNormal0, vec3* outNormal1, vec3* outNormal2);
};