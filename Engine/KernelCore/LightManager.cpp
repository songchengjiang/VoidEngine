#include "LightManager.h"
#include "Light.h"
#include "FileCore/File.h"
#include "Constants.h"
#include <rapidjson/include/document.h>

using namespace rapidjson;

veLightManager::veLightManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veLightManager::~veLightManager()
{

}

bool veLightManager::loadLightTemplates(const std::string &filePath)
{
	std::string buffer = veFile::readFileToBuffer(filePath);
	Document document;
	document.Parse(buffer.c_str());

	if (document.HasMember(LIGHT_TEMPLATES_KEY.c_str())) {
		_lightTemplate.clear();
		const Value &lightTemplates = document[LIGHT_TEMPLATES_KEY.c_str()];
		for (unsigned int i = 0; i < lightTemplates.Size(); ++i) {
			const Value & lt = lightTemplates[i];
			std::string name = lt[NAME_KEY.c_str()].GetString();
			veLightManager::LightTemplate lightTemplate;
			lightTemplate.limit = lt[LIMIT_KEY.c_str()].GetUint();
			const Value &paramsVals = lt[PARAMETERS_KEY.c_str()];
			if (!paramsVals.Empty()) {
				for (unsigned int i = 0; i < paramsVals.Size(); ++i) {
					const Value & pm = paramsVals[i];
					lightTemplate.parameters.push_back(std::make_pair(pm[NAME_KEY.c_str()].GetString(), pm[TYPE_KEY.c_str()].GetString()));
				}
				_lightTemplate[name] = lightTemplate;
			}
		}
		return true;
	}

	return false;
}

veLight* veLightManager::findLight(const std::string &name)
{
	for (auto &iter : _lightPool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veLight* veLightManager::createLight(const std::string &type, const std::string &name)
{
	auto iter = _lightTemplate.find(type);
	if (iter == _lightTemplate.end()) return nullptr;
	veParameterList params;
	for (auto &pm : iter->second.parameters) {
		auto param = new veParameter;
		param->setName(pm.first);
		params.push_back(param);
	}
	auto light = new veLight(type, params);
	light->setName(name);
	_lightPool.push_back(light);
	return light;
}
