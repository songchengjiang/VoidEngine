#ifndef _VE_TEXTURE_
#define _VE_TEXTURE_
#include "Prerequisites.h"
#include "Image.h"
#include "VE_Ptr.h"

class VE_EXPORT veTexture
{
	friend class veTextureManager;
public:
	static const int DEFAULT_WIDTH;
	static const int DEFAULT_HEIGHT;
	static const int DEFAULT_DEPTH;
	static const int DEFAULT_INTERNAL_FORMAT;

	enum TextureType
	{
		TEXTURE_2D,
		TEXTURE_3D,
		TEXTURE_RECT,
	};

	enum WrapMode{
		REPEAT = GL_REPEAT,
		MIRROR = GL_MIRRORED_REPEAT,
		CLAMP = GL_CLAMP_TO_EDGE,
		DECAL = GL_CLAMP_TO_BORDER,
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

	void setImage(veImage *image);
	veImage *getImage() { return _image.get(); }
	void storage(GLint internalFormat, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT, int depth = DEFAULT_DEPTH);

	void setWidth(int width) { if (width == _width) return;  _width = width; _needRefreshTex = true; }
	int getWidth() const { return _width; }
	void setHeight(int height) { if (_height == height) return; _height = height; _needRefreshTex = true; }
	int getHeight() const { return _height; }
	void setDepth(int depth) { if (_depth == depth) return; _depth = depth; _needRefreshTex = true; }
	int getDepth() const { return _depth; }
	void setInternalFormat(GLint interFormat) { if (_internalFormat == interFormat) return; _internalFormat = interFormat; _needRefreshTex = true; }
	GLint getInternalFormat() const { return _internalFormat; }

	GLuint glTex();
	GLenum glTarget() { return _target; }

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
	TextureType     _type;

	int             _width;
	int             _height;
	int             _depth;
	bool            _autoWidth;
	bool            _autoHeight;
	GLint           _internalFormat;
};

class VE_EXPORT veTexture2D : public veTexture
{
public:

	veTexture2D(veImage *image = nullptr);
	~veTexture2D();

	virtual void bind(unsigned int textureUnit) override;

protected:

};

class VE_EXPORT veTextureRECT : public veTexture
{
public:

	veTextureRECT(veImage *image = nullptr);
	~veTextureRECT();

	virtual void bind(unsigned int textureUnit) override;

protected:

};

#endif