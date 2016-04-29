#ifndef _VE_ENTITY_MANAGER_
#define _VE_ENTITY_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Entity.h"


class VE_EXPORT veEntityManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veEntityManager();

	virtual void resourceRecovery() override;

	veEntity* findEntity(const std::string &name);
	veEntity* createEntity(const std::string &name);

	static std::string TYPE() { return "ENTITY"; }

private:

	veEntityManager(veSceneManager *sm);
	std::vector< VE_Ptr<veEntity> > _entityPool;
};


#endif