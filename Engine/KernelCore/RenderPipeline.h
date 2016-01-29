#ifndef _VE_RENDER_PIPELINE_
#define _VE_RENDER_PIPELINE_
#include <unordered_map>
#include <mutex>
#include "Prerequisites.h"
#include "FrameBufferObject.h"

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
	void draw(veCamera *camera);

	void pushVisibleNode(veCamera *camera, veNode *node);
	bool isNodeVisible(veNode *node);

protected:

	void fillRenderQueues();
	void sortRenderQueues();
	void visitRenderQueues();
	virtual void renderCamera(veCamera *camera, bool isMainCamera) = 0;

protected:

	std::mutex _visitMutex;
	veSceneManager *_sceneManager;
	std::unordered_map<veCamera*, std::vector<veNode *> > _visibleNodeList;
	VE_Ptr<veFrameBufferObject> _postProcesserFBO;
};

#endif