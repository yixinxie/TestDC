#include "Visualize.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "Orihsay/misc/CharHelper.h"
Visualize* Visualize::_instance = nullptr;
void Visualize::add(VisualizeShape* shape){
	shapes.push_back(shape);
}
Visualize::~Visualize(){
	for (int i = 0; i < shapes.size(); i++){
		if (shapes[i] != nullptr){
			delete shapes[i];
		}
	}
	shapes.clear();
}
void Visualize::flush(){
	rapidjson::Document jsonDoc;
	jsonDoc.SetObject();
	rapidjson::Document::AllocatorType& allocator = jsonDoc.GetAllocator();
	jsonDoc.AddMember("type", "visualizer0", allocator);
	rapidjson::Value vertexArray(rapidjson::kArrayType);
	for (int i = 0; i < shapes.size(); i++){
		VisualizeShape* iter = shapes[i];

		rapidjson::Value objValue;
		objValue.SetObject();

		char tempName[32];
		char tempDesc[128];
		iter->getType32(tempName);
		iter->getDesc128(tempDesc);

		rapidjson::Value typeObj(rapidjson::kStringType);
		typeObj.SetString(tempName, allocator);
		objValue.AddMember("type", typeObj, allocator);

		rapidjson::Value descObj(rapidjson::kStringType);
		descObj.SetString(tempDesc, allocator);
		objValue.AddMember("desc", descObj, allocator);

		vertexArray.PushBack(objValue, allocator);
	}

	jsonDoc.AddMember("shape_array", vertexArray, allocator);

	rapidjson::StringBuffer strbuf;
	rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	jsonDoc.Accept(writer);

	CharBuffer outBuffer;
	outBuffer.buffer = const_cast<char *>(strbuf.GetString());
	outBuffer.length = strbuf.GetSize();

	CharHelper::writeTextFile("visualize.json", outBuffer);
}
