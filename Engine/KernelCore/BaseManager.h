#ifndef _VE_BASE_MANAGER_
#define _VE_BASE_MANAGER_
#include "Prerequisites.h"

class veSceneManager;
class VE_EXPORT veBaseManager
{
	friend class veSceneManager;
public:

	virtual ~veBaseManager();

protected:
	veBaseManager(veSceneManager *sm);

protected:

	veSceneManager *_sceneManager;
};

#endif