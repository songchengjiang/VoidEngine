#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec3 veLight::DEFAULT_SHADOW_RANGE      = veVec3(512, 512, 1000);
const float  veLight::DEFAULT_SHADOW_BIAS       = 0.05f;
const float  veLight::DEFAULT_SHADOW_STRENGTH   = 1.0f;

veLight::veLight(const std::string &type, const veParameterList &params)
	: _type(type)
	, _parameters(params)
	, _lightMatrix(veMat4::IDENTITY)
	, _shadowEnabled(false)
	, _shadowResolution(DEFAULT_SHADOW_RESOLUTION)
	, _shadowBias(DEFAULT_SHADOW_BIAS)
	, _shadowStrength(DEFAULT_SHADOW_STRENGTH)
	, _shadowRange(DEFAULT_SHADOW_RANGE)
	, _needRefreshShadow(true)
{
}

veLight::~veLight()
{

}

void veLight::update(veSceneManager *sm, const veMat4 &transform)
{
	veNode::update(sm, transform);
	if (_shadowEnabled) {
		if (!_shadowRenderingCam.valid()) {
			_shadowRenderingCam = sm->createCamera(_name + std::string("-ShadowCamera"));
			auto fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(_name + std::string("-ShadowFBO"));
			_shadowRenderingCam->setFrameBufferObject(fbo);
			_shadowTexture = sm->createTexture(_name + std::string("-ShadowTexture"));
		}

		if (_needRefreshShadow) {
			_shadowRenderingCam->setProjectionMatrixAsOrtho(-_shadowRange.x() * 0.5f, _shadowRange.x() * 0.5f
															, -_shadowRange.y() * 0.5f, _shadowRange.y() * 0.5f
															, 1.0f, _shadowRange.z());
			_shadowTexture->storage(int(_shadowResolution.x()), int(_shadowResolution.y()), 1, GL_DEPTH_COMPONENT);
			_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, _shadowTexture.get());
			_needRefreshShadow = false;
		}

		_shadowRenderingCam->setMatrix(this->getNodeToWorldMatrix());
	}
}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

//void veLight::render(veCamera *camera)
//{
//	veNode::render(camera);
//	_camera = nullptr;
//	if (_mask & camera->getMask())
//		_camera = camera;
//}

veParameter* veLight::getParameter(const std::string &name)
{
	for (auto &iter : _parameters) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

void veLight::updateSceneManager()
{

}