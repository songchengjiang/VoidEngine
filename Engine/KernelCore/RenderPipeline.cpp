#include "RenderPipeline.h"
#include "SceneManager.h"
#include "RenderQueue.h"
#include "Camera.h"
#include "Constants.h"

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
	_shadowingFBO = veFrameBufferObjectManager::instance()->createFrameBufferObject("_VE_DEFERRED_RENDER_PIPELINE_SHADOWING_FBO_");
}

veRenderPipeline::~veRenderPipeline()
{

}

void veRenderPipeline::rendering()
{
	auto &cameraList = _sceneManager->getCameraList();
	for (auto &iter : cameraList) {
		cullRenderQueues(iter.get());
	}

	for (auto &iter : cameraList) {
		fillRenderQueues(iter.get());
		sortRenderQueues(iter.get());
	}

	renderShadows();
	renderCameras();
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

void veRenderPipeline::cullRenderQueues(veCamera *camera)
{
	if (camera->isVisible()) {
		_sceneManager->enqueueTaskToThread(nullptr, nullptr, [camera, this] {
			//veNodeList nodeList;
			std::unique_lock<std::mutex> lock(_visitMutex);
			_visibleNodeList[camera].clear();
			camera->cull(_visibleNodeList[camera]);
		});
	}
}

void veRenderPipeline::fillRenderQueues(veCamera *camera)
{
	std::unique_lock<std::mutex> lock(_visitMutex);
	auto &nodeList = _visibleNodeList[camera];
	if (!nodeList.empty()) {
		for (auto &node : nodeList) {
			for (unsigned int i = 0; i < node->getRenderableObjectCount(); ++i) {
				if (node->getRenderableObject(i)->isVisible())
					node->getRenderableObject(i)->render(node.get(), camera);
			}
		}
	}
}

void veRenderPipeline::sortRenderQueues(veCamera *camera)
{
	if (camera->isVisible() && !camera->getRenderQueue()->renderCommandList.empty()) {
		//_sceneManager->enqueueTaskToThread(nullptr, nullptr, [camera] {
		for (auto &renderPass : camera->getRenderQueue()->renderCommandList) {
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
		//});
	}
}

void veRenderPipeline::renderCameras()
{
	auto &cameraList = _sceneManager->getCameraList();
	for (auto &iter : cameraList) {
		veCamera *camera = iter.get();
		if (camera->isVisible() && !camera->isShadowCamera() && camera != _sceneManager->getCamera() && !camera->getViewport().isNull()) {
			veRenderState::instance()->resetState();
			renderScene(camera, false);
		}
	}
	if (_sceneManager->getCamera()) {
		veRenderState::instance()->resetState();
		if (_sceneManager->getCamera()->isVisible())
			renderScene(_sceneManager->getCamera(), true);
	}
}

void veRenderPipeline::draw(veCamera *camera, const std::function<bool(veRenderCommand &command)> &callback)
{
	auto &vp = camera->getViewport();
	auto &clearColor = camera->getClearColor();
	glViewport(vp.x * VE_DEVICE_PIXEL_RATIO, vp.y * VE_DEVICE_PIXEL_RATIO, vp.width * VE_DEVICE_PIXEL_RATIO, vp.height * VE_DEVICE_PIXEL_RATIO);
	glClear(camera->getClearMask());
	glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());
	if (!camera->getRenderQueue()->renderCommandList.empty()) {
		for (auto &renderPass : camera->getRenderQueue()->renderCommandList) {
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
		camera->getRenderQueue()->renderCommandList.clear();
	}
}

void veRenderPipeline::renderScene(veCamera *camera, bool isMainCamera)
{
	if (camera->getFrameBufferObject()) {
		camera->getFrameBufferObject()->bind(camera->getClearMask());
	}
	auto &clearColor = camera->getClearColor();
	glClear(camera->getClearMask());
	glClearColor(clearColor.r(), clearColor.g(), clearColor.b(), clearColor.a());
	draw(camera);
	if (camera->getFrameBufferObject()) {
		camera->getFrameBufferObject()->unBind();
	}
}

void veRenderPipeline::renderDirectionalLightShadow(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	auto dLight = static_cast<veDirectionalLight *>(light);
	veRenderState::instance()->resetState();
	_shadowingFBO->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dLight->getShadowTexture());
	_shadowingFBO->bind(GL_DEPTH_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	draw(dLight->getShadowCamera(), [this](veRenderCommand &command) -> bool {
		if (command.pass->depthWrite()) {
			auto pass = this->getOrCreateDirectionalShadowPass(command.pass->getShader(veShader::VERTEX_SHADER)->getShaderHeader(), command.pass->getShader(veShader::FRAGMENT_SHADER)->getShaderHeader());
			pass->cullFace() = command.pass->cullFace();
			pass->cullFaceMode() = command.pass->cullFaceMode();
			command.pass = pass;
			return true;
		}
		return false;
	});
	_shadowingFBO->unBind();
}

void veRenderPipeline::renderPointLightShadow(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	auto pLight = static_cast<vePointLight *>(light);
	for (unsigned short i = 0; i < 6; ++i) {
		veRenderState::instance()->resetState();
		_shadowingFBO->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pLight->getShadowTexture());
		_shadowingFBO->bind(GL_DEPTH_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		draw(pLight->getShadowCamera(i), [this](veRenderCommand &command) -> bool {
			if (command.pass->depthWrite()) {
				auto pass = this->getOrCreateOmnidirectionalShadowPass(command.pass->getShader(veShader::VERTEX_SHADER)->getShaderHeader(), command.pass->getShader(veShader::FRAGMENT_SHADER)->getShaderHeader());
				pass->cullFace() = command.pass->cullFace();
				pass->cullFaceMode() = command.pass->cullFaceMode();
				command.pass = pass;
				return true;
			}
			return false;
		});
	}
	_shadowingFBO->unBind();
}

void veRenderPipeline::renderSpotLightShadow(veLight *light)
{
	if (!light->isShadowEnabled()) return;
	auto sLight = static_cast<veSpotLight *>(light);
	veRenderState::instance()->resetState();
	_shadowingFBO->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sLight->getShadowTexture());
	_shadowingFBO->bind(GL_DEPTH_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	draw(sLight->getShadowCamera(), [this](veRenderCommand &command) {
		if (command.pass->depthWrite()) {
			auto pass = this->getOrCreateDirectionalShadowPass(command.pass->getShader(veShader::VERTEX_SHADER)->getShaderHeader(), command.pass->getShader(veShader::FRAGMENT_SHADER)->getShaderHeader());
			pass->cullFace() = command.pass->cullFace();
			pass->cullFaceMode() = command.pass->cullFaceMode();
			command.pass = pass;
			return true;
		}
		return false;
	});
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

void veRenderPipeline::renderShadows()
{
	auto &lightListMap = _sceneManager->getLightListMap();
	if (!lightListMap.empty()) {
		{
			auto iter = lightListMap.find(veLight::POINT);
			if (iter != lightListMap.end()) {
				auto &pointLightList = iter->second;
				for (auto &light : pointLightList) {
					renderPointLightShadow(light.get());
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::SPOT);
			if (iter != lightListMap.end()) {
				auto &spotLightList = iter->second;
				for (auto &light : spotLightList) {
					renderSpotLightShadow(light.get());
				}
			}
		}

		{
			auto iter = lightListMap.find(veLight::DIRECTIONAL);
			if (iter != lightListMap.end()) {
				auto &directionalLightList = iter->second;
				for (auto &light : directionalLightList) {
					renderDirectionalLightShadow(light.get());
				}
			}
		}
	}
}
