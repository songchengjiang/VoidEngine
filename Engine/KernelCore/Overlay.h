#ifndef _VE_OVERLAY_
#define _VE_OVERLAY_
#include "Prerequisites.h"
#include "RenderableObject.h"
#include "BaseCore/Array.h"
#include "VE_Ptr.h"
#include "Texture.h"
#include "Shader.h"

class VE_EXPORT veOverlay : public veRenderableObject
{
public:
	veOverlay();
	veOverlay(veTexture *texture);
	~veOverlay();

	void setRenderOrder(int order);
	int getRenderOrder() const;

	void setAlphaThreshold(veReal threshold);
	veReal getAlphaThreshold() const;

protected:

	void initMaterial(veTexture *texture);

protected:

	VE_Ptr<veUniform> _alphaThreshold;
	VE_Ptr<veTexture> _texture;
};

#endif