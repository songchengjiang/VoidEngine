#ifndef _VE_DEFERRED_RENDER_PIPELINE_
#define _VE_DEFERRED_RENDER_PIPELINE_
#include "RenderPipeline.h"
#include "FrameBufferObject.h"
#include "Texture.h"
#include "Surface.h"
#include "LightRenderer.h"

class VE_EXPORT veDeferredRenderPipeline : public veRenderPipeline
{
public:
	veDeferredRenderPipeline(veSceneManager *sm);
	~veDeferredRenderPipeline();

protected:

	virtual void renderCamera(veCamera *camera) override;

	void initDeferredParams();
	void initLightingParams();
	void initLightCommomParams(veLight *light, vePass *pass);
	void updateLightCommomParams(veLight *light, vePass *pass, veCamera *camera);

	veMaterial* createDirectionalLightMaterial(veLight *light);
	veMaterial* createPointLightMaterial(veLight *light);
	veMaterial* createSpotLightMaterial(veLight *light);

	void renderLights(veCamera *camera);
	void cullPointLight(veLight *light, veCamera *camera);
	void cullSpotLight(veLight *light, veCamera *camera);
	void renderAmbientLight(veCamera *camera);
	void renderDirectionalLight(veLight *light, veCamera *camera);
	void renderPointLight(veLight *light, veCamera *camera);
	void renderSpotLight(veLight *light, veCamera *camera);

protected:

	VE_Ptr<veFrameBufferObject> _FBO;
	VE_Ptr<veTexture>           _DS;
	VE_Ptr<veTexture>           _RT0;//normalAndOpacity
	VE_Ptr<veTexture>           _RT1;//diffuseAndLightMask
	VE_Ptr<veTexture>           _RT2;//specularAndRoughness

	VE_Ptr<vePass>                    _ambientLightRenderPass;
	VE_Ptr<veScreenLightRenderer>     _ambientLightRender;
	VE_Ptr<veScreenLightRenderer>     _directionalLightRenderer;
	VE_Ptr<veSphereLightRenderer>     _pointLightRenderer;
	VE_Ptr<veConeLightRenderer>       _spotLightRenderer;

	std::unordered_map< veLight*, VE_Ptr<veMaterial> > _lightRenderParamsList;
};

#endif