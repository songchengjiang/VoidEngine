#ifndef _VE_OCTREE_SCENE_MANAGER_
#define _VE_OCTREE_SCENE_MANAGER_
#include "Prerequisites.h"
#include "SceneManager.h"
#include "BoudingBox.h"

class veOctree;
class veOctreeNode;
class VE_EXPORT veOctreeSceneManager : public veSceneManager
{
public:
	veOctreeSceneManager();
	veOctreeSceneManager(const veBoundingBox &bbox, unsigned int octreeDeep);
	~veOctreeSceneManager();

	void setOctreeMaxDeep(unsigned int maxDeep) { _octreeMaxDeep = maxDeep; }

	virtual veNode* createNode() override;
	virtual veCamera* createCamera(const veViewport &vp = { 0, 0, 0, 0 }) override;

	virtual void requestRender(veNode *node) override;
	virtual bool isNodeVisibleInScene(veNode *node) override;

	veOctree* getRootOctree() { return _octree; }

protected:

	typedef std::vector<veOctreeNode *> OctreeNodeList;

	virtual void update() override;
	virtual void render() override;

	void traverseOctree(veOctree *octant, veCamera *camera);
	void addOctreeNode(veOctreeNode *node, veOctree *octant, unsigned int depth = 0);
	void init();

protected:

	veBoundingBox _boundingBox;
	unsigned int _octreeMaxDeep;
	veOctree *_octree;
	OctreeNodeList _octreeNodeList;
};

#endif