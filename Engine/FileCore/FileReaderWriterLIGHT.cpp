#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Light.h"
#include "KernelCore/SceneManager.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterLIGHT : public veFileReaderWriter
{
public:
	veFileReaderWriterLIGHT()
		: _light(nullptr)
		, _doucument(nullptr)
	{};
	~veFileReaderWriterLIGHT(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		_sceneManager = sm;
		_name = name;
		_doucument = new Document;
		auto fileData = veFile::instance()->readFileToBuffer(filePath);
		_doucument->Parse(fileData->buffer);
		if (_doucument->HasParseError()) return nullptr;
		readLight();
		VE_SAFE_DELETE(_doucument);
		return _light;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readLight() {
		//std::string name = (*_doucument)[NAME_KEY.c_str()].GetString();
		std::string type = (*_doucument)[TYPE_KEY.c_str()].GetString();
		_light = _sceneManager->createLight(getLightType(type), _name);
		if (!_light) return;
		if (_doucument->HasMember(COLOR_KEY.c_str())) {
			const Value &val = (*_doucument)[COLOR_KEY.c_str()];
			veVec4 color = veVec4(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble(), val[3].GetDouble());
			_light->setColor(color);
		}

		if (_doucument->HasMember(INTENSITY_KEY.c_str())) {
			const Value &val = (*_doucument)[INTENSITY_KEY.c_str()];
			_light->setIntensity(val.GetDouble());
		}

		if (_doucument->HasMember(ATTENUATION_KEY.c_str())) {
			const Value &val = (*_doucument)[ATTENUATION_KEY.c_str()];
			_light->setAttenuationRange(val.GetDouble());
		}

		if (_doucument->HasMember(INNER_ANGLE_KEY.c_str())) {
			const Value &val = (*_doucument)[INNER_ANGLE_KEY.c_str()];
			_light->setInnerAngle(val.GetDouble());
		}

		if (_doucument->HasMember(OUTER_ANGLE_KEY.c_str())) {
			const Value &val = (*_doucument)[OUTER_ANGLE_KEY.c_str()];
			_light->setOuterAngle(val.GetDouble());
		}
	}

	veLight::LightType getLightType(const std::string &type) {
		if (type == DIRECTIONAL_KEY) {
			return veLight::DIRECTIONAL;
		}
		else if (type == POINT_KEY) {
			return veLight::POINT;
		}
		else if (type == SPOT_KEY) {
			return veLight::SPOT;
		}
		else
			return veLight::DIRECTIONAL;
	}

private:

	Document *_doucument;
	veLight *_light;
	veSceneManager *_sceneManager;
	std::string _name;
};

VE_READERWRITER_REG("velight", veFileReaderWriterLIGHT);