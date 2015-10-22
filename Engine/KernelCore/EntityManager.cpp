#include "EntityManager.h"

veEntityManager::veEntityManager()
{

}

veEntityManager::~veEntityManager()
{

}

veEntity* veEntityManager::getEntity(const std::string &name)
{
	auto iter = _entityPool.find(name);
	if (iter == _entityPool.end()) return nullptr;

	return iter->second.get();
}

veEntity* veEntityManager::createEntity(const std::string &name)
{
	auto iter = _entityPool.find(name);
	if (iter != _entityPool.end()) veAssert("Entity had existed!");
	veEntity *entity = new veEntity;
	entity->setName(name);
	_entityPool[name] = entity;
	return entity;
}