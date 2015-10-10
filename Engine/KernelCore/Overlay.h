#ifndef _VE_OVERLAY_
#define _VE_OVERLAY_
#include "Prerequisites.h"
#include "RenderableObject.h"
#include "BaseCore/Array.h"
#include "VE_Ptr.h"
#include "Image.h"
#include "Shader.h"

class VE_EXPORT veOverlay : public veRenderableObject
{
public:
	veOverlay();
	~veOverlay();

	void setImage(veImage *image);
	veImage* getImage();
	void setAlphaThreshold(veReal threshold);
	veReal getAlphaThreshold() const;

protected:

	void initMaterial();

protected:

	VE_Ptr<veUniform> _alphaThreshold;
	VE_Ptr<veTexture> _texture;
};

#endif