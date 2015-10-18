#include "Entity.h"
#include "EntityRenderer.h"
#include "Node.h"

veEntity::veEntity()
	: _needRefresh(true)
{
	_renderer = new veEntityRenderer;
}

veEntity::~veEntity()
{

}

void veEntity::update(veNode *node, veSceneManager *sm)
{
	if (!_isVisible) return;
	if (_animationContainer.valid()) {
		_animationContainer->update(node, this, sm);
	}
	if (_isDirtyBoundingBox) {
		dirtyBoundingBox();
		_isDirtyBoundingBox = false;
	}
	if (_renderer.valid())
		_renderer->visit(node, this, sm);
}

bool& veEntity::needRefresh()
{
	return _needRefresh;
}

int veEntity::addMesh(veMesh *mesh)
{
	if (!mesh) return -1;
	auto iter = std::find(_meshList.begin(), _meshList.end(), mesh);
	if (iter != _meshList.end()) return -1;
	_meshList.push_back(mesh);
	return int(_meshList.size() - 1);
}

bool veEntity::removeMesh(veMesh *mesh)
{
	auto iter = std::find(_meshList.begin(), _meshList.end(), mesh);
	if (iter == _meshList.end()) return false;
	_meshList.erase(iter);
	return true;
}

veMesh* veEntity::removeMesh(unsigned int mIndex)
{
	veAssert(mIndex < _meshList.size());
	veMesh* mesh = _meshList[mIndex].get();
	_meshList.erase(_meshList.begin() + mIndex);
	return mesh;
}

veMesh* veEntity::getMesh(unsigned int mIndex)
{
	veAssert(mIndex < _meshList.size());
	return _meshList[mIndex].get();
}

void veEntity::dirtyBoundingBox()
{
	_boundingBox.dirty();
	for (auto &iter : _meshList) {
		iter->updateBoundingBox();
		_boundingBox.expandBy(iter->getBoundingBox() * iter->getAttachedNode()->toMeshNodeRootMatrix());
	}
}
