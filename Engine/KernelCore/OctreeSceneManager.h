#ifndef _VE_OCTREE_SCENE_MANAGER_
#define _VE_OCTREE_SCENE_MANAGER_
#include "Prerequisites.h"
#include "SceneManager.h"

class VE_EXPORT veOctreeSceneManager : public veSceneManager
{
public:
	veOctreeSceneManager();
	~veOctreeSceneManager();

	virtual veNode* createNode() override;
	virtual veCamera* createCamera(const veViewport &vp = { 0, 0, 0, 0 }) override;

protected:

	virtual void update() override;
	virtual void cull() override;
	virtual void render() override;
};

#endif