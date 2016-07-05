#ifndef _VE_RENDER_PIPELINE_
#define _VE_RENDER_PIPELINE_
#include <unordered_map>
#include <mutex>
#include "Prerequisites.h"
#include "FrameBufferObject.h"
#include "RenderQueue.h"
#include "Node.h"

class veSceneManager;
class veCamera;
class veViewer;
class VE_EXPORT veRenderPipeline
{
public:
	veRenderPipeline(veSceneManager *sm);
	virtual ~veRenderPipeline();

	USE_VE_PTR;

	void rendering(veViewer *viewer);
    void prepareForDraws(veCamera *camera);
    void draw(veCamera *camera, veRenderQueue::RenderGroup &rg, const std::function<bool(veRenderCommand &command)> &callback = nullptr);

	bool isNodeVisible(veNode *node);

protected:

    void visitRenderQueues(veCamera *camera);
	void cullRenderQueues(veCamera *camera);
	void fillRenderQueues(veCamera *camera);
	void sortRenderQueues(veCamera *camera);
	void renderShadows();
	void renderCameras(veViewer *viewer);
	virtual void renderScene(veCamera *camera) = 0;

	void renderDirectionalLightShadow(veLight *light);
	void renderPointLightShadow(veLight *light);
	void renderSpotLightShadow(veLight *light);

	vePass* getOrCreateDirectionalShadowPass(const std::string &vDef, const std::string &fDef);
	vePass* getOrCreateOmnidirectionalShadowPass(const std::string &vDef, const std::string &fDef);

protected:

	std::mutex _visitMutex;
	veSceneManager *_sceneManager;
	std::unordered_map<veCamera*, veNodeList > _visibleNodeList;
	VE_Ptr<veFrameBufferObject> _postProcesserFBO;
	VE_Ptr<veFrameBufferObject> _shadowingFBO;

	std::unordered_map< std::string, VE_Ptr<vePass> > _shadowPassList;
};

#endif