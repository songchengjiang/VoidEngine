#include "RenderPipeline.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Constants.h"
#include "Configuration.h"
#include "Viewer.h"

static veLoopQueue<veRenderCommand>::SortFunc PASS_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.pass <= right.pass : right.priority <= left.priority;
};

static veLoopQueue<veRenderCommand>::SortFunc ENTITY_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.pass <= right.pass : right.priority <= left.priority;
};

static veLoopQueue<veRenderCommand>::SortFunc TRANSPARENT_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.depthInCamera <= right.depthInCamera : right.priority <= left.priority;
};

static veLoopQueue<veRenderCommand>::SortFunc OVERLAY_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return left.priority <= right.priority;
};

veRenderPipeline::veRenderPipeline(veSceneManager *sm)
	: USE_VE_PTR_INIT
	, _sceneManager(sm)
{
	_shadowingFBO = _sceneManager->createFrameBufferObject("_VE_DEFERRED_RENDER_PIPELINE_SHADOWING_FBO_");
}

veRenderPipeline::~veRenderPipeline()
{

}

void veRenderPipeline::rendering(veViewer *viewer)
{
//    auto &cameraList = _sceneManager->getCameraList();
//    for (auto &iter : cameraList) {
//        cullRenderQueues(iter.get());
//    }
//    
//    for (auto &iter : cameraList) {
//        fillRenderQueues(iter.get());
//        sortRenderQueues(iter.get());
//    }
	renderShadows(viewer);
	renderCameras(viewer);
}

bool veRenderPipeline::isNodeVisible(veNode *node)
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	for (auto &iter : _visibleNodeList) {
		for (auto &n : iter.second) {
			if (node == n.get())
				return true;
		}
	}
	return false;
}

void veRenderPipeline::visitRenderQueues(veCamera *camera, unsigned int contextID)
{
    cullRenderQueues(camera);
    fillRenderQueues(camera, contextID);
    sortRenderQueues(camera);
}

void veRenderPipeline::cullRenderQueues(veCamera *camera)
{
	if (camera->isVisible()) {
		//_sceneManager->enqueueTaskToThread(nullptr, nullptr, [camera, this] {
			//veNodeList nodeList;
			std::unique_lock<std::mutex> lock(_visitMutex);
			_visibleNodeList[camera].clear();
			camera->cull(_visibleNodeList[camera]);
		//});
	}
}

void veRenderPipeline::fillRenderQueues(veCamera *camera, unsigned int contextID)
{
	//std::unique_lock<std::mutex> lock(_visitMutex);
	auto &nodeList = _visibleNodeList[camera];
	if (!nodeList.empty()) {
		for (auto &node : nodeList) {
			for (unsigned int i = 0; i < node->getRenderableObjectCount(); ++i) {
				if (node->getRenderableObject(i)->isVisible())
					node->getRenderableObject(i)->render(node.get(), camera, contextID);
			}
		}
	}
}

void veRenderPipeline::sortRenderQueues(veCamera *camera)
{
    static auto renderGroupSortFunc = [&](veRenderQueue::RenderGroup &rg){
        if (!rg.empty()){
            for (auto &renderPass : rg) {
                auto &renderList = renderPass.second;
                auto bgQueue = renderList.find(veRenderQueue::RENDER_QUEUE_BACKGROUND);
                if (bgQueue != renderList.end()) {
                    if (!bgQueue->second.empty()) {
                        bgQueue->second.quickSort(PASS_SORT);
                    }
                }
                
                auto entityQueue = renderList.find(veRenderQueue::RENDER_QUEUE_ENTITY);
                if (entityQueue != renderList.end()) {
                    if (!entityQueue->second.empty()) {
                        entityQueue->second.quickSort(ENTITY_SORT);
                    }
                }
                
                auto tpQueue = renderList.find(veRenderQueue::RENDER_QUEUE_TRANSPARENT);
                if (tpQueue != renderList.end()) {
                    if (!tpQueue->second.empty()) {
                        tpQueue->second.quickSort(TRANSPARENT_SORT);
                    }
                }
                
                auto olQueue = renderList.find(veRenderQueue::RENDER_QUEUE_OVERLAY);
                if (olQueue != renderList.end()) {
                    if (!olQueue->second.empty()) {
                        olQueue->second.quickSort(OVERLAY_SORT);
                    }
                }
            }
        }
    };
	if (camera->isVisible()) {
        renderGroupSortFunc(camera->getRenderQueue()->deferredRenderGroup);
        renderGroupSortFunc(camera->getRenderQueue()->forwardRenderGroup);
	}
}

