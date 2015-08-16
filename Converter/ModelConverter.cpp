#include "ModelConverter.h"
#include <assimp/include/cimport.h>
#include <assimp/include/scene.h>
#include <assimp/include/postprocess.h>
#include <rapidjson/include/writer.h>
#include <rapidjson/include/stringbuffer.h>
#include <rapidjson/include/prettywriter.h>
#include <iostream>

ModelConverter::ModelConverter()
{
	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	aiAttachLogStream(&stream);
}

ModelConverter::~ModelConverter()
{

}

void ModelConverter::convert(const std::string &filePath, const std::string &outFile)
{
	//auto scene = aiImportFile(filePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	//if (!scene) return;

	rapidjson::StringBuffer s;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

	writer.StartObject();
	writer.String("hello");
	writer.String("world");
	writer.String("t");
	writer.Bool(true);
	writer.String("f");
	writer.Bool(false);
	writer.String("n");
	writer.Null();
	writer.String("i");
	writer.Uint(123);
	writer.String("pi");
	writer.Double(3.1416);
	writer.String("a");
	writer.StartArray();
	for (unsigned i = 0; i < 4; i++)
		writer.Uint(i);
	writer.EndArray();
	writer.EndObject();

	std::cout << s.GetString() << std::endl;
}

