#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Node.h"
#include "Parameter.h"
#include "Camera.h"

class VE_EXPORT veLight : public veNode
{
	friend class veSceneManager;
public:

	static const veVec2 DEFAULT_SHADOW_AREA;
	static const veVec2 DEFAULT_SHADOW_RESOLUTION;
	static const float  DEFAULT_SHADOW_BIAS;
	static const float  DEFAULT_SHADOW_STRENGTH;
	static const std::string DEFUALT_LIGHT_UNIFORM_SAMPLES_NAME;

	static veLight* CURRENT_LIGHT;

	enum LightType
	{
		DIRECTIONAL,
		POINT,
		SPOT,
		AREA,
	};

	~veLight();

	virtual void update(veSceneManager *sm, const veMat4 &transform) override;
	virtual void visit(veNodeVisitor &visitor) override;
	//virtual void render(veCamera *camera) override;

	virtual void setMask(unsigned int mask, bool isOverride = false) override;

	LightType getLightType() const { return _type; }
	void setColor(const veVec4 &color) { _color = color; }
	const veVec4& getColor() const { return _color; }
	void setIntensity(float intensity) { _intensity = intensity; }
	float getIntensity() const { return _intensity; }
	void setAttenuationRange(float range) { _attenuationRange = range; _attenuationRangeInverse = 1.0f / _attenuationRange; }
	float getAttenuationRange() const { return _attenuationRange; }
	float getAttenuationRangeInverse() const { return _attenuationRangeInverse; }

	void shadowEnable(bool isEnabled);
	bool isShadowEnabled() const { return _shadowEnabled; };
	void setShadowResolution(const veVec2 &resolution);
	const veVec2& getShadowResolution() const { return _shadowResolution; }
	void setShadowBias(float bias) { _shadowBias = bias; }
	float getShadowBias() const { return _shadowBias; }
	void setShadowStrength(float strength) { _shadowStrength = strength; }
	float getShadowStrength() const { return _shadowStrength; }
	void setShadowArea(const veVec2 &area) { _shadowArea = area; _needRefreshShadow = true; }
	const veVec2& getShadowArea() const { return _shadowArea; }
	void setUseSoftShadow(bool use) { _isUseSoftShadow = use; }
	bool isUseSoftShadow() const { return _isUseSoftShadow; }
	void setShadowSoftness(float softness) { _shadowSoftness = softness; }
	float getShadowSoftness() const { return _shadowSoftness; }

	void setLightInCameraMatrix(const veMat4 &mat) { _lightInCamMatrix = mat; }
	const veMat4& getLightInCameraMatrix() { return _lightInCamMatrix; }

	virtual veMat4 getLightMatrix() const = 0;

	virtual void shadowCameraCulling() = 0;
	virtual void shadowCameraRendering() = 0;

protected:
	veLight(LightType type);

	virtual void updateSceneManager() override;
	virtual void updateShadowTexture() = 0;
	virtual void updateShadowCamera() = 0;

protected:

	LightType _type;

	veVec4 _color;
	float  _intensity;
	float  _attenuationRange;
	float  _attenuationRangeInverse;
	veMat4 _lightInCamMatrix;
	unsigned int _currentLightIndex;

	bool _shadowEnabled;
	veVec2 _shadowResolution;
	veVec2 _shadowArea;
	float _shadowBias;
	float _shadowStrength;
	float _shadowSoftness;
	bool  _isUseSoftShadow;
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

	virtual veMat4 getLightMatrix() const override;

	virtual void shadowCameraCulling() override;
	virtual void shadowCameraRendering() override;

	static veTexture* getShadowTexture() { return _shadowTexture.get(); }
	static unsigned int totalLightNum() { return _totalDirLightNum; }

protected:

	veDirectionalLight();
	virtual void updateShadowTexture() override;
	virtual void updateShadowCamera() override;

protected:

	VE_Ptr<veCamera>  _shadowRenderingCam;
	static VE_Ptr<veTexture> _shadowTexture;
	static unsigned int _totalDirLightNum;
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

	virtual veMat4 getLightMatrix() const override;

	virtual void shadowCameraCulling() override;
	virtual void shadowCameraRendering() override;

	static veTexture* getShadowTexture() { return _shadowTexture.get(); }
	static unsigned int totalLightNum() { return _totalPointLightNum; }

protected:

	vePointLight();
	virtual void updateShadowTexture() override;
	virtual void updateShadowCamera() override;

protected:

	VE_Ptr<veCamera>  _shadowRenderingCam[6];

	static VE_Ptr<veTexture> _shadowTexture;
	static unsigned int _totalPointLightNum;
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

	virtual veMat4 getLightMatrix() const override;


	void setInnerAngle(float innerAng) { _innerAngle = innerAng; _innerAngleCos = veMath::veCos(veMath::veRadian(_innerAngle)); }
	float getInnerAngle() { return _innerAngle; }
	float getInnerAngleCos() { return _innerAngleCos; }
	void setOuterAngle(float outerAng) { _outerAngle = outerAng; _outerAngleCos = veMath::veCos(veMath::veRadian(_outerAngle)); }
	float getOuterAngle() { return _outerAngle; }
	float getOuterAngleCos() { return _outerAngleCos; }

	virtual void shadowCameraCulling() override;
	virtual void shadowCameraRendering() override;

	static veTexture* getShadowTexture() { return _shadowTexture.get(); }
	static unsigned int totalLightNum() { return _totalSpotLightNum; }

protected:

	veSpotLight();
	virtual void updateShadowTexture() override;
	virtual void updateShadowCamera() override;

protected:

	float  _innerAngle;
	float  _innerAngleCos;
	float  _outerAngle;
	float  _outerAngleCos;

	VE_Ptr<veCamera>  _shadowRenderingCam;

	static VE_Ptr<veTexture> _shadowTexture;
	static unsigned int _totalSpotLightNum;
};

typedef std::vector< VE_Ptr<veLight> > veLightList;

#endif