void veRenderPipeline::renderCameras(veViewer *viewer)
{
	auto &cameraList = _sceneManager->getCameraList();
	for (auto &iter : cameraList) {
		veCamera *camera = iter.get();
		if (camera->isVisible() &&
            !camera->isShadowCamera() &&
            camera != viewer->getCamera() &&
            !camera->getViewport().isNull() &&
            camera->getFrameBufferObject()) {
			_sceneManager->getRenderState(viewer->getContextID())->resetState();
            visitRenderQueues(camera, viewer->getContextID());
			renderScene(camera, viewer->getContextID());
		}
	}
	if (viewer->getCamera()) {
		_sceneManager->getRenderState(viewer->getContextID())->resetState();
        if (viewer->getCamera()->isVisible()){
            visitRenderQueues(viewer->getCamera(), viewer->getContextID());
            auto vp = viewer->getCamera()->getViewport();
            viewer->getCamera()->setViewport({
                  vp.x * (int)veConfiguration::VE_DEVICE_PIXEL_RATIO
                , vp.y * (int)veConfiguration::VE_DEVICE_PIXEL_RATIO
                , vp.width * (int)veConfiguration::VE_DEVICE_PIXEL_RATIO
                , vp.height * (int)veConfiguration::VE_DEVICE_PIXEL_RATIO
            });
            renderScene(viewer->getCamera(), viewer->getContextID());
            viewer->getCamera()->setViewport(vp);
        }
	}
}

void veRenderPipeline::prepareForDraws(veCamera *camera)
{
   	auto &vp = camera->getViewport();
    auto &clearColor = camera->getClearColor();
    glViewport(vp.x, vp.y, vp.width, vp.height);
    glClear(camera->getClearMask());
    glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());
}

void veRenderPipeline::draw(veCamera *camera, veRenderQueue::RenderGroup &rg, const std::function<bool(veRenderCommand &command)> &callback)
{
	if (!rg.empty()) {
		for (auto &renderPass : rg) {
			auto &renderList = renderPass.second;
			for (auto &rq : renderList) {
				size_t sz = rq.second.size();
				for (size_t i = 0; i < sz; ++i) {
					auto &cmd = rq.second[i];
					bool needDraw = true;
					if (callback != nullptr)
						needDraw = callback(cmd);

					if (needDraw) 
						cmd.renderer->draw(cmd);
				}
			}
		}
		rg.clear();
	}
}

void veRenderPipeline::renderScene(veCamera *camera, unsigned int contextID)
{
	if (camera->getFrameBufferObject()) {
		camera->getFrameBufferObject()->bind(contextID, camera->getClearMask());
	}
    prepareForDraws(camera);
    draw(camera, camera->getRenderQueue()->deferredRenderGroup);
	draw(camera, camera->getRenderQueue()->forwardRenderGroup);
	if (camera->getFrameBufferObject()) {
		camera->getFrameBufferObject()->unBind();
	}
}

void veRenderPipeline::renderDirectionalLightShadow(veLight *light, veViewer *viewer)
{
	if (!light->isShadowEnabled()) return;
	auto dLight = static_cast<veDirectionalLight *>(light);
    static auto shadowPass = [this](veRenderCommand &command) -> bool {
		if (command.pass->castShadow()) {
			auto pass = this->getOrCreateDirectionalShadowPass(command.pass->getShader(veShader::VERTEX_SHADER)->getShaderHeader(), command.pass->getShader(veShader::FRAGMENT_SHADER)->getShaderHeader());
			pass->cullFace() = command.pass->cullFace();
			pass->cullFaceMode() = command.pass->cullFaceMode();
			command.pass = pass;
			return true;
		}
		return false;
    };
    
	_sceneManager->getRenderState(viewer->getContextID())->resetState();
	_shadowingFBO->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dLight->getShadowTexture());
	_shadowingFBO->bind(viewer->getContextID(), GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
    visitRenderQueues(dLight->getShadowCamera(), viewer->getContextID());
    prepareForDraws(dLight->getShadowCamera());
	draw(dLight->getShadowCamera(), dLight->getShadowCamera()->getRenderQueue()->deferredRenderGroup, shadowPass);
    draw(dLight->getShadowCamera(), dLight->getShadowCamera()->getRenderQueue()->forwardRenderGroup, shadowPass);
	_shadowingFBO->unBind();
}

void veRenderPipeline::renderPointLightShadow(veLight *light, veViewer *viewer)
{
	if (!light->isShadowEnabled()) return;
	auto pLight = static_cast<vePointLight *>(light);
    static auto shadowPass = [this](veRenderCommand &command) -> bool {
        if (command.pass->castShadow()) {
            auto pass = this->getOrCreateOmnidirectionalShadowPass(command.pass->getShader(veShader::VERTEX_SHADER)->getShaderHeader(), command.pass->getShader(veShader::FRAGMENT_SHADER)->getShaderHeader());
            pass->cullFace() = command.pass->cullFace();
            pass->cullFaceMode() = command.pass->cullFaceMode();
            command.pass = pass;
            return true;
        }
        return false;
    };
    
	for (unsigned short i = 0; i < 6; ++i) {
		_sceneManager->getRenderState(viewer->getContextID())->resetState();
		_shadowingFBO->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pLight->getShadowTexture());
		_shadowingFBO->bind(viewer->getContextID(), GL_DEPTH_BUFFER_BIT);
		//glClear(GL_DEPTH_BUFFER_BIT);
        visitRenderQueues(pLight->getShadowCamera(i), viewer->getContextID());
        prepareForDraws(pLight->getShadowCamera(i));
		draw(pLight->getShadowCamera(i), pLight->getShadowCamera(i)->getRenderQueue()->deferredRenderGroup, shadowPass);
        draw(pLight->getShadowCamera(i), pLight->getShadowCamera(i)->getRenderQueue()->forwardRenderGroup, shadowPass);
        _shadowingFBO->unBind();
	}
}

