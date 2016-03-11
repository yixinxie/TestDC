#include "MeshSerializer.h"

using namespace std;
void MeshSerializer::serialize(const char* fileName, const vector<glm::vec3>& vertices, const vector<unsigned int>& indices, const vector<glm::vec3>& normals){

	rapidjson::Document jsonDoc;
	jsonDoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();
	jsonDoc.AddMember("type", "dcmesh0", allocator);
	// vertex array
	rapidjson::Value vertexArray(rapidjson::kArrayType);
	for (auto iter = vertices.begin(); iter != vertices.end(); ++iter){

		rapidjson::Value objValue;
		objValue.SetObject();
		char temp[64];
		sprintf_s(temp, sizeof(temp), "%f,%f,%f", iter->x, iter->y, iter->z);

		rapidjson::Value strObj(rapidjson::kStringType);
		strObj.SetString(temp, allocator);
		objValue.AddMember("p", strObj, allocator);
		vertexArray.PushBack(objValue, allocator);
	}

	jsonDoc.AddMember("vertexArray", vertexArray, allocator);

	// index array
	{
		rapidjson::Value indexArray(rapidjson::kStringType);

		std::string str;
		for (auto iter = indices.begin(); iter != indices.end(); ++iter){
			char temp[12];
			if (iter != indices.begin())
				str.append(",");
			sprintf_s(temp, sizeof(temp), "%d", *iter);
			str.append(temp);
		}
		

		indexArray.SetString(str.c_str(), allocator);
		jsonDoc.AddMember("indexArray", indexArray, allocator);
	}
	// normal array
	rapidjson::Value normalArray(rapidjson::kArrayType);
	for (auto iter = normals.begin(); iter != normals.end(); ++iter){

		rapidjson::Value objValue;
		objValue.SetObject();
		char temp[64];
		sprintf_s(temp, sizeof(temp), "%f,%f,%f", iter->x, iter->y, iter->z);

		rapidjson::Value strObj(rapidjson::kStringType);
		strObj.SetString(temp, allocator);
		objValue.AddMember("n", strObj, allocator);
		normalArray.PushBack(objValue, allocator);
	}

	jsonDoc.AddMember("normalArray", normalArray, allocator);

	// end of data

	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	jsonDoc.Accept(writer);

	CharBuffer outBuffer;
	outBuffer.buffer = const_cast<char *>(strbuf.GetString());
	outBuffer.length = strbuf.GetSize();

	CharHelper::writeTextFile(fileName, outBuffer);

}
void MeshSerializer::serializeIndexFile(const char* indexPath, const vector<string> fileNames, const vector<glm::ivec4>& coords){
	rapidjson::Document jsonDoc;
	jsonDoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();
	jsonDoc.AddMember("type", "dcindex", allocator);
	// vertex array
	rapidjson::Value vertexArray(rapidjson::kArrayType);
	
	for (int i = 0; i < fileNames.size(); i++){

		rapidjson::Value objValue;
		objValue.SetObject();
		{
			rapidjson::Value strObj(rapidjson::kStringType);
			strObj.SetString(fileNames[i].c_str(), allocator);
			objValue.AddMember("file_name", strObj, allocator);
		}

		objValue.AddMember("coord_X", coords[i].x, allocator);
		objValue.AddMember("coord_Y", coords[i].y, allocator);
		objValue.AddMember("coord_Z", coords[i].z, allocator);
		objValue.AddMember("coord_W", coords[i].w, allocator);
		vertexArray.PushBack(objValue, allocator);
	}
	jsonDoc.AddMember("files", vertexArray, allocator);
	// end of data

	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	jsonDoc.Accept(writer);

	CharBuffer outBuffer;
	outBuffer.buffer = const_cast<char *>(strbuf.GetString());
	outBuffer.length = strbuf.GetSize();

	CharHelper::writeTextFile(indexPath, outBuffer);
}