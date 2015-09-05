#ifndef _VE_TEXTURE_
#define _VE_TEXTURE_
#include "Prerequisites.h"
#include "Image.h"
#include "VE_Ptr.h"

class VE_EXPORT veTexture
{
public:
	static const int DEFAULT_WIDTH;
	static const int DEFAULT_HEIGHT;
	static const int DEFAULT_DEPTH;
	static const int DEFAULT_INTERNAL_FORMAT;

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

	enum SourceType
	{
		IMAGE,
		FRAME_BUFFER_OBJECT,
	};

	virtual ~veTexture();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	virtual void bind(unsigned int textureUnit);

	void setWrapMode(WrapMode wrapMode) { _wrapMode = wrapMode; _needRefreshSampler = true; }
	WrapMode getWrapMode() const { return _wrapMode; }
	void setFilterMode(FilterMode filterMode){ _filterMode = filterMode; _needRefreshSampler = true; }
	FilterMode getFilterMode() const { return _filterMode; }
	void setSourceType(SourceType sourceType) { _sourceType = sourceType; }
	SourceType getSourceType() const { return _sourceType; }
	void setAttachedName(const std::string &name) { _attachedName = name; }
	const std::string& getAttachedName() const { return _attachedName; }
	void setAttachment(GLenum attachment) { _attachment = attachment; }
	GLenum getAttachment() const { return _attachment; }

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

	bool& autoWidth() { return _autoWidth; }
	bool& autoHeight() { return _autoHeight; }

	GLuint glTex();

protected:

	veTexture(veImage *image, GLenum target);

protected:

	VE_Ptr<veImage> _image;
	WrapMode        _wrapMode;
	FilterMode      _filterMode;
	SourceType      _sourceType;
	bool            _needRefreshTex;
	bool            _needRefreshSampler;
	GLuint          _texID;
	GLuint          _samplerID;
	GLenum          _target;
	GLenum          _attachment;
	std::string     _attachedName;

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

#endif