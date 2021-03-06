#ifndef _VE_BASE_MANAGER_
#define _VE_BASE_MANAGER_
#include "Prerequisites.h"

class veSceneManager;
class VE_EXPORT veBaseManager
{
	friend class veSceneManager;
public:

	virtual ~veBaseManager();

	virtual void update() {}
	virtual void resourceRecovery(unsigned int contextID) = 0;
	veSceneManager* getSceneManager() { return _sceneManager; }

protected:
	veBaseManager(veSceneManager *sm);

protected:

	veSceneManager *_sceneManager;
};

#endif