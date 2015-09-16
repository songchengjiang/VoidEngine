#include "Light.h"
#include "NodeVisitor.h"
#include "FileCore/File.h"
#include <rapidjson/include/document.h>
#include "Constants.h"

using namespace rapidjson;
veLightManager::~veLightManager()
{

}

veLightManager* veLightManager::instance()
{
	static veLightManager lightDefinations;
	return &lightDefinations;
}

bool veLightManager::loadLightTemplates(const std::string &filePath)
{
	std::string buffer = veFile::instance()->readFileToBuffer(filePath);
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
				veLightManager::instance()->addLightTemplate(name, lightTemplate);
			}
		}
		return true;
	}

	return false;
}

void veLightManager::addLightTemplate(const std::string &className, const LightTemplate &lightTemp)
{
	auto iter = _lightTemplate.find(className);
	if (iter != _lightTemplate.end()) return;

	_lightTemplate[className] = lightTemp;
}

const veLightManager::LightTemplate& veLightManager::getLightTemplate(const std::string &className) const
{
	auto iter = _lightTemplate.find(className);
	return iter->second;
}

veLight* veLightManager::instanceOneLight(const std::string &className)
{
	auto iter = _lightTemplate.find(className);
	if (iter == _lightTemplate.end()) return nullptr;
	veParameterList params;
	for (auto &pm : iter->second.parameters) {
		auto param = new veParameter;
		param->setName(pm.first);
		params.push_back(param);
	}
	auto light = new veLight(className, params);
	return light;
}

veLightManager::veLightManager()
{

}


veLight::veLight(const std::string &type, const veParameterList &params)
	: _type(type)
	, _parameters(params)
{
}

veLight::~veLight()
{

}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

veParameter* veLight::getParameter(const std::string &name)
{
	for (auto &iter : _parameters) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}