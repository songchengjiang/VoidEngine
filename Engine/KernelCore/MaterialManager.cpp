#include "MaterialManager.h"
#include "SceneManager.h"

veMaterialManager::veMaterialManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veMaterialManager::~veMaterialManager()
{

}

void veMaterialManager::update()
{
	if (_sceneManager->isNeedReload()) {
		for (auto &mats : _materialPool) {
			for (size_t mat = 0; mat < mats->getMaterialNum(); ++mat) {
				auto material = mats->getMaterial(mat);
				for (size_t tec = 0; tec < material->getTechniqueNum(); ++tec) {
					auto technique = material->getTechnique(tec);
					for (size_t ps = 0; ps < technique->getPassNum(); ++ps) {
						technique->getPass(ps)->needLink();
					}
				}
			}
		}
	}
}

void veMaterialManager::resourceRecovery()
{
	for (std::vector< VE_Ptr<veMaterialArray> >::iterator iter = _materialPool.begin(); iter != _materialPool.end(); ) {
		if ((*iter)->refCount() <= 1) {
			iter = _materialPool.erase(iter);
		}else {
			++iter;
		}
	}
}

veMaterialArray* veMaterialManager::findMaterialArray(const std::string &name)
{
	for (auto &iter : _materialPool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veMaterialArray* veMaterialManager::createMaterialArray(const std::string &name)
{
	veMaterialArray *matArray = new veMaterialArray;
	matArray->setName(name);
	_materialPool.push_back(matArray);
	return matArray;
}