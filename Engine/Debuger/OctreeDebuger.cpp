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

void veOctreeDebuger::update(veNode *node, veSceneManager *sm)
{
	veDebuger::update(node, sm);
	if (_isDrawOctree) {
		auto octreesm = static_cast<veOctreeSceneManager *>(sm);
		renderOctree(octreesm->getRootOctree());
	}
}

void veOctreeDebuger::renderOctree(veOctree *octree)
{
	if (!octree) return;
	renderBoundingBoxWireframe(octree->boundingBox);
	for (unsigned int i = 0; i < 8; ++i) {
		renderOctree(octree->children[i]);
	}
}
