#include "MeshManager.h"

veMeshManager::veMeshManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veMeshManager::~veMeshManager()
{

}

void veMeshManager::resourceRecovery(unsigned int contextID)
{
	for (std::vector< VE_Ptr<veMesh> >::iterator iter = _meshPool.begin(); iter != _meshPool.end(); ) {
		if ((*iter)->refCount() <= 1) {
			iter = _meshPool.erase(iter);
		}else {
			++iter;
		}
	}
}

veMesh* veMeshManager::findMesh(const std::string &name)
{
	for (auto &iter : _meshPool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veMesh* veMeshManager::createMesh(const std::string &name)
{
	veMesh *mesh = new veMesh(_sceneManager);
	mesh->setName(name);
	_meshPool.push_back(mesh);
	return mesh;
}