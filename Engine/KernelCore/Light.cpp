#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "Constants.h"

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec2 veLight::DEFAULT_SHADOW_AREA = veVec2(100.0f);
const float  veLight::DEFAULT_SHADOW_BIAS       = 0.0f;
const float  veLight::DEFAULT_SHADOW_STRENGTH   = 1.0f;

veLight* veLight::CURRENT_LIGHT = nullptr;

const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NAME                     = "ve_dirLight";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME                 = "ve_dirLightNum";

const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME             = "ve_dirLightVisible";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME           = "ve_dirLightDirection";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME               = "ve_dirLightColor";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME           = "ve_dirLightIntensity";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME      = "ve_dirLightShadowEnabled";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME         = "ve_dirLightShadowBias";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME     = "ve_dirLightShadowStrength";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME   = "ve_dirLightShadowSoft";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME     = "ve_dirLightShadowSoftness";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME       = "ve_dirLightShadowMat";
const std::string veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME          = "ve_dirLightShadowMap";

const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_NAME                           = "ve_pointLight";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME                       = "ve_pointLightNum";

const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME                   = "ve_pointLightVisible";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME                  = "ve_pointLightPosition";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME                     = "ve_pointLightColor";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME                 = "ve_pointLightIntensity";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME = "ve_pointLightARI";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME            = "ve_pointLightShadowEnabled";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME               = "ve_pointLightShadowBias";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME           = "ve_pointLightShadowStrength";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME         = "ve_pointLightShadowSoft";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME           = "ve_pointLightShadowSoftness";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME             = "ve_pointLightShadowMat";
const std::string vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME                = "ve_pointLightShadowMap";

const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_NAME                            = "ve_spotLight";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME                        = "ve_spotLightNum";

const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME                    = "ve_spotLightVisible";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME                   = "ve_spotLightPosition";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME                  = "ve_spotLightDirection";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME                      = "ve_spotLightColor";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME                  = "ve_spotLightIntensity";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME  = "ve_spotLightARI";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME            = "ve_spotLightInnerAngleCos";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME            = "ve_spotLightOuterAngleCos";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME             = "ve_spotLightShadowEnabled";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME                = "ve_spotLightShadowBias";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME            = "ve_spotLightShadowStrength";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME          = "ve_spotLightShadowSoft";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME            = "ve_spotLightShadowSoftness";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME              = "ve_spotLightShadowMat";
const std::string veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME                 = "ve_spotLightShadowMap";

VE_Ptr<veTexture> veDirectionalLight::_shadowTexture;
VE_Ptr<veTexture> veSpotLight::_shadowTexture;
VE_Ptr<veTexture> vePointLight::_shadowTexture;


unsigned int veDirectionalLight::_totalDirLightNum = 0;
unsigned int vePointLight::_totalPointLightNum = 0;
unsigned int veSpotLight::_totalSpotLightNum = 0;

static const std::string DEFAULT_DIRECTIONAL_LIGHT_SHADOW_MAP_TEXTURE_NAME = "DEFAULT_DIRECTIONAL_LIGHT_SHADOW_MAP_TEXTURE_NAME";
static const std::string DEFAULT_POINT_LIGHT_SHADOW_MAP_TEXTURE_NAME = "DEFAULT_POINT_LIGHT_SHADOW_MAP_TEXTURE_NAME";
static const std::string DEFAULT_SPOT_LIGHT_SHADOW_MAP_TEXTURE_NAME = "DEFAULT_SPOT_LIGHT_SHADOW_MAP_TEXTURE_NAME";


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
	, _shadowSoftness(0.1f)
	, _isUseSoftShadow(false)
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

