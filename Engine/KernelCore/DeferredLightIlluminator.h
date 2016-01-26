#ifndef _VE_DEFFERED_LIGHT_ILLUMINATOR_
#define _VE_DEFFERED_LIGHT_ILLUMINATOR_
#include "Prerequisites.h"
#include "Material.h"
#include "Texture.h"
#include "LightRenderer.h"

class veCamera;
class VE_EXPORT veDeferredLightSceneIlluminator
{
public:
	veDeferredLightSceneIlluminator(veCamera *camera);
	~veDeferredLightSceneIlluminator();

	USE_VE_PTR;

	void illuminate();
	veTexture *getIlluminationTexture() { return _illuminationTexture.get(); }
	void resize(const veVec2 &size);

private:

	void initLightMaterials();
	void initIlluminationParams();
	void initPassCommanParams(vePass *pass);

	void fillCommonLightParamsToPass(veLight *light, vePass *pass);
	void directionalLightIlluminate(veLight *light);
	void pointLightCulling(veLight *light);
	void pointLightIlluminate(veLight *light);
	void spotLightCulling(veLight *light);
	void spotLightIlluminate(veLight *light);

private:

	veCamera *_camera;
	VE_Ptr<veMaterialArray> _lightMatrials;
	veFrameBufferObject *_illuminationFBO;
	VE_Ptr<veTexture>    _sceneDepthTexture;
	VE_Ptr<veTexture>    _sceneNormalTexture;
	VE_Ptr<veTexture>    _illuminationTexture;
	veVec2               _size;

	VE_Ptr<veScreenLightRenderer> _directionalLightRenderer;
	VE_Ptr<veSphereLightRenderer>       _pointLightRenderer;
	VE_Ptr<veConeLightRenderer>        _spotLightRenderer;
};

#endif