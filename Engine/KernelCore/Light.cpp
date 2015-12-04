#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "Constants.h"

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec2 veLight::DEFAULT_SHADOW_AREA = veVec2(100.0f);
const float  veLight::DEFAULT_SHADOW_BIAS       = 0.0f;
const float  veLight::DEFAULT_SHADOW_STRENGTH   = 1.0f;

veLight* veLight::CURRENT_LIGHT = nullptr;

const std::string veLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME = "position";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME = "direction";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME = "color";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME = "intensity";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME = "attenuationRangeInverse";

const std::string veLight::DEFUALT_LIGHT_UNIFORM_LIGHT_MATRIX_NAME = "lightMat";

const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME = "shadowEnabled";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME = "shadowBias";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME = "shadowStrength";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_RESOLUTION_NAME = "shadowResolution";

const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NAME = "ve_directionalLight";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME = "ve_directionalLightNum";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_2D_NAME = "shadowMap2D";

const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_NAME = "ve_pointLight";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME = "ve_pointLightNum";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_CUBE_NAME = "shadowMapCube";

const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_NAME = "ve_spotLight";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME = "ve_spotLightNum";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME = "innerAngleCos";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME = "outerAngleCos";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_2D_NAME = "shadowMap2D";

static const veMat4 LIGHT_BIAS_MAT = veMat4(0.5f, 0.0f, 0.0f, 0.5f
										  , 0.0f, 0.5f, 0.0f, 0.5f
										  , 0.0f, 0.0f, 0.5f, 0.5f
										  , 0.0f, 0.0f, 0.0f, 1.0f);

veLight::veLight(LightType type)
	: _type(type)
	, _color(veVec4::WHITE)
	, _intensity(1.0f)
	, _attenuationRange(1000.0f)
	, _attenuationRangeInverse(1.0f / _attenuationRange)
	, _shadowArea(DEFAULT_SHADOW_AREA)
	, _lightInCamMatrix(veMat4::IDENTITY)
	, _shadowEnabled(false)
	, _shadowResolution(DEFAULT_SHADOW_RESOLUTION)
	, _shadowBias(DEFAULT_SHADOW_BIAS)
	, _shadowStrength(DEFAULT_SHADOW_STRENGTH)
	, _needRefreshShadow(true)
	, _needRefreshShadowCamera(true)
{
}

veLight::~veLight()
{

}

void veLight::update(veSceneManager *sm, const veMat4 &transform)
{
	veNode::update(sm, transform);
	if (_shadowEnabled) {
		updateShadowTexture();
		updateShadowCamera();
	}
}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

void veLight::setMask(unsigned int mask, bool isOverride /*= true*/)
{
	veNode::setMask(mask, isOverride);
	_needRefreshShadowCamera = true;
}

void veLight::setShadowResolution(const veVec2 &resolution)
{
	if (_shadowResolution == resolution)
		return;
	_shadowResolution = resolution;
	_needRefreshShadow = true;
}


void veLight::updateSceneManager()
{

}

veDirectionalLight::veDirectionalLight()
	: veLight(DIRECTIONAL)
{

}

veDirectionalLight::~veDirectionalLight()
{

}

veMat4 veDirectionalLight::getLightMatrix() const
{
	return LIGHT_BIAS_MAT * _shadowRenderingCam->projectionMatrix() * _shadowRenderingCam->viewMatrix();
}

void veDirectionalLight::shadowCameraCulling()
{
	_shadowRenderingCam->cull();
}

void veDirectionalLight::shadowCameraRendering()
{
	_shadowRenderingCam->fillRenderQueue();
	_shadowRenderingCam->render();
}

void veDirectionalLight::updateShadowTexture()
{
	if (_needRefreshShadow) {
		if (!_shadowTexture.valid())
			_shadowTexture = _sceneManager->createTexture(_name + std::string("-ShadowTexture"), veTexture::TEXTURE_2D);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT
			, GL_UNSIGNED_BYTE, nullptr, 1);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		_needRefreshShadow = false;
	}
}

void veDirectionalLight::updateShadowCamera()
{
	if (_needRefreshShadowCamera) {
		if (!_shadowRenderingCam.valid()) {
			_shadowRenderingCam = _sceneManager->createCamera(_name + std::string("-ShadowCamera"));
			auto fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(_name + std::string("-ShadowFBO"));
			_shadowRenderingCam->setFrameBufferObject(fbo);
		}

		_shadowRenderingCam->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
		_shadowRenderingCam->setProjectionMatrixAsOrtho(-_shadowArea.x() * 0.5f, _shadowArea.x() * 0.5f, -_shadowArea.y() * 0.5f, _shadowArea.y() * 0.5f
			, 0.1f, _attenuationRange);
		_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTexture.get());
		_shadowRenderingCam->setMask(_mask);

		_needRefreshShadowCamera = false;
	}

	_shadowRenderingCam->setMatrix(this->getNodeToWorldMatrix());
}

