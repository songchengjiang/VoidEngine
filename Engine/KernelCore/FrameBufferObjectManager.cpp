#include "FrameBufferObjectManager.h"

veFrameBufferObjectManager::veFrameBufferObjectManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veFrameBufferObjectManager::~veFrameBufferObjectManager()
{

}

void veFrameBufferObjectManager::resourceRecovery()
{
	for (std::vector< VE_Ptr<veFrameBufferObject> >::iterator iter = _fboPool.begin(); iter != _fboPool.end(); ) {
		if ((*iter)->refCount() <= 1) {
			iter = _fboPool.erase(iter);
		}else {
			++iter;
		}
	}
}

veFrameBufferObject* veFrameBufferObjectManager::findFrameBufferObject(const std::string &name)
{
	for (auto &iter : _fboPool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veFrameBufferObject* veFrameBufferObjectManager::createFrameBufferObject(const std::string &name)
{
	auto fbo = new veFrameBufferObject;
	fbo->setName(name);
	_fboPool.push_back(fbo);
	return fbo;
}