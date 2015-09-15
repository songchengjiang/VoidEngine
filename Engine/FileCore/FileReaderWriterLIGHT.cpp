#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Light.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterLIGHT : public veFileReaderWriter
{
public:
	veFileReaderWriterLIGHT()
	{};
	~veFileReaderWriterLIGHT(){};

	virtual void* readFile(const std::string &filePath){
		std::string buffer = veFile::instance()->readFileToBuffer(filePath);
		_doucument.Parse(buffer.c_str());

		return _light;
	}

	virtual bool writeFile(void *data, const std::string &filePath){
		return true;
	}

private:

	void readLight() {
		_light = new veLight;
	}

private:

	Document _doucument;
	veLight *_light;
};

VE_READERWRITER_REG("velight", veFileReaderWriterLIGHT);