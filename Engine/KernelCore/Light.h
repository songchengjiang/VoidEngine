#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Component.h"
#include "Texture.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Vector4.h"
#include "BaseCore/Matrix4.h"
#include <map>

class veCamera;
class veSceneManager;
class VE_EXPORT veLight : public veComponent
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
        IB,
		AREA,
	};

	~veLight();

    virtual void update(veSceneManager *sm) override;
	//virtual void render(veCamera *camera) override;

    virtual void setEnabled(bool isEnabled) { _isEnabled = isEnabled; }
    bool isEnabled() const { return _isEnabled; }
	LightType getLightType() const { return _type; }
    
	void setColor(const veVec3 &color) { _color = color; }
	const veVec3& getColor() const { return _color; }
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
	void setUseSoftShadow(bool use) { _isUseSoftShadow = use; }
	bool isUseSoftShadow() const { return _isUseSoftShadow; }
	void setShadowSoftness(float softness) { _shadowSoftness = softness; }
	float getShadowSoftness() const { return _shadowSoftness; }

	veTexture* getShadowTexture() { return _shadowTexture.get(); }

protected:
	veLight(LightType type);

    virtual void updateShadow(veSceneManager *sm){};

protected:

	LightType _type;
    bool      _isEnabled;

	veVec3 _color;
	float  _intensity;
	float  _attenuationRange;
	float  _attenuationRangeInverse;
	veMat4 _lightInCamMatrix;

	bool   _shadowEnabled;
	veVec2 _shadowResolution;
	float _shadowBias;
	float _shadowStrength;
	float _shadowSoftness;
	bool  _isUseSoftShadow;

	VE_Ptr<veTexture>  _shadowTexture;
	bool               _needUpdateShadowMap;
};

class VE_EXPORT veDirectionalLight : public veLight
{
	friend class veSceneManager;
public:

	~veDirectionalLight();

    virtual void setEnabled(bool isEnabled) override;
	veCamera* getShadowCamera(unsigned short idx) const { return _shadowCameras[idx].get(); }
    void setShadowCascadedCount(unsigned short count) { _shadowCascadedCount = count < 4? count: 4; _needUpdateShadowMap = true; }
    unsigned short getShadowCascadedCount() const { return _shadowCascadedCount; }
    void setShadowCascadedLevelScale(unsigned short level, float scl) { _shadowCascadedLevelScales[level] = scl; }
    float getShadowCascadedLevelScale(unsigned short level) const { return _shadowCascadedLevelScales[level]; }

protected:

	veDirectionalLight();
	virtual void updateShadow(veSceneManager *sm) override;

protected:

	VE_Ptr<veCamera> _shadowCameras[4];
    float            _shadowCascadedLevelScales[4];
    unsigned short   _shadowCascadedCount;
};

class VE_EXPORT vePointLight : public veLight
{
	friend class veSceneManager;
public:

	~vePointLight();

    virtual void setEnabled(bool isEnabled) override;
	veCamera* getShadowCamera(unsigned short idx) const { return _shadowCameras[idx].get(); }

protected:

	vePointLight();
	virtual void updateShadow(veSceneManager *sm) override;

protected:

	VE_Ptr<veCamera> _shadowCameras[6];
};

class VE_EXPORT veSpotLight : public veLight
{
	friend class veSceneManager;
public:

	~veSpotLight();

    virtual void setEnabled(bool isEnabled) override;
	void setInnerAngle(float innerAng) { _innerAngle = innerAng; _innerAngleCos = veMath::veCos(veMath::veRadian(_innerAngle)); }
	float getInnerAngle() { return _innerAngle; }
	float getInnerAngleCos() { return _innerAngleCos; }
	void setOuterAngle(float outerAng) { _outerAngle = outerAng; _outerAngleCos = veMath::veCos(veMath::veRadian(_outerAngle)); }
	float getOuterAngle() { return _outerAngle; }
	float getOuterAngleCos() { return _outerAngleCos; }

	veCamera* getShadowCamera() const { return _shadowCamera.get(); }

protected:

	veSpotLight();
	virtual void updateShadow(veSceneManager *sm) override;

protected:

	float  _innerAngle;
	float  _innerAngleCos;
	float  _outerAngle;
	float  _outerAngleCos;

	VE_Ptr<veCamera> _shadowCamera;
};

class VE_EXPORT veIBLight : public veLight
{
    friend class veSceneManager;
public:
    
    ~veIBLight();
    
    void setDiffuseLightingTexture(veTexture *diffTex) { _diffuseLightingTex = diffTex; }
    veTexture* getDiffuseLightingTexture() const { return _diffuseLightingTex.get(); }
    void setSpecularLightingTexture(veTexture *specTex) { _specularLightingTex = specTex; }
    veTexture* getSpecularLightingTexture() const { return _specularLightingTex.get(); }
    
    
protected:
    
    veIBLight();
    
protected:
    
    VE_Ptr<veTexture> _diffuseLightingTex;
    VE_Ptr<veTexture> _specularLightingTex;
};

typedef std::vector< VE_Ptr<veLight> > veLightList;
typedef std::map<veLight::LightType, veLightList> veLightListMap;

#endif
