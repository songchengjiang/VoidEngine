#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"

static const veMat4 LIGHT_BIAS_MAT = veMat4(0.5f, 0.0f, 0.0f, 0.5f
	, 0.0f, 0.5f, 0.0f, 0.5f
	, 0.0f, 0.0f, 0.5f, 0.5f
	, 0.0f, 0.0f, 0.0f, 1.0f);

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec2 veLight::DEFAULT_SHADOW_AREA = veVec2(100.0f);
const float  veLight::DEFAULT_SHADOW_BIAS = 0.0f;
const float  veLight::DEFAULT_SHADOW_STRENGTH = 1.0f;

veLight* veLight::CURRENT_LIGHT = nullptr;

const std::string veLight::DEFUALT_LIGHT_TEXTURE_UNIFORM_NAME = "ve_lightTex";

veLight::veLight(LightType type)
	: _type(type)
	, _color(1.0f)
	, _intensity(1.0f)
	, _attenuationRange(1000.0f)
	, _attenuationRangeInverse(1.0f / _attenuationRange)
	, _shadowArea(DEFAULT_SHADOW_AREA)
	, _lightInCamMatrix(veMat4::IDENTITY)
	, _shadowEnabled(false)
	, _shadowResolution(DEFAULT_SHADOW_RESOLUTION)
	, _shadowBias(DEFAULT_SHADOW_BIAS)
	, _shadowStrength(DEFAULT_SHADOW_STRENGTH)
	, _shadowSoftness(0.1f)
	, _isUseSoftShadow(false)
	, _needUpdateShadowMap(true)
	, _lightMatrix(veMat4::IDENTITY)
{
}

veLight::~veLight()
{

}

void veLight::update(veSceneManager *sm, const veMat4 &transform)
{
	veNode::update(sm, transform);
	updateShadow();
}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

void veLight::refreshUpdate(veSceneManager *sm, const veMat4 &transform)
{
	_needUpdateShadowMap = true;
	veNode::refreshUpdate(sm, transform);
}

void veLight::setShadowResolution(const veVec2 &resolution)
{
	if (_shadowResolution == resolution)
		return;
	_shadowResolution = resolution;
	_needUpdateShadowMap = true;
}

void veLight::setShadowArea(const veVec2 &area)
{
	if (_shadowArea == area)
		return;
	_shadowArea = area;
	_needUpdateShadowMap = true;
}

void veLight::updateSceneManager()
{

}

veDirectionalLight::veDirectionalLight()
	: veLight(DIRECTIONAL)
{
}

void veDirectionalLight::updateShadow()
{
	if (_needUpdateShadowMap) {
		if (_shadowEnabled) {
			if (!_shadowTexture.valid()) {
				_shadowTexture = _sceneManager->createTexture(_name + std::string("-shadowTex"), veTexture::TEXTURE_2D);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			}

			if (!_shadowCamera.valid()) {
				_shadowCamera = _sceneManager->createCamera(_name + std::string("-shadowCamera"));
				_shadowCamera->setShadowCamera(true);
			}

			_shadowTexture->storage(int(_shadowResolution.x()), int(_shadowResolution.y()), 1
				, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);

			_shadowCamera->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
			_shadowCamera->setMask(_mask);
			auto halfShadowArea = _shadowArea * 0.5f;
			_shadowCamera->setProjectionMatrixAsOrtho(-halfShadowArea.x(), halfShadowArea.x(), -halfShadowArea.y(), halfShadowArea.y(), 0.1f, _attenuationRange);
		}
		if (_shadowCamera.valid())
			_shadowCamera->setVisible(_shadowEnabled);
		_needUpdateShadowMap = false;
	}

	if (_shadowEnabled) {
		_shadowCamera->setMatrix(this->getNodeToWorldMatrix());
		_lightMatrix = LIGHT_BIAS_MAT * _shadowCamera->projectionMatrix() * _shadowCamera->viewMatrix();
	}
}

veDirectionalLight::~veDirectionalLight()
{
}

vePointLight::vePointLight()
	: veLight(POINT)
{
}

