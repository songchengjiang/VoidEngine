#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "Configuration.h"

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec2 veLight::DEFAULT_SHADOW_AREA = veVec2(100.0f);
const float  veLight::DEFAULT_SHADOW_BIAS = 0.0f;
const float  veLight::DEFAULT_SHADOW_STRENGTH = 1.0f;

veLight* veLight::CURRENT_LIGHT = nullptr;

const std::string veLight::DEFUALT_LIGHT_TEXTURE_UNIFORM_NAME = "ve_lightTex";

veLight::veLight(LightType type)
	: _type(type)
    , _isEnabled(true)
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
{
}

veLight::~veLight()
{

}

void veLight::beforeUpdate(veSceneManager *sm)
{
    _sceneManager = sm;
    if (_isEnabled)
        updateShadow();
}

void veLight::shadowEnable(bool isEnabled)
{
    if (_shadowEnabled == isEnabled)
        return;
    _shadowEnabled = isEnabled;
    _needUpdateShadowMap = true;
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

veDirectionalLight::veDirectionalLight()
	: veLight(DIRECTIONAL)
    , _shadowCascadedCount(4)
{
    _shadowCascadedLevelScales[0] = 0.05f;
    _shadowCascadedLevelScales[1] = 0.1f;
    _shadowCascadedLevelScales[2] = 0.4f;
    _shadowCascadedLevelScales[3] = 1.0f;
}

void veDirectionalLight::updateShadow()
{
	if (_needUpdateShadowMap) {
		if (_shadowEnabled) {
			if (!_shadowTexture.valid()) {
				_shadowTexture = _sceneManager->createTexture(_name + std::string("-shadowTex"), veTexture::TEXTURE_2D_ARRAY);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
				_shadowTexture->setFilterMode(veTexture::NEAREST);
			}

            if (!_shadowCameras[0].valid()) {
                for (unsigned short i = 0; i < 4; ++i) {
                    _shadowCameras[i] = _sceneManager->createCamera(_name + std::string("-shadowCamera"));
                    _shadowCameras[i]->setShadowCamera(true);
                }
            }

			_shadowTexture->storage(int(_shadowResolution.x()), int(_shadowResolution.y()), _shadowCascadedCount
				, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);

            for (unsigned short i = 0; i < 4; ++i) {
                _shadowCameras[i]->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
                _shadowCameras[i]->setMask(_attachedNodeList[0]->getMask());
                _shadowCameras[i]->setVisible(true);
                auto halfShadowArea = _shadowArea * 0.5f;
                _shadowCameras[i]->setProjectionMatrixAsOrtho(-halfShadowArea.x(), halfShadowArea.x(), -halfShadowArea.y(), halfShadowArea.y(), 0.1f, _attenuationRange);
            }
		}
        if (_shadowCameras[0].valid()) {
            for (unsigned short i = 0; i < 4; ++i) {
                _shadowCameras[i]->setVisible(_shadowEnabled);
            }
        }
		_needUpdateShadowMap = false;
	}

}

void veDirectionalLight::setEnabled(bool isEnabled)
{
    veLight::setEnabled(isEnabled);
    if (_shadowEnabled && _shadowCameras[0].valid()) {
        for (unsigned short i = 0; i < 4; ++i) {
            _shadowCameras[i]->setVisible(isEnabled);
        }
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
				_shadowTexture->setFilterMode(veTexture::NEAREST);
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
				_shadowCameras[i]->setMask(_attachedNodeList[0]->getMask());
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
}

void vePointLight::setEnabled(bool isEnabled)
{
    veLight::setEnabled(isEnabled);
    if (_shadowEnabled && _shadowCameras[0].valid()) {
        for (unsigned short i = 0; i < 6; ++i) {
            _shadowCameras[i]->setVisible(isEnabled);
        }
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
				_shadowTexture->setFilterMode(veTexture::NEAREST);
			}
			if (!_shadowCamera.valid()) {
				_shadowCamera = _sceneManager->createCamera(_name + std::string("-shadowCamera"));
				_shadowCamera->setShadowCamera(true);
			}

			_shadowTexture->storage(int(_shadowResolution.x()), int(_shadowResolution.y()), 1
				, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
			_shadowCamera->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y())});
			_shadowCamera->setMask(_attachedNodeList[0]->getMask());
			_shadowCamera->setProjectionMatrixAsPerspective(2.0f * _outerAngle, 1.0f, 0.1f, _attenuationRange);
		}
		if (_shadowCamera.valid())
			_shadowCamera->setVisible(_shadowEnabled);
		_needUpdateShadowMap = false;
	}
}

void veSpotLight::setEnabled(bool isEnabled)
{
    veLight::setEnabled(isEnabled);
    if (_shadowEnabled && _shadowCamera.valid()){
        _shadowCamera->setVisible(isEnabled);
    }
}

veSpotLight::~veSpotLight()
{
}

veIBLight::veIBLight()
: veLight(IB)
{
}

veIBLight::~veIBLight()
{
}
