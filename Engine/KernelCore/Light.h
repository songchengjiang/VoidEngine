#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Node.h"
#include "Parameter.h"
#include "Camera.h"

class VE_EXPORT veLight : public veNode
{
	friend class veLightManager;
	friend class vePass;
public:

	static const veVec2 DEFAULT_SHADOW_RESOLUTION;
	static const veVec3 DEFAULT_SHADOW_RANGE;
	static const float  DEFAULT_SHADOW_BIAS;
	static const float  DEFAULT_SHADOW_STRENGTH;

	~veLight();

	virtual void update(veSceneManager *sm, const veMat4 &transform) override;
	virtual void visit(veNodeVisitor &visitor) override;
	//virtual void render(veCamera *camera) override;

	const std::string& getType() const { return _type; }
	const veParameterList& getParameters() const { return _parameters; }
	veParameter* getParameter(const std::string &name);

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
	veLight(const std::string &type, const veParameterList &params);

	virtual void updateSceneManager() override;

protected:

	std::string _type;
	veParameterList _parameters;
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