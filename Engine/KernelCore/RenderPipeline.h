#ifndef _VE_RENDER_PIPELINE_
#define _VE_RENDER_PIPELINE_
#include <unordered_map>
#include <mutex>
#include "Prerequisites.h"

class veSceneManager;
class veCamera;
class veNode;
class VE_EXPORT veRenderPipeline
{
public:
	veRenderPipeline(veSceneManager *sm);
	virtual ~veRenderPipeline();

	USE_VE_PTR;

	void rendering();
	void pushVisibleNode(veCamera *camera, veNode *node);
	bool isNodeVisible(veNode *node);

protected:

	void fillRenderQueues();
	void sortRenderQueues();
	void visitRenderQueues();
	void draw(veCamera *camera);
	virtual void renderCamera(veCamera *camera) = 0;

protected:

	std::mutex _visitMutex;
	veSceneManager *_sceneManager;
	std::unordered_map<veCamera*, std::vector<veNode *> > _visibleNodeList;
};

#endif