vePointLight::vePointLight()
	: veLight(POINT)
{

}

vePointLight::~vePointLight()
{

}

veMat4 vePointLight::getLightMatrix() const
{
	return this->getWorldToNodeMatrix();
}

void vePointLight::shadowCameraCulling()
{
	for (unsigned int i = 0; i < 6; ++i) {
		_shadowRenderingCam[i]->cull();
	}
}

void vePointLight::shadowCameraRendering()
{
	for (unsigned int i = 0; i < 6; ++i) {
		_shadowRenderingCam[i]->fillRenderQueue();
		_shadowRenderingCam[i]->render();
	}
}

void vePointLight::updateShadowTexture()
{
	if (_needRefreshShadow) {
		if (!_shadowTexture.valid())
			_shadowTexture = _sceneManager->createTexture(_name + std::string("-ShadowTexture"), veTexture::TEXTURE_CUBE);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT
			, GL_UNSIGNED_BYTE, nullptr, 1);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		_needRefreshShadow = false;
	}
}

void vePointLight::updateShadowCamera()
{
	if (_needRefreshShadowCamera) {
		char str[4];
		for (unsigned int i = 0; i < 6; ++i) {
			if (!_shadowRenderingCam[i].valid()) {
				sprintf(str, "%d", i);
				_shadowRenderingCam[i] = _sceneManager->createCamera(_name + std::string("-ShadowCamera") + std::string(str));
				auto fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(_name + std::string("-ShadowFBO") + std::string(str));
				_shadowRenderingCam[i]->setFrameBufferObject(fbo);
			}
			_shadowRenderingCam[i]->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
			_shadowRenderingCam[i]->setProjectionMatrixAsPerspective(90.0f, 1.0f, 0.1f, _attenuationRange);
			_shadowRenderingCam[i]->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _shadowTexture.get());
			_shadowRenderingCam[i]->setMask(_mask);
		}
		_needRefreshShadowCamera = false;
	}

	veMat4 ltow = this->getNodeToWorldMatrix();
	_shadowRenderingCam[0]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_X, veVec3::NEGATIVE_UNIT_Y));
	_shadowRenderingCam[1]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_X, veVec3::NEGATIVE_UNIT_Y));
	_shadowRenderingCam[2]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Y, veVec3::UNIT_Z));
	_shadowRenderingCam[3]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Y, veVec3::NEGATIVE_UNIT_Z));
	_shadowRenderingCam[4]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Z, veVec3::NEGATIVE_UNIT_Y));
	_shadowRenderingCam[5]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Z, veVec3::NEGATIVE_UNIT_Y));
}

veSpotLight::veSpotLight()
	: veLight(SPOT)
	, _innerAngle(0.0f)
	, _innerAngleCos(veMath::veCos(veMath::veRadian(_innerAngle)))
	, _outerAngle(45.0f)
	, _outerAngleCos(veMath::veCos(veMath::veRadian(_outerAngle)))
{

}

veSpotLight::~veSpotLight()
{

}

veMat4 veSpotLight::getLightMatrix() const
{
	return LIGHT_BIAS_MAT * _shadowRenderingCam->projectionMatrix() * _shadowRenderingCam->viewMatrix();
}

void veSpotLight::shadowCameraCulling()
{
	_shadowRenderingCam->cull();
}

void veSpotLight::shadowCameraRendering()
{
	_shadowRenderingCam->fillRenderQueue();
	_shadowRenderingCam->render();
}

void veSpotLight::updateShadowTexture()
{
	if (_needRefreshShadow) {
		if (!_shadowTexture.valid())
			_shadowTexture = _sceneManager->createTexture(_name + std::string("-ShadowTexture"), veTexture::TEXTURE_2D);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT
			, GL_UNSIGNED_BYTE, nullptr, 1);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		_needRefreshShadow = false;
	}
}

void veSpotLight::updateShadowCamera()
{
	if (_needRefreshShadowCamera) {
		if (!_shadowRenderingCam.valid()) {
			_shadowRenderingCam = _sceneManager->createCamera(_name + std::string("-ShadowCamera"));
			auto fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(_name + std::string("-ShadowFBO"));
			_shadowRenderingCam->setFrameBufferObject(fbo);
		}

		_shadowRenderingCam->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
		_shadowRenderingCam->setProjectionMatrixAsPerspective(2.0f * _outerAngle, 1.0f, 0.1f, _attenuationRange);
		_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTexture.get());
		_shadowRenderingCam->setMask(_mask);

		_needRefreshShadowCamera = false;
	}

	_shadowRenderingCam->setMatrix(this->getNodeToWorldMatrix());
}
