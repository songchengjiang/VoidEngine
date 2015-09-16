#include "Light.h"
#include "NodeVisitor.h"
#include <sstream>

veLightManager::~veLightManager()
{

}

veLightManager* veLightManager::instance()
{
	static veLightManager lightDefinations;
	return &lightDefinations;
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
	veLight::Parameters params;
	for (auto &pm : iter->second.parameters) {
		auto uniform = new veUniform(pm.first);
		params.push_back(uniform);
	}
	auto light = new veLight(className, params);
	return light;
}

veLightManager::veLightManager()
{

}


veLight::veLight(const std::string &type, const Parameters &params)
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

veUniform* veLight::getParameter(const std::string &name)
{
	for (auto &iter : _parameters) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

void veLight::apply(const veRenderCommand &command)
{
	getParameter("position")->setValue(veVec3(_matrix[0][3], _matrix[1][3], _matrix[2][3]));
	for (auto &iter : _parameters) {
		iter->apply(command);
	}
}