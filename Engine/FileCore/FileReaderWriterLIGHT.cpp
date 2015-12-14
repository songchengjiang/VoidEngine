#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Light.h"
#include "KernelCore/SceneManager.h"
#include "KernelCore/MaterialManager.h"
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
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		_sceneManager = sm;
		_name = name;
		_fileFolder = fullPath.substr(0, fullPath.find_last_of("/\\") + 1);
		_doucument = new Document;
		auto fileData = veFile::instance()->readFileToBuffer(fullPath);
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

		if (_doucument->HasMember(MATERIALS_KEY.c_str())) {
			std::string matFile = (*_doucument)[MATERIALS_KEY.c_str()].GetString();
			veMaterialArray *materials = nullptr;
			if (veFile::instance()->isFileExist(veFile::instance()->getFullFilePath(matFile))) {
				materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, matFile, _name + std::string("-Materials")));
			}
			else {
				materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, _fileFolder + matFile, _name + std::string("-Materials")));
			}
			_light->setMaterialArray(materials);
		}


		if (_doucument->HasMember(COLOR_KEY.c_str())) {
			const Value &val = (*_doucument)[COLOR_KEY.c_str()];
			veVec3 color = veVec3(val[0].GetDouble(), val[1].GetDouble(), val[2].GetDouble());
			_light->setColor(color);
		}

		if (_doucument->HasMember(INTENSITY_KEY.c_str())) {
			const Value &val = (*_doucument)[INTENSITY_KEY.c_str()];
			_light->setIntensity(val.GetDouble());
		}

		if (_light->getLightType() == veLight::POINT || _light->getLightType() == veLight::SPOT) {
			if (_doucument->HasMember(ATTENUATION_KEY.c_str())) {
				const Value &val = (*_doucument)[ATTENUATION_KEY.c_str()];
				if (_light->getLightType() == veLight::POINT) {
					static_cast<vePointLight *>(_light)->setAttenuationRange(val.GetDouble());
				}
				if (_light->getLightType() == veLight::SPOT) {
					static_cast<veSpotLight *>(_light)->setAttenuationRange(val.GetDouble());
				}
			}
		}

		if (_doucument->HasMember(SHADOW_ENABLED_KEY.c_str())) {
			const Value &val = (*_doucument)[SHADOW_ENABLED_KEY.c_str()];
			_light->shadowEnable(val.GetBool());
		}

		if (_doucument->HasMember(SHADOW_BIAS_KEY.c_str())) {
			const Value &val = (*_doucument)[SHADOW_BIAS_KEY.c_str()];
			_light->setShadowBias(val.GetDouble());
		}

		if (_doucument->HasMember(SHADOW_STRENGTH_KEY.c_str())) {
			const Value &val = (*_doucument)[SHADOW_STRENGTH_KEY.c_str()];
			_light->setShadowStrength(val.GetDouble());
		}

		if (_doucument->HasMember(SHADOW_RESOLUTION_KEY.c_str())) {
			const Value &val = (*_doucument)[SHADOW_RESOLUTION_KEY.c_str()];
			if (val.Size() == 2)
				_light->setShadowResolution(veVec2(val[0].GetDouble(), val[1].GetDouble()));
		}

		if (_doucument->HasMember(SHADOW_AREA_KEY.c_str())) {
			const Value &val = (*_doucument)[SHADOW_AREA_KEY.c_str()];
			if (val.Size() == 2)
				_light->setShadowArea(veVec2(val[0].GetDouble(), val[1].GetDouble()));
		}

		if (_doucument->HasMember(SHADOW_SOFT_KEY.c_str())) {
			const Value &val = (*_doucument)[SHADOW_SOFT_KEY.c_str()];
			_light->setUseSoftShadow(val.GetBool());
		}

		if (_doucument->HasMember(SHADOW_SOFTNESS_KEY.c_str())) {
			const Value &val = (*_doucument)[SHADOW_SOFTNESS_KEY.c_str()];
			_light->setShadowSoftness(val.GetDouble());
		}

		if (_light->getLightType() == veLight::SPOT) {
			if (_doucument->HasMember(INNER_ANGLE_KEY.c_str())) {
				const Value &val = (*_doucument)[INNER_ANGLE_KEY.c_str()];
				static_cast<veSpotLight *>(_light)->setInnerAngle(val.GetDouble());
			}

			if (_doucument->HasMember(OUTER_ANGLE_KEY.c_str())) {
				const Value &val = (*_doucument)[OUTER_ANGLE_KEY.c_str()];
				static_cast<veSpotLight *>(_light)->setOuterAngle(val.GetDouble());
			}
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
	std::string _fileFolder;
};

VE_READERWRITER_REG("velight", veFileReaderWriterLIGHT);