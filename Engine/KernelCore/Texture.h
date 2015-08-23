#ifndef _VE_TEXTURE_
#define _VE_TEXTURE_
#include "Prerequisites.h"
#include "Image.h"
#include "VE_Ptr.h"

class VE_EXPORT veTexture
{
public:

	enum WrapMode{
		REPEAT,
		MIRROR,
		CLAMP,
		DECAL,
	};

	enum FilterMode{
		NEAREST,
		LINEAR,
		NEAREST_MIP_MAP,
		LINEAR_MIP_MAP,
	};

	virtual ~veTexture();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual void bind(unsigned int textureUnit);

	void setWrapMode(WrapMode wrapMode) { _wrapMode = wrapMode; }
	WrapMode getWrapMode() const { return _wrapMode; }
	void setFilterMode(FilterMode filterMode){ _filterMode = filterMode; }
	FilterMode getFilterMode() const { return _filterMode; }

protected:

	veTexture(veImage *image);

protected:

	VE_Ptr<veImage> _image;
	WrapMode _wrapMode;
	FilterMode _filterMode;
};

class VE_EXPORT veTexture2D : public veTexture
{
public:

	veTexture2D(veImage *image = nullptr);
	~veTexture2D();

	virtual void bind(unsigned int textureUnit) override;

protected:

};

#endif