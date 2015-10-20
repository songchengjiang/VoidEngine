#include "Entity.h"
#include "EntityRenderer.h"
#include "Node.h"
#include "Ray.h"

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

bool veEntity::intersectWith(veRay *ray, veNode *node)
{
	bool state = false;
	if (!_meshList.empty()) {
		veMat4 wton = node->getWorldToNodeMatrix();
		veVec3 start = ray->getStart();
		veVec3 end = ray->getEnd();
		veVec3 localStart = wton * start;
		veVec3 localEnd = wton * end;
		for (auto &iter : _meshList) {
			veMat4 meshToRoot = iter->getAttachedNode()->toMeshNodeRootMatrix();
			meshToRoot.inverse();
			ray->setStart(meshToRoot * localStart);
			ray->setEnd(meshToRoot * localEnd);
			if (ray->isIntersectWith(iter->getBoundingBox())) {
				veRay::Intersection inters;
				if (iter->intersectWith(ray, inters.position, inters.normal)) {
					inters.node = node;
					inters.renderable = this;
					ray->addIntersection(inters);
					state = true;
				}
			}
		}

		ray->setStart(start);
		ray->setEnd(end);
	}

	return state;
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
		veMat4 meshToRoot = iter->getAttachedNode()->toMeshNodeRootMatrix();
		iter->updateBoundingBox(meshToRoot);
		_boundingBox.expandBy(iter->getBoundingBox() * meshToRoot);
	}
}
