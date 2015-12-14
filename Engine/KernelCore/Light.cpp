#include "Light.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "LightRenderer.h"

const veVec2 veLight::DEFAULT_SHADOW_RESOLUTION = veVec2(256);
const veVec2 veLight::DEFAULT_SHADOW_AREA = veVec2(100.0f);
const float  veLight::DEFAULT_SHADOW_BIAS       = 0.0f;
const float  veLight::DEFAULT_SHADOW_STRENGTH   = 1.0f;

veLight* veLight::CURRENT_LIGHT = nullptr;

const std::string veLight::DEFUALT_LIGHT_TEXTURE_UNIFORM_NAME                        = "ve_lightTex";
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


static const std::string DEFAULT_DIRECTIONAL_LIGHT_SHADOW_MAP_TEXTURE_NAME = "DEFAULT_DIRECTIONAL_LIGHT_SHADOW_MAP_TEXTURE_NAME";
static const std::string DEFAULT_POINT_LIGHT_SHADOW_MAP_TEXTURE_NAME = "DEFAULT_POINT_LIGHT_SHADOW_MAP_TEXTURE_NAME";
static const std::string DEFAULT_SPOT_LIGHT_SHADOW_MAP_TEXTURE_NAME = "DEFAULT_SPOT_LIGHT_SHADOW_MAP_TEXTURE_NAME";


static const veMat4 LIGHT_BIAS_MAT = veMat4(0.5f, 0.0f, 0.0f, 0.5f
										  , 0.0f, 0.5f, 0.0f, 0.5f
										  , 0.0f, 0.0f, 0.5f, 0.5f
										  , 0.0f, 0.0f, 0.0f, 1.0f);

veLight::veLight(LightType type, veSceneManager *sm)
	: veRenderableObject(sm)
	, _type(type)
	, _shadowArea(DEFAULT_SHADOW_AREA)
	, _shadowResolution(DEFAULT_SHADOW_RESOLUTION)
	, _needRefreshShadow(true)
	, _needRefreshShadowCamera(true)
{
}

void veLight::fetechLightParams()
{
	auto pass = _materials->getMaterial(0)->getTechnique(0)->getPass(0);
	_color           = pass->getUniform("u_lightColor");
	_intensity       = pass->getUniform("u_lightIntensity");
	_lightMatrix     = pass->getUniform("u_lightMatrix");
	_shadowEnabled   = pass->getUniform("u_lightShadowEnabled");
	_shadowBias      = pass->getUniform("u_lightShadowBias");
	_shadowStrength  = pass->getUniform("u_lightShadowStrength");
	_isUseSoftShadow = pass->getUniform("u_lightShadowSoft");
	_shadowSoftness  = pass->getUniform("u_lightShadowSoftness");
}

veLight::~veLight()
{

}

void veLight::update(veNode *node, veSceneManager *sm)
{
	//if (_shadowEnabled) {
	//	updateShadowTexture();
	//	updateShadowCamera(node);
	//}
	veRenderableObject::update(node, sm);
}

void veLight::setMaterialArray(veMaterialArray *material)
{
	_materials = material;
	fetechLightParams();
}

veVec3 veLight::getColor() const
{
	veVec3 val;
	_color->getValue(val);
	return val;
}

float veLight::getIntensity() const
{
	bool val;
	_intensity->getValue(val);
	return val;
}

void veLight::shadowEnable(bool isEnabled)
{
#if VE_PLATFORM == VE_PLATFORM_ANDROID
	if (_type == POINT) {
		veLog("Unfortunately, point light shadow not support on android!(The GLSL not support samplerCubeArray or samplerCubeArrayShadow)");
		return;
	}
#endif
	_shadowEnabled->setValue(isEnabled? 1.0f: 0.0f);
}

bool veLight::isShadowEnabled() const
{
	float val;
	_shadowEnabled->getValue(val);
	return val == 1.0f? true: false;
}

