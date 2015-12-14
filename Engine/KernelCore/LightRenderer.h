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

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
};


class VE_EXPORT vePointLightRenderer : public veSphereRenderer
{
public:
	vePointLightRenderer();
	~vePointLightRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
	void setLightVolumeScale(const veMat4 &scale) { _lightVolumeScale = scale; }

private:
	veMat4 _lightVolumeScale;
};

class VE_EXPORT veSpotLightRenderer : public veConeRenderer
{
public:
	veSpotLightRenderer();
	~veSpotLightRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override;
};


#endif