void veLight::shadowEnable(bool isEnabled)
{
#if VE_PLATFORM == VE_PLATFORM_ANDROID
	if (_type == POINT) {
		veLog("Unfortunately, point light shadow not support on android!(The GLSL not support samplerCubeArray or samplerCubeArrayShadow)");
		return;
	}
#endif
	_shadowEnabled = isEnabled;
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
	_currentLightIndex = _totalDirLightNum;
	++_totalDirLightNum;
}

veDirectionalLight::~veDirectionalLight()
{
	--_totalDirLightNum;
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
			_shadowTexture = _sceneManager->createTexture(DEFAULT_DIRECTIONAL_LIGHT_SHADOW_MAP_TEXTURE_NAME, veTexture::TEXTURE_2D_ARRAY);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, _totalDirLightNum
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
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
			auto fbo = veFrameBufferObjectManager::instance()->createFrameBufferObject(_name + std::string("-ShadowFBO"));
			_shadowRenderingCam->setFrameBufferObject(fbo);
		}

		_shadowRenderingCam->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
		_shadowRenderingCam->setProjectionMatrixAsOrtho(-_shadowArea.x() * 0.5f, _shadowArea.x() * 0.5f, -_shadowArea.y() * 0.5f, _shadowArea.y() * 0.5f
			, 0.1f, _attenuationRange);
		_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTexture.get(), _currentLightIndex);
		_shadowRenderingCam->setMask(_mask);

		_needRefreshShadowCamera = false;
	}

	_shadowRenderingCam->setMatrix(this->getNodeToWorldMatrix());
}

vePointLight::vePointLight()
	: veLight(POINT)
{
	_currentLightIndex = _totalPointLightNum;
	++_totalPointLightNum;
}

vePointLight::~vePointLight()
{
	--_totalPointLightNum;
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
			_shadowTexture = _sceneManager->createTexture(DEFAULT_POINT_LIGHT_SHADOW_MAP_TEXTURE_NAME, veTexture::TEXTURE_CUBE_ARRAY);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, _totalPointLightNum * 6
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		_needRefreshShadow = false;
	}
}

void vePointLight::updateShadowCamera()
{
	if (_needRefreshShadowCamera) {
		char str[4];
		unsigned int layerBase = _currentLightIndex * 6;
		for (unsigned int i = 0; i < 6; ++i) {
			if (!_shadowRenderingCam[i].valid()) {
				sprintf(str, "%d", i);
				_shadowRenderingCam[i] = _sceneManager->createCamera(_name + std::string("-ShadowCamera") + std::string(str));
				auto fbo = veFrameBufferObjectManager::instance()->createFrameBufferObject(_name + std::string("-ShadowFBO") + std::string(str));
				_shadowRenderingCam[i]->setFrameBufferObject(fbo);
			}
			_shadowRenderingCam[i]->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
			_shadowRenderingCam[i]->setProjectionMatrixAsPerspective(90.0f, 1.0f, 0.1f, _attenuationRange);
			_shadowRenderingCam[i]->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _shadowTexture.get(), layerBase + i);
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
	_currentLightIndex = _totalSpotLightNum;
	++_totalSpotLightNum;
}

veSpotLight::~veSpotLight()
{
	--_totalSpotLightNum;
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
			_shadowTexture = _sceneManager->createTexture(DEFAULT_SPOT_LIGHT_SHADOW_MAP_TEXTURE_NAME, veTexture::TEXTURE_2D_ARRAY);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, _totalSpotLightNum
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
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
			auto fbo = veFrameBufferObjectManager::instance()->createFrameBufferObject(_name + std::string("-ShadowFBO"));
			_shadowRenderingCam->setFrameBufferObject(fbo);
		}

		_shadowRenderingCam->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
		_shadowRenderingCam->setProjectionMatrixAsPerspective(2.0f * _outerAngle, 1.0f, 0.1f, _attenuationRange);
		_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTexture.get(), _currentLightIndex);
		_shadowRenderingCam->setMask(_mask);

		_needRefreshShadowCamera = false;
	}

	_shadowRenderingCam->setMatrix(this->getNodeToWorldMatrix());
}