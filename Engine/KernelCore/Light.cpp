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
	, _needUpdateShadowMap(true)
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