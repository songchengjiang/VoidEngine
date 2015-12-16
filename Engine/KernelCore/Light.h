#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "RenderableObject.h"
#include "Parameter.h"
#include "Camera.h"

class VE_EXPORT veLight : public veRenderableObject
{
	friend class veSceneManager;
public:

	static const veVec2 DEFAULT_SHADOW_AREA;
	static const veVec2 DEFAULT_SHADOW_RESOLUTION;
	static const float  DEFAULT_SHADOW_BIAS;
	static const float  DEFAULT_SHADOW_STRENGTH;
	static const std::string DEFUALT_LIGHT_TEXTURE_UNIFORM_NAME;

	static veLight* CURRENT_LIGHT;

	enum LightType
	{
		DIRECTIONAL,
		POINT,
		SPOT,
		AREA,
	};

	~veLight();

	virtual void update(veNode *node, veSceneManager *sm) override;
	//virtual void render(veCamera *camera) override;

	LightType getLightType() const { return _type; }
	void setColor(const veVec3 &color) { _color->setValue(color); }
	veVec3 getColor() const;
	void setIntensity(float intensity) { _intensity->setValue(intensity); }
	float getIntensity() const;

	void shadowEnable(bool isEnabled);
	bool isShadowEnabled() const;
	void setShadowResolution(const veVec2 &resolution);
	const veVec2& getShadowResolution() const { return _shadowResolution; }
	void setShadowBias(float bias) { _shadowBias->setValue(bias); }
	float getShadowBias() const;
	void setShadowStrength(float strength) { _shadowStrength->setValue(strength); }
	float getShadowStrength() const;
	void setShadowArea(const veVec2 &area) { _shadowArea = area; _needRefreshShadow = true; }
	const veVec2& getShadowArea() const { return _shadowArea; }
	void setUseSoftShadow(bool use) { _isUseSoftShadow->setValue(use ? 1.0f: 0.0f); }
	bool isUseSoftShadow() const;
	void setShadowSoftness(float softness) { _shadowSoftness->setValue(softness); }
	float getShadowSoftness() const;

	virtual void shadowCameraCulling() = 0;
	virtual void shadowCameraRendering() = 0;

protected:
	veLight(LightType type, veSceneManager *sm);

	virtual void updateShadowTexture() = 0;
	virtual void updateShadowCamera(veNode *node) = 0;
	virtual void initMaterials() = 0;

protected:

	LightType _type;

	VE_Ptr<veUniform> _color;
	VE_Ptr<veUniform> _intensity;
	VE_Ptr<veUniform> _lightMatrix;
	VE_Ptr<veUniform> _shadowEnabled;
	VE_Ptr<veUniform> _shadowBias;
	VE_Ptr<veUniform> _shadowStrength;
	VE_Ptr<veUniform> _isUseSoftShadow;
	VE_Ptr<veUniform> _shadowSoftness;

	veVec2 _shadowResolution;
	veVec2 _shadowArea;
	bool _needRefreshShadow;
	bool _needRefreshShadowCamera;
};

class VE_EXPORT veDirectionalLight : public veLight
{
	friend class veSceneManager;
public:

	static const std::string DEFUALT_LIGHT_UNIFORM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_NUM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_COLOR_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME;

	~veDirectionalLight();

	virtual void render(veNode *node, veCamera *camera) override;

	void setLightArea(const veVec3 &area);
	virtual void shadowCameraCulling() override;
	virtual void shadowCameraRendering() override;

	veTexture* getShadowTexture() { return _shadowTexture.get(); }

protected:

	veDirectionalLight(veSceneManager *sm);
	virtual void updateShadowTexture() override;
	virtual void updateShadowCamera(veNode *node) override;
	virtual void initMaterials() override;

protected:

	VE_Ptr<veUniform> _direction;
	VE_Ptr<veCamera>  _shadowRenderingCam;
	VE_Ptr<veTexture> _shadowTexture;
	veVec3            _lightArea;
};

class VE_EXPORT vePointLight : public veLight
{
	friend class veSceneManager;
public:

	static const std::string DEFUALT_LIGHT_UNIFORM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_NUM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_POSITION_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_COLOR_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME;

	~vePointLight();

	virtual void render(veNode *node, veCamera *camera) override;

	void setAttenuationRange(float range);
	float getAttenuationRange() const;

	virtual void shadowCameraCulling() override;
	virtual void shadowCameraRendering() override;

	veTexture* getShadowTexture() { return _shadowTexture.get(); }

protected:

	vePointLight(veSceneManager *sm);
	virtual void updateShadowTexture() override;
	virtual void updateShadowCamera(veNode *node) override;
	virtual void initMaterials() override;

protected:

	VE_Ptr<veUniform> _position;
	VE_Ptr<veUniform> _attenuationRangeInverse;
	VE_Ptr<veCamera>  _shadowRenderingCam[6];
	VE_Ptr<veTexture> _shadowTexture;
};

class VE_EXPORT veSpotLight : public veLight
{
	friend class veSceneManager;
public:

	static const std::string DEFUALT_LIGHT_UNIFORM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_NUM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_POSITION_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_COLOR_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME;

	~veSpotLight();

	virtual void render(veNode *node, veCamera *camera) override;

	void setAttenuationRange(float range);
	float getAttenuationRange() const;

	void setInnerAngle(float innerAng) { _innerAngle = innerAng; _innerAngleCos->setValue(veMath::veCos(veMath::veRadian(innerAng))); }
	float getInnerAngle();
	void setOuterAngle(float outerAng);
	float getOuterAngle();

	virtual void shadowCameraCulling() override;
	virtual void shadowCameraRendering() override;

	veTexture* getShadowTexture() { return _shadowTexture.get(); }

protected:

	veSpotLight(veSceneManager *sm);
	virtual void updateShadowTexture() override;
	virtual void updateShadowCamera(veNode *node) override;
	virtual void initMaterials() override;

protected:

	VE_Ptr<veUniform> _direction;
	VE_Ptr<veUniform> _position;
	VE_Ptr<veUniform> _attenuationRangeInverse;
	VE_Ptr<veUniform> _innerAngleCos;
	VE_Ptr<veUniform> _outerAngleCos;
	float             _attenuationRange;
	float             _innerAngle;
	float             _outerAngle;

	VE_Ptr<veCamera>  _shadowRenderingCam;
	VE_Ptr<veTexture> _shadowTexture;
};

typedef std::vector< VE_Ptr<veLight> > veLightList;

#endif