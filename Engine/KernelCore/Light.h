#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Node.h"

class veCamera;
class VE_EXPORT veLight : public veNode
{
	friend class veSceneManager;
	friend class veShadowGenerator;
	friend class veDeferredLightSceneIlluminator;
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

	virtual void visit(veNodeVisitor &visitor) override;
	//virtual void render(veCamera *camera) override;

	LightType getLightType() const { return _type; }
	void setColor(const veVec3 &color) { _color = color; }
	const veVec3& getColor() const { return _color; }
	void setIntensity(float intensity) { _intensity = intensity; }
	float getIntensity() const { return _intensity; }
	void setAttenuationRange(float range) { _attenuationRange = range; _attenuationRangeInverse = 1.0f / _attenuationRange; }
	float getAttenuationRange() const { return _attenuationRange; }
	float getAttenuationRangeInverse() const { return _attenuationRangeInverse; }

	void shadowEnable(bool isEnabled) { _shadowEnabled = isEnabled; _needUpdateShadowMap = true;};
	bool isShadowEnabled() const { return _shadowEnabled; };
	void setShadowResolution(const veVec2 &resolution);
	const veVec2& getShadowResolution() const { return _shadowResolution; }
	void setShadowBias(float bias) { _shadowBias = bias; }
	float getShadowBias() const { return _shadowBias; }
	void setShadowStrength(float strength) { _shadowStrength = strength; }
	float getShadowStrength() const { return _shadowStrength; }
	void setShadowArea(const veVec2 &area) { _shadowArea = area; }
	const veVec2& getShadowArea() const { return _shadowArea; }
	void setUseSoftShadow(bool use) { _isUseSoftShadow = use; }
	bool isUseSoftShadow() const { return _isUseSoftShadow; }
	void setShadowSoftness(float softness) { _shadowSoftness = softness; }
	float getShadowSoftness() const { return _shadowSoftness; }

protected:
	veLight(LightType type);

	virtual void refreshUpdate(veSceneManager *sm, const veMat4 &transform) override;
	virtual void updateSceneManager() override;
	veTexture* getShadowTexture() { return _shadowTexture.get(); }
	void setShadowTexture(veTexture *texture) { _shadowTexture = texture; }
	void setLightMatrix(const veMat4 &mat) { _lightMatrix = mat; }
	const veMat4& getLightMatrix() const { return _lightMatrix; }

protected:

	LightType _type;

	veVec3 _color;
	float  _intensity;
	float  _attenuationRange;
	float  _attenuationRangeInverse;
	veMat4 _lightInCamMatrix;

	bool _shadowEnabled;
	veVec2 _shadowResolution;
	veVec2 _shadowArea;
	float _shadowBias;
	float _shadowStrength;
	float _shadowSoftness;
	bool  _isUseSoftShadow;

	VE_Ptr<veTexture>  _shadowTexture;
	veMat4             _lightMatrix;
	bool               _needUpdateShadowMap;
};

class VE_EXPORT veDirectionalLight : public veLight
{
	friend class veSceneManager;
public:

	~veDirectionalLight();

protected:

	veDirectionalLight();
};

class VE_EXPORT vePointLight : public veLight
{
	friend class veSceneManager;
public:

	~vePointLight();

protected:

	vePointLight();

protected:
};

class VE_EXPORT veSpotLight : public veLight
{
	friend class veSceneManager;
public:

	~veSpotLight();

	void setInnerAngle(float innerAng) { _innerAngle = innerAng; _innerAngleCos = veMath::veCos(veMath::veRadian(_innerAngle)); }
	float getInnerAngle() { return _innerAngle; }
	float getInnerAngleCos() { return _innerAngleCos; }
	void setOuterAngle(float outerAng) { _outerAngle = outerAng; _outerAngleCos = veMath::veCos(veMath::veRadian(_outerAngle)); }
	float getOuterAngle() { return _outerAngle; }
	float getOuterAngleCos() { return _outerAngleCos; }

protected:

	veSpotLight();

protected:

	float  _innerAngle;
	float  _innerAngleCos;
	float  _outerAngle;
	float  _outerAngleCos;
};

typedef std::vector< VE_Ptr<veLight> > veLightList;
typedef std::map<veLight::LightType, veLightList> veLightListMap;

#endif