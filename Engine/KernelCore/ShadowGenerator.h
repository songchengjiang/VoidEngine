#ifndef _VE_SHADOW_GENERATOR_
#define _VE_SHADOW_GENERATOR_
#include "Prerequisites.h"
#include "Camera.h"

class veSceneManager;
class veLight;
class veShadowGenerator
{
public:
	veShadowGenerator(veSceneManager * sm);
	~veShadowGenerator();

	USE_VE_PTR;

	void shadowing();

private:

	void directionalLightShadowing(veLight *light);
	void pointLightShadowing(veLight *light);
	void spotLightShadowing(veLight *light);

private:

	veSceneManager *_sceneManager;
	VE_Ptr<veCamera> _shadowCamera;
};

#endif