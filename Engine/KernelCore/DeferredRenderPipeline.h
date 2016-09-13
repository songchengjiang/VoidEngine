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

	virtual void renderScene(veCamera *camera, unsigned int contextID) override;
    virtual void renderToPostProcesser(vePostProcesser *processer, veCamera *camera, unsigned int contextID) override;

	void initLightingParams();
	void initLightCommomParams(veLight *light, vePass *pass);
	void updateLightCommomParams(veLight *light, vePass *pass, veCamera *camera);

    veMaterial* createAmbientLightMaterial();
	veMaterial* createDirectionalLightMaterial(veLight *light);
	veMaterial* createPointLightMaterial(veLight *light);
	veMaterial* createSpotLightMaterial(veLight *light);
    veMaterial* createIBLightMaterial(veLight *light);

	void renderLights(veCamera *camera, unsigned int contextID);
	void cullPointLight(veLight *light, veCamera *camera, unsigned int contextID);
	void cullSpotLight(veLight *light, veCamera *camera, unsigned int contextID);
    void renderAmbientLight(const veVec3 &ambient, veCamera *camera, unsigned int contextID);
	void renderDirectionalLight(veLight *light, veCamera *camera, unsigned int contextID);
	void renderPointLight(veLight *light, veCamera *camera, unsigned int contextID);
	void renderSpotLight(veLight *light, veCamera *camera, unsigned int contextID);
    void renderIBLight(veLight *light, veCamera *camera, unsigned int contextID);

protected:

	struct CameraRenderParams {
		VE_Ptr<veFrameBufferObject>   FBO;
		VE_Ptr<veTexture>             DS;
		VE_Ptr<veTexture>             RT0;//normal/lightMask
		VE_Ptr<veTexture>             RT1;//diffuse/roughness
		VE_Ptr<veTexture>             RT2;//specular/fresnelFactor
        
        VE_Ptr<veFrameBufferObject>   postProcesserFBO;
        VE_Ptr<veSurface>             postProcesserSurface;
        VE_Ptr<veTexture>             sceneColorTexture;
        VE_Ptr<veTexture>             sceneDepthTexture;
	};

protected:

	CameraRenderParams& getCameraParams(veCamera *camera);
    void initPostProcesserParams(veCamera *camera);

protected:

    VE_Ptr<veScreenLightRenderer> _ambientLightRenderer;
	VE_Ptr<veScreenLightRenderer> _directionalLightRenderer;
	VE_Ptr<veSphereLightRenderer> _pointLightRenderer;
	VE_Ptr<veConeLightRenderer>   _spotLightRenderer;
    VE_Ptr<veScreenLightRenderer> _iBLightRenderer;

    VE_Ptr<veMaterial>            _ambientLightMaterial;

	std::unordered_map< veLight*, VE_Ptr<veMaterial> > _lightRenderParamsList;
	std::unordered_map< veCamera*, CameraRenderParams > _cameraRenderParamsList;
};

#endif