void vePointLight::updateShadow()
{

	if (_needUpdateShadowMap) {
		if (_shadowEnabled) {
			if (!_shadowTexture.valid()) {
				_shadowTexture = _sceneManager->createTexture(_name + std::string("-shadowTex"), veTexture::TEXTURE_CUBE);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			}

			if (!_shadowCameras[0].valid()) {
				for (unsigned short i = 0; i < 6; ++i) {
					_shadowCameras[i] = _sceneManager->createCamera(_name + std::string("-shadowCamera"));
					_shadowCameras[i]->setShadowCamera(true);
				}
			}
			_shadowTexture->storage(int(_shadowResolution.x()), int(_shadowResolution.y()), 1
				, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
			for (unsigned short i = 0; i < 6; ++i) {
				_shadowCameras[i]->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
				_shadowCameras[i]->setMask(_mask);
				_shadowCameras[i]->setVisible(true);
				_shadowCameras[i]->setProjectionMatrixAsPerspective(90.0f, 1.0f, 0.1f, _attenuationRange);
			}
		}
		if (_shadowCameras[0].valid()) {
			for (unsigned short i = 0; i < 6; ++i) {
				_shadowCameras[i]->setVisible(_shadowEnabled);
			}
		}
		_needUpdateShadowMap = false;
	}

	if (_shadowEnabled) {
		veMat4 lightWorldMat = this->getNodeToWorldMatrix();
		veVec3 lightWorldPos = veVec3(lightWorldMat[0][3], lightWorldMat[1][3], lightWorldMat[2][3]);
		_shadowCameras[0]->setMatrix(veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::UNIT_X, veVec3::NEGATIVE_UNIT_Y));
		_shadowCameras[1]->setMatrix(veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::NEGATIVE_UNIT_X, veVec3::NEGATIVE_UNIT_Y));
		_shadowCameras[2]->setMatrix(veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::UNIT_Y, veVec3::UNIT_Z));
		_shadowCameras[3]->setMatrix(veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::NEGATIVE_UNIT_Y, veVec3::NEGATIVE_UNIT_Z));
		_shadowCameras[4]->setMatrix(veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::UNIT_Z, veVec3::NEGATIVE_UNIT_Y));
		_shadowCameras[5]->setMatrix(veMat4::lookAt(lightWorldPos, lightWorldPos + veVec3::NEGATIVE_UNIT_Z, veVec3::NEGATIVE_UNIT_Y));

		_lightMatrix = veMat4::translation(-lightWorldPos);
	}
}

vePointLight::~vePointLight()
{
}

veSpotLight::veSpotLight()
	: veLight(SPOT)
	, _innerAngle(0.0f)
	, _innerAngleCos(veMath::veCos(veMath::veRadian(_innerAngle)))
	, _outerAngle(45.0f)
	, _outerAngleCos(veMath::veCos(veMath::veRadian(_outerAngle)))
{
}

void veSpotLight::updateShadow()
{
	if (_needUpdateShadowMap) {
		if (_shadowEnabled) {
			if (!_shadowTexture.valid()) {
				_shadowTexture = _sceneManager->createTexture(_name + std::string("-shadowTex"), veTexture::TEXTURE_2D);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			}
			if (!_shadowCamera.valid()) {
				_shadowCamera = _sceneManager->createCamera(_name + std::string("-shadowCamera"));
				_shadowCamera->setShadowCamera(true);
			}

			_shadowTexture->storage(int(_shadowResolution.x()), int(_shadowResolution.y()), 1
				, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
			_shadowCamera->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
			_shadowCamera->setMask(_mask);
			_shadowCamera->setProjectionMatrixAsPerspective(2.0f * _outerAngle, 1.0f, 0.1f, _attenuationRange);
		}
		if (_shadowCamera.valid())
			_shadowCamera->setVisible(_shadowEnabled);
		_needUpdateShadowMap = false;
	}

	if (_shadowEnabled) {
		_shadowCamera->setMatrix(this->getNodeToWorldMatrix());
		_lightMatrix = LIGHT_BIAS_MAT * _shadowCamera->projectionMatrix() * _shadowCamera->viewMatrix();
	}
}

veSpotLight::~veSpotLight()
{
}