void veRenderPipeline::renderSpotLightShadow(veLight *light, veViewer *viewer)
{
	if (!light->isShadowEnabled()) return;
	auto sLight = static_cast<veSpotLight *>(light);
    static auto shadowPass = [this](veRenderCommand &command) {
		if (command.pass->castShadow()) {
			auto pass = this->getOrCreateDirectionalShadowPass(command.pass->getShader(veShader::VERTEX_SHADER)->getShaderHeader(), command.pass->getShader(veShader::FRAGMENT_SHADER)->getShaderHeader());
			pass->cullFace() = command.pass->cullFace();
			pass->cullFaceMode() = command.pass->cullFaceMode();
			command.pass = pass;
			return true;
		}
		return false;
    };
    
	_sceneManager->getRenderState(viewer->getContextID())->resetState();
	_shadowingFBO->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sLight->getShadowTexture());
	_shadowingFBO->bind(viewer->getContextID(), GL_DEPTH_BUFFER_BIT);
	//glClear(GL_DEPTH_BUFFER_BIT);
    visitRenderQueues(sLight->getShadowCamera(), viewer->getContextID());
    prepareForDraws(sLight->getShadowCamera());
	draw(sLight->getShadowCamera(), sLight->getShadowCamera()->getRenderQueue()->deferredRenderGroup, shadowPass);
    draw(sLight->getShadowCamera(), sLight->getShadowCamera()->getRenderQueue()->forwardRenderGroup, shadowPass);
	_shadowingFBO->unBind();
}

vePass* veRenderPipeline::getOrCreateDirectionalShadowPass(const std::string &vDef, const std::string &fDef)
{
	std::string id = vDef + fDef + std::string("|D");
	auto iter = _shadowPassList.find(id);
	if (iter != _shadowPassList.end()) {
		return iter->second.get();
	}

	auto pass = new vePass;
	auto vShader = new veShader(veShader::VERTEX_SHADER, std::string("system/DepthWriter.vert"));
	vShader->setShaderHeader(vDef);
	pass->setShader(vShader);
	auto fShader = new veShader(veShader::FRAGMENT_SHADER, std::string("system/DepthWriter.frag"));
	fShader->setShaderHeader(fDef);
	pass->setShader(fShader);
	pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	pass->addUniform(new veUniform("u_BoneMates", BONE_MATRIXES));
	_shadowPassList[id] = pass;
	return pass;
}

vePass* veRenderPipeline::getOrCreateOmnidirectionalShadowPass(const std::string &vDef, const std::string &fDef)
{
	std::string id = vDef + fDef + std::string("|OD");
	auto iter = _shadowPassList.find(id);
	if (iter != _shadowPassList.end()) {
		return iter->second.get();
	}

	auto pass = new vePass;
	auto vShader = new veShader(veShader::VERTEX_SHADER, std::string("system/SquareDepthWriter.vert"));
	vShader->setShaderHeader(vDef);
	pass->setShader(vShader);
	auto fShader = new veShader(veShader::FRAGMENT_SHADER, std::string("system/SquareDepthWriter.frag"));
	fShader->setShaderHeader(fDef);
	pass->setShader(fShader);
	pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	pass->addUniform(new veUniform("u_ModelViewMat", MV_MATRIX));
	pass->addUniform(new veUniform("u_BoneMates", BONE_MATRIXES));
	_shadowPassList[id] = pass;
	return pass;
}

void veRenderPipeline::renderShadows(veViewer *viewer)
{
	auto &lightListMap = _sceneManager->getLightListMap();
	if (!lightListMap.empty()) {
		{
			auto iter = lightListMap.find(veLight::POINT);
			if (iter != lightListMap.end()) {
				auto &pointLightList = iter->second;
				for (auto &light : pointLightList) {
                    if (light->isEnabled())
                        renderPointLightShadow(light.get(), viewer);
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::SPOT);
			if (iter != lightListMap.end()) {
				auto &spotLightList = iter->second;
				for (auto &light : spotLightList) {
                    if (light->isEnabled())
                        renderSpotLightShadow(light.get(), viewer);
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::DIRECTIONAL);
			if (iter != lightListMap.end()) {
				auto &directionalLightList = iter->second;
				for (auto &light : directionalLightList) {
                    if (light->isEnabled())
                        renderDirectionalLightShadow(light.get(), viewer);
				}
			}
		}
	}
}
