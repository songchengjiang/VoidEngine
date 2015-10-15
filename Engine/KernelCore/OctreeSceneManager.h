#ifndef _VE_OCTREE_SCENE_MANAGER_
#define _VE_OCTREE_SCENE_MANAGER_
#include "Prerequisites.h"
#include "SceneManager.h"
#include "BoudingBox.h"

class veOctree;
class VE_EXPORT veOctreeSceneManager : public veSceneManager
{
public:
	veOctreeSceneManager();
	veOctreeSceneManager(const veBoundingBox &bbox, unsigned int octreeDeep);
	~veOctreeSceneManager();

	void setOctreeMaxDeep(unsigned int maxDeep) { _octreeMaxDeep = maxDeep; }

	virtual veNode* createNode() override;
	virtual veCamera* createCamera(const veViewport &vp = { 0, 0, 0, 0 }) override;

protected:

	virtual void update() override;
	virtual void cull() override;
	virtual void render() override;

	void initOctrees();

protected:

	veBoundingBox _boundingBox;
	unsigned int _octreeMaxDeep;
	veOctree *_octree;
};

#endif