#ifndef _VE_ANIMATION_MANAGER_
#define _VE_ANIMATION_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Animation.h"

class VE_EXPORT veAnimationManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veAnimationManager();

	virtual void update() override;
	virtual void resourceRecovery() override;

	veAnimationPlayer* findAnimationPlayer(const std::string &name);
	veAnimationPlayer* createAnimationPlayer(const std::string &name, veAnimationContainer *container);
	veAnimationContainer* createAnimationContainer(const std::string &name);
	veAnimation* createAnimation(const std::string &name);

	static std::string TYPE() { return "ANIMATION"; }

private:

	veAnimationManager(veSceneManager *sm);

private:

	std::vector< VE_Ptr<veAnimationPlayer> > _animationPlayerPool;
};

#endif