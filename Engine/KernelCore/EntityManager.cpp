#include "EntityManager.h"

veEntityManager::veEntityManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veEntityManager::~veEntityManager()
{

}

void veEntityManager::resourceRecovery()
{
	for (std::vector< VE_Ptr<veEntity> >::iterator iter = _entityPool.begin(); iter != _entityPool.end(); ) {
		if ((*iter)->refCount() <= 1) {
			iter = _entityPool.erase(iter);
		}else {
			++iter;
		}
	}
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
	veEntity *entity = new veEntity(_sceneManager);
	entity->setName(name);
	_entityPool.push_back(entity);
	return entity;
}