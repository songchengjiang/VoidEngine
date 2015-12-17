#ifndef _VE_LIGHT_RENDERER_
#define _VE_LIGHT_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"
#include "SurfaceRenderer.h"
#include "SphereRenderer.h"
#include "ConeRenderer.h"

class VE_EXPORT veDirectionalLightRenderer : public veSurfaceRenderer
{
public:
	veDirectionalLightRenderer();
	~veDirectionalLightRenderer();

	void immediatelyRender(veNode *node, vePass *pass, veCamera *camera);
};


class VE_EXPORT vePointLightRenderer : public veSphereRenderer
{
public:
	vePointLightRenderer();
	~vePointLightRenderer();

	void immediatelyRender(veNode *node, vePass *pass, veCamera *camera);
	void setLightVolumeScale(const veMat4 &scale) { _lightVolumeScale = scale; }

private:
	veMat4 _lightVolumeScale;
};

class VE_EXPORT veSpotLightRenderer : public veConeRenderer
{
public:
	veSpotLightRenderer();
	~veSpotLightRenderer();

	void immediatelyRender(veNode *node, vePass *pass, veCamera *camera);
	void setLightVolumeScale(const veMat4 &scale) { _lightVolumeScale = scale; }

private:
	veMat4 _lightVolumeScale;
};

#endif