void veLight::setShadowResolution(const veVec2 &resolution)
{
	if (_shadowResolution == resolution)
		return;
	_shadowResolution = resolution;
	_needRefreshShadow = true;
}

float veLight::getShadowBias() const
{
	float val;
	_shadowBias->getValue(val);
	return val;
}

float veLight::getShadowStrength() const
{
	float val;
	_shadowStrength->getValue(val);
	return val;
}

bool veLight::isUseSoftShadow() const
{
	float val;
	_isUseSoftShadow->getValue(val);
	return val == 1.0f? true: false;
}

float veLight::getShadowSoftness() const
{
	float val;
	_shadowSoftness->getValue(val);
	return val;
}

veDirectionalLight::veDirectionalLight(veSceneManager *sm)
	: veLight(DIRECTIONAL, sm)
{
	_renderer = new veDirectionalLightRenderer;
	_renderer->setRenderStageMask(veRenderer::LIGHTINGING);
	setLightArea(veVec3(100.0f));
}

veDirectionalLight::~veDirectionalLight()
{
}

void veDirectionalLight::render(veNode *node, veCamera *camera)
{
	veMat4 lightInView = camera->viewMatrix() * node->getNodeToWorldMatrix();
	lightInView[0][3] = lightInView[1][3] = lightInView[2][3] = 0.0f;
	veVec3 direction = lightInView * -veVec3::UNIT_Z;
	direction.normalize();
	_direction->setValue(direction);
	veRenderableObject::render(node, camera);
}

void veDirectionalLight::setLightArea(const veVec3 &area)
{
	_lightArea = area;
	_boundingBox.min() = -veVec3(_lightArea) * 0.5f;
	_boundingBox.max() = veVec3(_lightArea) * 0.5f;
}

//veMat4 veDirectionalLight::getLightMatrix() const
//{
//	return LIGHT_BIAS_MAT * _shadowRenderingCam->projectionMatrix() * _shadowRenderingCam->viewMatrix();
//}

void veDirectionalLight::shadowCameraCulling()
{
	_shadowRenderingCam->cull();
}

void veDirectionalLight::shadowCameraRendering()
{
	_shadowRenderingCam->render();
}

void veDirectionalLight::updateShadowTexture()
{
	if (_needRefreshShadow) {
		if (!_shadowTexture.valid())
			_shadowTexture = _sceneManager->createTexture(DEFAULT_DIRECTIONAL_LIGHT_SHADOW_MAP_TEXTURE_NAME, veTexture::TEXTURE_2D);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, 1
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		_needRefreshShadow = false;
	}
}

void veDirectionalLight::updateShadowCamera(veNode *node)
{
	if (_needRefreshShadowCamera) {
		if (!_shadowRenderingCam.valid()) {
			_shadowRenderingCam = _sceneManager->createCamera(_name + std::string("-ShadowCamera"));
			auto fbo = veFrameBufferObjectManager::instance()->createFrameBufferObject(_name + std::string("-ShadowFBO"));
			_shadowRenderingCam->setFrameBufferObject(fbo);
		}

		_shadowRenderingCam->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
		_shadowRenderingCam->setProjectionMatrixAsOrtho(-_shadowArea.x() * 0.5f, _shadowArea.x() * 0.5f, -_shadowArea.y() * 0.5f, _shadowArea.y() * 0.5f
			, 0.1f, 1000.0f);
		_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTexture.get());
		_shadowRenderingCam->setMask(node->getMask());

		_needRefreshShadowCamera = false;
	}

	_shadowRenderingCam->setMatrix(node->getNodeToWorldMatrix());
}

void veDirectionalLight::fetechLightParams()
{
	veLight::fetechLightParams();
	auto pass = _materials->getMaterial(0)->getTechnique(0)->getPass(0);
	_direction = pass->getUniform("u_lightDirection");
}

