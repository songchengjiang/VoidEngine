#ifndef _VE_TEXTURE_
#define _VE_TEXTURE_
#include "Prerequisites.h"
#include "Image.h"
#include "VE_Ptr.h"

class VE_EXPORT veTexture
{
public:

	enum WrapMode{
		REPEAT = GL_REPEAT,
		MIRROR = GL_MIRRORED_REPEAT,
		CLAMP = GL_CLAMP_TO_EDGE,
		DECAL = GL_CLAMP,
	};

	enum FilterMode{
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIP_MAP = GL_NEAREST_MIPMAP_NEAREST,
		LINEAR_MIP_MAP = GL_LINEAR_MIPMAP_LINEAR,
	};

	virtual ~veTexture();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual void bind(unsigned int textureUnit);

	void setWrapMode(WrapMode wrapMode) { _wrapMode = wrapMode; _needRefreshSampler = true; }
	WrapMode getWrapMode() const { return _wrapMode; }
	void setFilterMode(FilterMode filterMode){ _filterMode = filterMode; _needRefreshSampler = true; }
	FilterMode getFilterMode() const { return _filterMode; }
	void setImage(veImage *image) { _image = image; _needRefreshTex = true; };
	veImage *getImage() { return _image.get(); }

	GLuint glTex();

protected:

	veTexture(veImage *image, GLenum target);

protected:

	VE_Ptr<veImage> _image;
	WrapMode        _wrapMode;
	FilterMode      _filterMode;
	bool            _needRefreshTex;
	bool            _needRefreshSampler;
	GLuint          _texID;
	GLuint          _samplerID;
	GLenum          _target;
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