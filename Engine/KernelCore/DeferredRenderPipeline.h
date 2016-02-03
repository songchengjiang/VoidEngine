#ifndef _VE_DEFERRED_RENDER_PIPELINE_
#define _VE_DEFERRED_RENDER_PIPELINE_
#include "RenderPipeline.h"
#include "FrameBufferObject.h"
#include "Texture.h"
#include "Surface.h"
#include "Image.h"
#include "LightRenderer.h"
class VE_EXPORT veDeferredRenderPipeline : public veRenderPipeline
{
public:
	veDeferredRenderPipeline(veSceneManager *sm);
	~veDeferredRenderPipeline();

protected:

	virtual void renderCamera(veCamera *camera, bool isMainCamera) override;

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
	void renderDirectionalLight(veLight *light, veCamera *camera);
	void renderPointLight(veLight *light, veCamera *camera);
	void renderSpotLight(veLight *light, veCamera *camera);

protected:

	VE_Ptr<veFrameBufferObject>   _FBO;
	VE_Ptr<veTexture>             _DS;
	VE_Ptr<veTexture>             _RT0;//normal/lightMask
	VE_Ptr<veTexture>             _RT1;//diffuse/roughness
	VE_Ptr<veTexture>             _RT2;//specular/fresnelFactor

	VE_Ptr<veScreenLightRenderer> _directionalLightRenderer;
	VE_Ptr<veSphereLightRenderer> _pointLightRenderer;
	VE_Ptr<veConeLightRenderer>   _spotLightRenderer;

	VE_Ptr<veFrameBufferObject>   _fullScreenFBO;
	VE_Ptr<veSurface>             _fullScreenSurface;
	VE_Ptr<veTexture>             _fullScreenTexture;
	VE_Ptr<veUniform>             _ambientColor;

	std::unordered_map< veLight*, VE_Ptr<veMaterial> > _lightRenderParamsList;
};

#endif