#ifndef _VE_ENTITY_MANAGER_
#define _VE_ENTITY_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Entity.h"
#include <unordered_map>


class VE_EXPORT veEntityManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veEntityManager();

	veEntity* getEntity(const std::string &name);
	veEntity* createEntity(const std::string &name);

	static std::string TYPE() { return "ENTITY"; }

private:

	veEntityManager();
	std::unordered_map< std::string, VE_Ptr<veEntity> > _entityPool;
};


#endif