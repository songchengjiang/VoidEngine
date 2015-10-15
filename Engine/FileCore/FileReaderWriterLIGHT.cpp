#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Light.h"
#include "KernelCore/LightManager.h"
#include "KernelCore/SceneManager.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterLIGHT : public veFileReaderWriter
{
public:
	veFileReaderWriterLIGHT()
		: _light(nullptr)
	{};
	~veFileReaderWriterLIGHT(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath) override{
		_sceneManager = sm;
		_fileFolder = filePath.substr(0, filePath.find_last_of("/\\") + 1);
		std::string buffer = veFile::readFileToBuffer(filePath);
		_doucument.Parse(buffer.c_str());
        if (_doucument.HasParseError()) return  nullptr;
		readLight();
		return _light;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readLight() {
		std::string name = _doucument[NAME_KEY.c_str()].GetString();
		std::string type = _doucument[TYPE_KEY.c_str()].GetString();
		_light = _sceneManager->createLight(type);
		if (!_light) return;
		_light->setName(name);
		const Value &paramsVals = _doucument[PARAMETERS_KEY.c_str()];
		if (!paramsVals.Empty()) {
			for (unsigned int i = 0; i < paramsVals.Size(); ++i) {
				const Value & pm = paramsVals[i];
				const Value & values = pm[VALUES_KEY.c_str()];
				auto paramter = _light->getParameter(pm[NAME_KEY.c_str()].GetString());
				if (paramter) {
					setValues(paramter, values);
				}
			}
		}
	}

	void setValues(veParameter *paramter, const Value & values) {
		if (values.IsInt()) {
			paramter->set(values.GetInt());
		}
		else if (values.IsDouble()) {
			paramter->set((veReal)values.GetDouble());
		}
		else if (values.Size() == 2) {
			veVec2 vec2(values[0].GetDouble(), values[1].GetDouble());
			paramter->set(vec2);
		}
		else if (values.Size() == 3) {
			veVec3 vec3(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble());
			paramter->set(vec3);
		}
		else if (values.Size() == 4) {
			veVec4 vec4(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble(), values[3].GetDouble());
			paramter->set(vec4);
		}
	}

private:

	Document _doucument;
	std::string _fileFolder;
	std::string _className;
	veLight *_light;
	veSceneManager *_sceneManager;
};

VE_READERWRITER_REG("velight", veFileReaderWriterLIGHT);