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

	struct DirectionalShadow
	{
		VE_Ptr<veTexture> texture;
		VE_Ptr<veCamera> camera;
	};

	struct OmnidirectionalShadow
	{
		VE_Ptr<veTexture> texture;
		VE_Ptr<veCamera> cameras[6];
	};

private:

	void directionalLightShadowing(veLight *light);
	void pointLightShadowing(veLight *light);
	void spotLightShadowing(veLight *light);

private:

	veSceneManager *_sceneManager;
	std::map<veLight*, DirectionalShadow>     _directionalShadowMapList;
	std::map<veLight*, OmnidirectionalShadow> _omnidirectionalShadowMapList;
};

#endif