#include "LightManager.h"
#include "Light.h"

veLightManager::veLightManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veLightManager::~veLightManager()
{

}

veLight* veLightManager::findLight(const std::string &name)
{
	for (auto &iter : _lightPool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veLight* veLightManager::createLight(veLight::LightType type, const std::string &name)
{
	auto light = new veLight(type);
	light->setName(name);
	_lightPool.push_back(light);
	return light;
}
