#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"

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
	, _needRefreshShadowTexture(false)
	, _lightMatrix(veMat4::IDENTITY)
{
}

veLight::~veLight()
{

}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

void veLight::update(veSceneManager *sm, const veMat4 &transform)
{
	if (_shadowEnabled) {
		if (_needRefreshShadowTexture) {
			if (!_shadowTexture.valid()) {
				_shadowTexture = _sceneManager->createTexture(_name + std::string("-shadowTex")
					, (_type == veLight::DIRECTIONAL || _type == veLight::SPOT)? veTexture::TEXTURE_2D: veTexture::TEXTURE_CUBE);
			}
			_shadowTexture->storage(int(_shadowResolution.x()), int(_shadowResolution.y()), 1
				, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
			_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
			_needRefreshShadowTexture = false;
		}
	}
	else {
		if (_shadowTexture.valid()) {
			_shadowTexture->storage(0, 0, 0, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
		}
	}
	veNode::update(sm, transform);
}

void veLight::setShadowResolution(const veVec2 &resolution)
{
	if (_shadowResolution == resolution)
		return;
	_shadowResolution = resolution;
	_needRefreshShadowTexture = true;
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

vePointLight::vePointLight()
	: veLight(POINT)
{
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

veSpotLight::~veSpotLight()
{
}