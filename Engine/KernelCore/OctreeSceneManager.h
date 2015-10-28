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

	virtual veNode* createNode(const std::string &name) override;
	virtual veCamera* createCamera(const std::string &name, const veViewport &vp = { 0, 0, 0, 0 }) override;

	virtual void requestRender(veNode *node) override;
	virtual void requestRayCast(veRay *ray) override;
	virtual bool isNodeVisibleInScene(veNode *node) override;

	veOctree* getRootOctree() { return _octree; }

protected:

	typedef std::vector<veOctreeNode *> OctreeNodeList;

	virtual void update() override;
	virtual void render() override;

	void addOctreeNode(veOctreeNode *node, veOctree *octant, unsigned int depth = 0);
	void intersectByRay(veOctree *octant, veRay *ray);
	void init();
	void waitSync();

protected:

	veBoundingBox _boundingBox;
	unsigned int _octreeMaxDeep;
	veOctree *_octree;
	OctreeNodeList _octreeNodeList;

	std::mutex              _octantUpdateMutex;
	unsigned int            _octantUpdateCount;
	std::condition_variable _octreesUpdateDoneCondition;

	std::mutex              _renderMutex;
};

#endif