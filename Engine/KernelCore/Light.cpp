#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "Constants.h"

const unsigned int veLight::DEFUALT_LIGHT_PARAM_NUM = 8;

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec3 veLight::DEFAULT_SHADOW_RANGE      = veVec3(512, 512, 1000);
const float  veLight::DEFAULT_SHADOW_BIAS       = 0.05f;
const float  veLight::DEFAULT_SHADOW_STRENGTH   = 1.0f;
const std::string veLight::DEFUALT_LIGHT_UNIFORM_NAME = "ve_Light";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_MAX_NAME = "ve_LightMax";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME = "ve_LightNum";

const std::string veLight::DEFUALT_LIGHT_UNIFORM_TYPE_NAME = "type";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME = "position";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME = "direction";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME = "color";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME = "intensity";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME = "attenuationRangeInverse";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME = "innerAngleCos";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME = "outerAngleCos";

veLight::veLight(LightType type)
	: _type(type)
	, _color(veVec4::WHITE)
	, _intensity(1.0f)
	, _attenuationRange(1000.0f)
	, _attenuationRangeInverse(1.0f / _attenuationRange)
	, _innerAngle(0.0f)
	, _innerAngleCos(veMath::veCos(veMath::veRadian(_innerAngle)))
	, _outerAngle(veMath::QUARTER_PI)
	, _outerAngleCos(veMath::veCos(veMath::veRadian(_outerAngle)))
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
			_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTexture.get());
			_needRefreshShadow = false;
		}

		_shadowRenderingCam->setMatrix(this->getNodeToWorldMatrix());
	}
}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

void veLight::updateSceneManager()
{

}

//void veLight::render(veCamera *camera)
//{
//	veNode::render(camera);
//	_camera = nullptr;
//	if (_mask & camera->getMask())
//		_camera = camera;
//}