#include "HeightMapSampler.h"
#include "lodepng/lodepng.h"

void SF_Heightmap::setSpecs(const ivec3& _dim){
	dimension = _dim;
}
void SF_Heightmap::loadPNG(const ivec3& _dim, const char* fileName){
	setSpecs(_dim);
	std::vector<unsigned char> pngData; //the raw pixels
	unsigned int width = dimension.x;
	unsigned int height = dimension.z;

	lodepng::load_file(pngData, fileName);
	unsigned int error = lodepng::decode(image, width, height, pngData);
	assert(error == 0);
	imageBuffer = image.data();
}
unsigned char SF_Heightmap::materialFunc(const ivec3& pos, vec3* intersects, vec3* outNormal0, vec3* outNormal1, vec3* outNormal2){
	vec3 floatPos = pos;
	float sampleScale = 1.0f;
	floatPos *= sampleScale;
	unsigned char material = 0;

	float heightOrigin = getHeight(vec2(floatPos.x, floatPos.z));
	if (floatPos.y <= heightOrigin){
		material = 1;
	}
	if (floatPos.y <= heightOrigin && floatPos.y + sampleScale >= heightOrigin){
		// crossing on the y axis.
		intersects->y = (heightOrigin - floatPos.y) / sampleScale;
		// calculate normal
	}
	
	float heightXEnd = getHeight(vec2(floatPos.x + sampleScale, floatPos.z));
	if (floatPos.y > heightOrigin && floatPos.y < heightXEnd){
		// on an incline.
		intersects->x = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightXEnd - floatPos.y));
	}
	else if (floatPos.y < heightOrigin && floatPos.y > heightXEnd){
		// on a decline.
		intersects->x = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightXEnd - floatPos.y));
	}

	float heightZEnd = getHeight(vec2(floatPos.x, floatPos.z + sampleScale));
	if (floatPos.y > heightOrigin && floatPos.y < heightZEnd){
		// on an incline.
		intersects->z = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightZEnd - floatPos.y));
	}
	else if (floatPos.y < heightOrigin && floatPos.y > heightZEnd){
		// on a decline.
		intersects->z = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightZEnd - floatPos.y));
	}
	return material;
}
float SF_Heightmap::getHeight(vec2 pos){
	ivec2 floorPos = glm::floor(pos);
	vec2 remainderPos = pos - vec2(floorPos);
	
	float val0 = getRGBSum(floorPos);
	float val1 = getRGBSum(floorPos + ivec2(1, 0));
	float val2 = getRGBSum(floorPos + ivec2(0, 1));
	float val3 = getRGBSum(floorPos + ivec2(1, 1));

	// use a simple sampling method for four-point sampling.
	float bottom = val0 + remainderPos.x * (val1 - val0);
	float top = val2 + remainderPos.x * (val3 - val2);
	float height = bottom + remainderPos.y * (top - bottom);
	return height;

}
