#include "HeightMapSampler.h"
#include "lodepng/lodepng.h"
// this function sets the region to sample from.
void SF_Heightmap::setSpecs(const ivec3& from, const ivec3& to){
	minBound = from;
	maxBound = to;
}
void SF_Heightmap::loadPNG(const ivec2& _dim, const char* fileName){
	dimension = _dim;
	std::vector<unsigned char> pngData; //the raw pixels
	unsigned int width = dimension.x;
	unsigned int height = dimension.y;

	lodepng::load_file(pngData, fileName);
	unsigned int error = lodepng::decode(image, width, height, pngData);
	assert(error == 0);
	imageBuffer = image.data();
}
unsigned char SF_Heightmap::materialFunc(const ivec3& pos, vec3* intersects, vec3* outNormal0, vec3* outNormal1, vec3* outNormal2){
	vec3 floatPos(pos);
	
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
		*outNormal1 = getNormal(vec2(floatPos.x, floatPos.z), heightOrigin);
	}
	
	float heightXEnd = getHeight(vec2(floatPos.x + sampleScale, floatPos.z));
	if (floatPos.y > heightOrigin && floatPos.y <= heightXEnd){
		// on an incline on the x axis.
		intersects->x = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightXEnd - floatPos.y));
		*outNormal0 = getNormal(vec2(floatPos.x, floatPos.z), heightOrigin);
	}
	else if (floatPos.y <= heightOrigin && floatPos.y > heightXEnd){
		// on a decline on the x axis.
		intersects->x = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightXEnd - floatPos.y));
		*outNormal0 = getNormal(vec2(floatPos.x, floatPos.z), heightOrigin);
	}

	float heightZEnd = getHeight(vec2(floatPos.x, floatPos.z + sampleScale));
	if (floatPos.y > heightOrigin && floatPos.y < heightZEnd){
		// on an incline on the z axis.
		intersects->z = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightZEnd - floatPos.y));
		*outNormal2 = getNormal(vec2(floatPos.x, floatPos.z), heightOrigin);
	}
	else if (floatPos.y < heightOrigin && floatPos.y > heightZEnd){
		// on a decline on the z axis.
		intersects->z = (floatPos.y - heightOrigin) * sampleScale / ((floatPos.y - heightOrigin) + (heightZEnd - floatPos.y));
		*outNormal2 = getNormal(vec2(floatPos.x, floatPos.z), heightOrigin);
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
vec3 SF_Heightmap::getNormal(vec2 pos, float height){
	/* use this simple method to calculate the normal.
	   sample the heights of the four adjacent points. if it's at the boundary,
	   arbitrarily set the normal to pointing upwards, for now.
	*/
	if (pos.x == 0 || pos.y == 0 || pos.x == dimension.x - 1 || pos.y == dimension.y - 1)
		return vec3(0, 1, 0);
	// otherwise, sample the four adjacent points.
	float heightNegX = getHeight(vec2(pos.x - sampleScale, pos.y));
	float heightPosX = getHeight(vec2(pos.x + sampleScale, pos.y));
	float heightNegY = getHeight(vec2(pos.x, pos.y - sampleScale));
	float heightPosY = getHeight(vec2(pos.x, pos.y + sampleScale));
	vec3 origin(pos.x, height, pos.y);
	vec3 xpos(pos.x + sampleScale, heightPosX, pos.y);
	vec3 ypos(pos.x, heightPosY, pos.y + sampleScale);
	vec3 seg0 = glm::normalize(xpos - origin);
	vec3 seg1 = glm::normalize(ypos - origin);
	vec3 ret(glm::normalize(glm::cross(seg0, seg1)));

	return ret;
}