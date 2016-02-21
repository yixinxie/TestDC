#pragma once
#include <vector>

#include <glm/glm.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Orihsay/misc/CharHelper.h"
using namespace rapidjson;
using namespace std;
class MeshSerializer{
private:
public:
	static void serialize(const char* fileName, const vector<glm::vec3>& vertices, const vector<unsigned int>& indices, const vector<glm::vec3>& normals);
};