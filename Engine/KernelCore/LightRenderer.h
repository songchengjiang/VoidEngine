#ifndef _VE_LIGHT_RENDERER_
#define _VE_LIGHT_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"
#include "SurfaceRenderer.h"
#include "SphereRenderer.h"
#include "ConeRenderer.h"

class VE_EXPORT veScreenLightRenderer : public veSurfaceRenderer
{
public:
	veScreenLightRenderer();
	~veScreenLightRenderer();

	void render(veNode *node, vePass *pass, veCamera *camera);
};


class VE_EXPORT veSphereLightRenderer : public veSphereRenderer
{
public:
	veSphereLightRenderer();
	~veSphereLightRenderer();

	void render(veNode *node, vePass *pass, veCamera *camera);
	void setLightVolumeScale(const veMat4 &scale) { _lightVolumeScale = scale; }

private:
	veMat4 _lightVolumeScale;
};

class VE_EXPORT veConeLightRenderer : public veConeRenderer
{
public:
	veConeLightRenderer();
	~veConeLightRenderer();

	void render(veNode *node, vePass *pass, veCamera *camera);
	void setLightVolumeScale(const veMat4 &scale) { _lightVolumeScale = scale; }

private:
	veMat4 _lightVolumeScale;
};

#endif