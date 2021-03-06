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
class vePostProcesser;
class VE_EXPORT veRenderPipeline
{
public:
	veRenderPipeline(veSceneManager *sm);
	virtual ~veRenderPipeline();

	USE_VE_PTR;

	void rendering(veViewer *viewer);
    virtual void renderToPostProcesser(vePostProcesser *processer, veCamera *camera, unsigned int bufferMask, unsigned int contextID) {};

	bool isNodeVisible(veNode *node);

protected:

    void visitRenderQueues(veCamera *camera, unsigned int contextID);
	void cullRenderQueues(veCamera *camera);
	void fillRenderQueues(veCamera *camera, unsigned int contextID);
	void sortRenderQueues(veCamera *camera);
	//void renderShadows(veViewer *viewer);
	void renderCameras(veViewer *viewer);
	virtual void renderScene(veCamera *camera, unsigned int contextID) = 0;

	void renderDirectionalLightShadow(const veMat4 &lightInWorldMat, veLight *light, veCamera *camera, unsigned int contextID, veMat4 *lightShadowMats);
	void renderPointLightShadow(const veMat4 &lightInWorldMat, veLight *light, unsigned int contextID, veMat4 &lightShadowMat);
	void renderSpotLightShadow(const veMat4 &lightInWorldMat, veLight *light, unsigned int contextID, veMat4 &lightShadowMat);
    
    void caculateDirectionalLightCascadedParams(const veMat4 &lightInWorldMat, veLight *light, veCamera *camera, float *cascadedLevels);

	vePass* getOrCreateDirectionalShadowPass(const std::string &vDef, const std::string &fDef);
	vePass* getOrCreateOmnidirectionalShadowPass(const std::string &vDef, const std::string &fDef);
    
    void prepareForDraws(veCamera *camera);
    void draw(veCamera *camera, veRenderQueue::RenderGroup &rg, const std::function<bool(veRenderCommand &command)> &callback = nullptr);

protected:

	std::mutex _visitMutex;
	veSceneManager *_sceneManager;
	std::unordered_map<veCamera*, veNodeList > _visibleNodeList;
	//VE_Ptr<veFrameBufferObject> _postProcesserFBO;
	VE_Ptr<veFrameBufferObject> _shadowingFBO;

	std::unordered_map< std::string, VE_Ptr<vePass> > _shadowPassList;
};

#endif
