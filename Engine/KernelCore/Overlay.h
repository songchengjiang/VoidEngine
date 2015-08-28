#ifndef _VE_OVERLAY_
#define _VE_OVERLAY_
#include "Prerequisites.h"
#include "RenderableObject.h"
#include "BaseCore/Array.h"
#include "VE_Ptr.h"
#include "Image.h"

class VE_EXPORT veOverlay : public veRenderableObject
{
public:
	veOverlay();
	~veOverlay();

	void setImage(veImage *image);
	veImage* getImage();

private:

	void createMaterial();
};

#endif