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
}

void veMaterialManager::resourceRecovery(unsigned int contextID)
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