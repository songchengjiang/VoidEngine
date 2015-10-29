#include "OctreeDebuger.h"
#include "KernelCore/Octree.h"
#include "KernelCore/OctreeSceneManager.h"

veOctreeDebuger::veOctreeDebuger()
	: _isDrawOctree(false)
{

}

veOctreeDebuger::~veOctreeDebuger()
{

}

void veOctreeDebuger::render(veNode *node, veCamera *camera)
{
	if (_isDrawOctree) {
		auto octreesm = static_cast<veOctreeSceneManager *>(camera->getSceneManager());
		//_boundingBox = octreesm->getRootOctree()->boundingBox;
		renderOctree(octreesm->getRootOctree());
	}
	veDebuger::render(node, camera);
}

void veOctreeDebuger::renderOctree(veOctree *octree)
{
	if (!octree) return;
	renderBoundingBoxWireframe(octree->boundingBox, _drawDrawOctreeColor);
	for (unsigned int i = 0; i < 8; ++i) {
		renderOctree(octree->children[i]);
	}
}
