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

	void render(const veMat4 &transform, vePass *pass, veCamera *camera, unsigned int contextID);
};


class VE_EXPORT veSphereLightRenderer : public veSphereRenderer
{
public:
	veSphereLightRenderer();
	~veSphereLightRenderer();

	void render(const veMat4 &transform, vePass *pass, veCamera *camera, unsigned int contextID);
};

class VE_EXPORT veConeLightRenderer : public veConeRenderer
{
public:
	veConeLightRenderer();
	~veConeLightRenderer();

	void render(const veMat4 &transform, vePass *pass, veCamera *camera, unsigned int contextID);
};

#endif