vePointLight::vePointLight(veSceneManager *sm)
	: veLight(POINT, sm)
{
	_renderer = new vePointLightRenderer;
	_renderer->setRenderStageMask(veRenderer::LIGHTINGING);
}

vePointLight::~vePointLight()
{
}

void vePointLight::render(veNode *node, veCamera *camera)
{
	veMat4 lightInView = camera->viewMatrix() * node->getNodeToWorldMatrix();
	_position->setValue(veVec3(lightInView[0][3], lightInView[1][3], lightInView[2][3]));
	veRenderableObject::render(node, camera);
}

void vePointLight::setAttenuationRange(float range)
{
	_attenuationRangeInverse->setValue(1.0f / range);
	static_cast<vePointLightRenderer *>(_renderer.get())->setLightVolumeScale(veMat4::scale(veVec3(range)));
}

float vePointLight::getAttenuationRange() const
{
	float val;
	_attenuationRangeInverse->getValue(val);
	return 1.0f / val;
}

//veMat4 vePointLight::getLightMatrix() const
//{
//	return this->getWorldToNodeMatrix();
//}

void vePointLight::shadowCameraCulling()
{
	for (unsigned int i = 0; i < 6; ++i) {
		_shadowRenderingCam[i]->cull();
	}
}

void vePointLight::shadowCameraRendering()
{
	for (unsigned int i = 0; i < 6; ++i) {
		_shadowRenderingCam[i]->render();
	}
}

void vePointLight::updateShadowTexture()
{
	if (_needRefreshShadow) {
		if (!_shadowTexture.valid())
			_shadowTexture = _sceneManager->createTexture(DEFAULT_POINT_LIGHT_SHADOW_MAP_TEXTURE_NAME, veTexture::TEXTURE_CUBE);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, 1
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		_needRefreshShadow = false;
	}
}

void vePointLight::updateShadowCamera(veNode *node)
{
	if (_needRefreshShadowCamera) {
		char str[4];
		for (unsigned int i = 0; i < 6; ++i) {
			if (!_shadowRenderingCam[i].valid()) {
				sprintf(str, "%d", i);
				_shadowRenderingCam[i] = _sceneManager->createCamera(_name + std::string("-ShadowCamera") + std::string(str));
				auto fbo = veFrameBufferObjectManager::instance()->createFrameBufferObject(_name + std::string("-ShadowFBO") + std::string(str));
				_shadowRenderingCam[i]->setFrameBufferObject(fbo);
			}
			_shadowRenderingCam[i]->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
			_shadowRenderingCam[i]->setProjectionMatrixAsPerspective(90.0f, 1.0f, 0.1f, getAttenuationRange());
			_shadowRenderingCam[i]->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, _shadowTexture.get());
			_shadowRenderingCam[i]->setMask(node->getMask());
		}
		_needRefreshShadowCamera = false;
	}

	veMat4 ltow = node->getNodeToWorldMatrix();
	_shadowRenderingCam[0]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_X, veVec3::NEGATIVE_UNIT_Y));
	_shadowRenderingCam[1]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_X, veVec3::NEGATIVE_UNIT_Y));
	_shadowRenderingCam[2]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Y, veVec3::UNIT_Z));
	_shadowRenderingCam[3]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Y, veVec3::NEGATIVE_UNIT_Z));
	_shadowRenderingCam[4]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::UNIT_Z, veVec3::NEGATIVE_UNIT_Y));
	_shadowRenderingCam[5]->setMatrix(ltow * veMat4::lookAt(veVec3::ZERO, veVec3::NEGATIVE_UNIT_Z, veVec3::NEGATIVE_UNIT_Y));
}

void vePointLight::fetechLightParams()
{
	veLight::fetechLightParams();
	auto pass = _materials->getMaterial(0)->getTechnique(0)->getPass(0);
	_position = pass->getUniform("u_lightPosition");
	_attenuationRangeInverse = pass->getUniform("u_lightARI");
}

