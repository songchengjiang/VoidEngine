#include "EntityManager.h"

veEntityManager::veEntityManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veEntityManager::~veEntityManager()
{

}

veEntity* veEntityManager::findEntity(const std::string &name)
{
	for (auto &iter : _entityPool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veEntity* veEntityManager::createEntity(const std::string &name)
{
	veEntity *entity = new veEntity;
	entity->setName(name);
	_entityPool.push_back(entity);
	return entity;
}