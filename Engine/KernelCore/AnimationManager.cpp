#include "AnimationManager.h"
#include "SceneManager.h"

veAnimationManager::~veAnimationManager()
{

}

veAnimationManager::veAnimationManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

void veAnimationManager::update()
{
	for (auto &animPlayer : _animationPlayerPool) {
		animPlayer->update(_sceneManager);
	}
}

void veAnimationManager::resourceRecovery(unsigned int contextID)
{

}

veAnimationPlayer* veAnimationManager::findAnimationPlayer(const std::string &name)
{
	for (auto &iter : _animationPlayerPool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veAnimationContainer* veAnimationManager::createAnimationContainer(const std::string &name)
{
	auto animContainer = new veAnimationContainer;
	animContainer->setName(name);
	return animContainer;
}

veAnimationPlayer* veAnimationManager::createAnimationPlayer(const std::string &name, veAnimationContainer *container)
{
	auto animPlayer = new veAnimationPlayer(container);
	animPlayer->setName(name);
	animPlayer->_manager = this;
	_animationPlayerPool.push_back(animPlayer);
	return animPlayer;
}

veAnimation* veAnimationManager::createAnimation(const std::string &name)
{
	auto anim = new veAnimation;
	anim->setName(name);
	return anim;
}