veSpotLight::veSpotLight(veSceneManager *sm)
	: veLight(SPOT, sm)
{
	_renderer = new veSpotLightRenderer;
	_renderer->setRenderStageMask(veRenderer::LIGHTINGING);
}

veSpotLight::~veSpotLight()
{
}

void veSpotLight::render(veNode *node, veCamera *camera)
{
	veMat4 lightInView = camera->viewMatrix() * node->getNodeToWorldMatrix();
	_position->setValue(veVec3(lightInView[0][3], lightInView[1][3], lightInView[2][3]));

	lightInView[0][3] = lightInView[1][3] = lightInView[2][3] = 0.0f;
	veVec3 direction = lightInView * -veVec3::UNIT_Z;
	direction.normalize();
	_direction->setValue(direction);
	veRenderableObject::render(node, camera);
}

float veSpotLight::getAttenuationRange() const
{
	float val;
	_attenuationRangeInverse->getValue(val);
	return 1.0f / val;
}

float veSpotLight::getInnerAngle()
{
	float val;
	_innerAngleCos->getValue(val);
	return veMath::veDegree(veMath::veAcos(val));
}

float veSpotLight::getOuterAngle()
{
	float val;
	_outerAngleCos->getValue(val);
	return veMath::veDegree(veMath::veAcos(val));
}

//veMat4 veSpotLight::getLightMatrix() const
//{
//	return LIGHT_BIAS_MAT * _shadowRenderingCam->projectionMatrix() * _shadowRenderingCam->viewMatrix();
//}

void veSpotLight::shadowCameraCulling()
{
	_shadowRenderingCam->cull();
}

void veSpotLight::shadowCameraRendering()
{
	_shadowRenderingCam->render();
}

void veSpotLight::updateShadowTexture()
{
	if (_needRefreshShadow) {
		if (!_shadowTexture.valid())
			_shadowTexture = _sceneManager->createTexture(DEFAULT_SPOT_LIGHT_SHADOW_MAP_TEXTURE_NAME, veTexture::TEXTURE_2D);
		_shadowTexture->storage(int(_shadowResolution.x()) * VE_DEVICE_PIXEL_RATIO, int(_shadowResolution.y()) * VE_DEVICE_PIXEL_RATIO, 1
			, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr, 1);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		_shadowTexture->setTexParameter(GL_TEXTURE_COMPARE_FUNC, GL_LESS);
		_needRefreshShadow = false;
	}
}

void veSpotLight::updateShadowCamera(veNode *node)
{
	if (_needRefreshShadowCamera) {
		if (!_shadowRenderingCam.valid()) {
			_shadowRenderingCam = _sceneManager->createCamera(_name + std::string("-ShadowCamera"));
			auto fbo = veFrameBufferObjectManager::instance()->createFrameBufferObject(_name + std::string("-ShadowFBO"));
			_shadowRenderingCam->setFrameBufferObject(fbo);
		}

		_shadowRenderingCam->setViewport({ 0, 0, int(_shadowResolution.x()), int(_shadowResolution.y()) });
		_shadowRenderingCam->setProjectionMatrixAsPerspective(2.0f * getOuterAngle(), 1.0f, 0.1f, getAttenuationRange());
		_shadowRenderingCam->getFrameBufferObject()->attach(GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTexture.get());
		_shadowRenderingCam->setMask(node->getMask());

		_needRefreshShadowCamera = false;
	}

	_shadowRenderingCam->setMatrix(node->getNodeToWorldMatrix());
}

void veSpotLight::fetechLightParams()
{
	veLight::fetechLightParams();
	auto pass = _materials->getMaterial(0)->getTechnique(0)->getPass(0);
	_direction = pass->getUniform("u_lightDirection");
	_position = pass->getUniform("u_lightPosition");
	_attenuationRangeInverse = pass->getUniform("u_lightARI");
	_innerAngleCos = pass->getUniform("u_lightInnerAngleCos");
	_outerAngleCos = pass->getUniform("u_lightOuterAngleCos");
}
