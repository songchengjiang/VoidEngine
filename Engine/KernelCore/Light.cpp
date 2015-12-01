#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "Constants.h"

const unsigned int veLight::DEFUALT_LIGHT_PARAM_NUM = 8;

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec2 veLight::DEFAULT_SHADOW_AREA       = veVec2(100.0f);
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

const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME = "shadowEnabled";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME = "shadowBias";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME = "shadowStrength";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_RESOLUTION_NAME = "shadowResolution";
const std::string veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME = "shadowMap";

veLight::veLight(LightType type)
	: _type(type)
	, _color(veVec4::WHITE)
	, _intensity(1.0f)
	, _attenuationRange(1000.0f)
	, _attenuationRangeInverse(1.0f / _attenuationRange)
	, _innerAngle(0.0f)
	, _innerAngleCos(veMath::veCos(veMath::veRadian(_innerAngle)))
	, _outerAngle(45.0f)
	, _outerAngleCos(veMath::veCos(veMath::veRadian(_outerAngle)))
	, _shadowArea(DEFAULT_SHADOW_AREA)
	, _lightMatrix(veMat4::IDENTITY)
	, _shadowEnabled(false)
	, _shadowResolution(DEFAULT_SHADOW_RESOLUTION)
	, _shadowBias(DEFAULT_SHADOW_BIAS)
	, _shadowStrength(DEFAULT_SHADOW_STRENGTH)
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
		updateShadowCamera();
		updateShadow();
	}
}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

void veLight::setLightType(LightType type)
{
	if (_type == type)
		return;
	_type = type;
	_needRefreshShadow = true;
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

void veLight::updateShadow()
{
	if (_needRefreshShadow) {
		_shadowTexture = _sceneManager->createTexture(_name + std::string("-ShadowTexture"), _type == POINT ? veTexture::TEXTURE_CUBE : veTexture::TEXTURE_2D);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, 1, GL_R8, GL_RED
			, GL_UNSIGNED_BYTE, nullptr, log2(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO) + 1);

		switch (_type)
		{
		case DIRECTIONAL:
		{
			_shadowRenderingCam[0]->setProjectionMatrixAsOrtho(-_shadowArea.x() * 0.5f, _shadowArea.x() * 0.5f, -_shadowArea.y() * 0.5f, _shadowArea.y() * 0.5f
				, 1.0f, _attenuationRange);
			_shadowRenderingCam[0]->getFrameBufferObject()->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _shadowTexture.get());
		}
		break;

		case POINT: 
		{
			for (unsigned int i = 0; i < 6; ++i) {
				_shadowRenderingCam[i]->setProjectionMatrixAsPerspective(90.0f, 1.0f, 1.0f, _attenuationRange);
				_shadowRenderingCam[i]->getFrameBufferObject()->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _shadowTexture.get());
			}
		}
		break;

		case SPOT:
		{
			_shadowRenderingCam[0]->setProjectionMatrixAsPerspective(_outerAngle, 1.0, 1.0, _attenuationRange);
			_shadowRenderingCam[0]->getFrameBufferObject()->attach(GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _shadowTexture.get());
		}
			break;

		case AREA:

			break;
		default:
			break;
		}

		_needRefreshShadow = false;
	}
}

void veLight::updateShadowCamera()
{
	if (!_shadowRenderingCam[0].valid()) {
		char str[4];
		for (unsigned int i = 0; i < 6; ++i) {
			if (!_shadowRenderingCam[i].valid()) {
				sprintf(str, "%d", i);
				_shadowRenderingCam[i] = _sceneManager->createCamera(_name + std::string("-ShadowCamera") + std::string(str));
				auto fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(_name + std::string("-ShadowFBO") + std::string(str));
				_shadowRenderingCam[i]->setFrameBufferObject(fbo);
			}
		}
	}

	switch (_type)
	{
	case DIRECTIONAL:
	case SPOT:
	{
		_shadowRenderingCam[0]->setMatrix(this->getNodeToWorldMatrix());
		_shadowRenderingCam[0]->update(_sceneManager, veMat4::IDENTITY);
	}
	break;

	case POINT: 
	{
		veMat4 ltow = this->getNodeToWorldMatrix();
		_shadowRenderingCam[0]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_X, veVec3::UNIT_Y));
		_shadowRenderingCam[1]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_X, veVec3::UNIT_Y));
		_shadowRenderingCam[2]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Y, veVec3::UNIT_Z));
		_shadowRenderingCam[3]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Y, veVec3::NEGATIVE_UNIT_Z));
		_shadowRenderingCam[4]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Z, veVec3::UNIT_Y));
		_shadowRenderingCam[5]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Z, veVec3::UNIT_Y));

		for (unsigned int i = 0; i < 6; ++i)
			_shadowRenderingCam[i]->update(_sceneManager, veMat4::IDENTITY);
	}
	break;

	case AREA:

		break;
	default:
		break;
	}
}

//void veLight::render(veCamera *camera)
//{
//	veNode::render(camera);
//	_camera = nullptr;
//	if (_mask & camera->getMask())
//		_camera = camera;
//}