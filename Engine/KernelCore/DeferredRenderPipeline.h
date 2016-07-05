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

	virtual void renderScene(veCamera *camera) override;

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

	struct CameraRenderParams {
		VE_Ptr<veFrameBufferObject>   FBO;
		VE_Ptr<veTexture>             DS;
		VE_Ptr<veTexture>             RT0;//normal/lightMask
		VE_Ptr<veTexture>             RT1;//diffuse/roughness
		VE_Ptr<veTexture>             RT2;//specular/fresnelFactor

		VE_Ptr<veFrameBufferObject>   fullScreenFBO;
		VE_Ptr<veSurface>             fullScreenSurface;
		VE_Ptr<veTexture>             fullScreenTexture;
	};

protected:

	CameraRenderParams& getCameraParams(veCamera *camera);

protected:

	VE_Ptr<veScreenLightRenderer> _directionalLightRenderer;
	VE_Ptr<veSphereLightRenderer> _pointLightRenderer;
	VE_Ptr<veConeLightRenderer>   _spotLightRenderer;

	VE_Ptr<veUniform>             _ambientColor;

	std::unordered_map< veLight*, VE_Ptr<veMaterial> > _lightRenderParamsList;
	std::unordered_map< veCamera*, CameraRenderParams > _cameraRenderParamsList;
};

#endif