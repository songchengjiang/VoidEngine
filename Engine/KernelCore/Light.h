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

	static const unsigned int DEFUALT_LIGHT_PARAM_NUM;
	static const std::string DEFUALT_LIGHT_UNIFORM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_MAX_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_NUM_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_TYPE_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_POSITION_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_COLOR_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME;
	static const std::string DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME;

	static const veVec2 DEFAULT_SHADOW_RESOLUTION;
	static const veVec3 DEFAULT_SHADOW_RANGE;
	static const float  DEFAULT_SHADOW_BIAS;
	static const float  DEFAULT_SHADOW_STRENGTH;

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

	void setLightType(LightType type) { _type = type; }
	LightType getLightType() const { return _type; }
	void setColor(const veVec4 &color) { _color = color; }
	const veVec4& getColor() const { return _color; }
	void setIntensity(float intensity) { _intensity = intensity; }
	float getIntensity() const { return _intensity; }
	void setAttenuationRange(float range) { _attenuationRange = range; _attenuationRangeInverse = 1.0f / _attenuationRange; }
	float getAttenuationRange() const { return _attenuationRange; }
	float getAttenuationRangeInverse() const { return _attenuationRangeInverse; }
	void setInnerAngle(float innerAng) { _innerAngle = innerAng; _innerAngleCos = veMath::veCos(veMath::veRadian(_innerAngle)); }
	float getInnerAngle() { return _innerAngle; }
	float getInnerAngleCos() { return _innerAngleCos; }
	void setOuterAngle(float outerAng) { _outerAngle = outerAng; _outerAngleCos = veMath::veCos(veMath::veRadian(_outerAngle)); }
	float getOuterAngle() { return _outerAngle; }
	float getOuterAngleCos() { return _outerAngleCos; }


	void shadowEnable(bool isEnabled) { _shadowEnabled = isEnabled; }
	bool isShadowEnabled() const { return _shadowEnabled; };
	void setShadowResolution(const veVec2 &resolution) { _shadowResolution = resolution; _needRefreshShadow = true; }
	const veVec2& getShadowResolution() const { return _shadowResolution; }
	void setShadowBias(float bias) { _shadowBias = bias; }
	float getShadowBias() const { return _shadowBias; }
	void setShadowStrength(float strength) { _shadowStrength = strength; }
	float getShadowStrength() const { return _shadowStrength; }
	void setShadowRange(const veVec3 &range) { _shadowRange = range; _needRefreshShadow = true; }
	const veVec3 getShadowRange() const { return _shadowRange; }

	veCamera* getShadowRenderingCamera() { return _shadowRenderingCam.get(); }
	const veCamera* getShadowRenderingCamera() const { return _shadowRenderingCam.get(); }
	veTexture* getShadowTexture() { return _shadowTexture.get(); }
	const veTexture* getShadowTexture() const { return _shadowTexture.get(); }

	void setLightViewMatrix(const veMat4 &mat) { _lightMatrix = mat; }
	const veMat4& getLightViewMatrix() { return _lightMatrix; }

protected:
	veLight(LightType type);

	virtual void updateSceneManager() override;

protected:

	LightType _type;

	veVec4 _color;
	float  _intensity;
	float  _attenuationRange;
	float  _attenuationRangeInverse;
	float  _innerAngle;
	float  _innerAngleCos;
	float  _outerAngle;
	float  _outerAngleCos;
	veMat4 _lightMatrix;

	bool _shadowEnabled;
	bool _perspectiveShadow;
	veVec2 _shadowResolution;
	veVec3 _shadowRange;
	float _shadowBias;
	float _shadowStrength;
	VE_Ptr<veCamera>  _shadowRenderingCam;
	VE_Ptr<veTexture> _shadowTexture;
	bool _needRefreshShadow;
};

typedef std::vector< VE_Ptr<veLight> > veLightList;

#endif