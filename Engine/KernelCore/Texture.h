#ifndef _VE_TEXTURE_
#define _VE_TEXTURE_
#include "Prerequisites.h"
#include "VE_Ptr.h"

class veTextureManager;
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
		TEXTURE_CUBE,
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

	void storage(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat = GL_RGB, GLenum dataType = GL_UNSIGNED_BYTE, unsigned char *data = nullptr);

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	int getDepth() const { return _depth; }
	GLint getInternalFormat() const { return _internalFormat; }
	GLenum getPixelFormat() const { return _pixelFormat; }
	GLenum getDataType() const { return _dataType; }
	unsigned char* getData() { return _data; }
	unsigned int getDataSize() { return _dataSize; }
	virtual unsigned int getTextureTotalMemory();

	GLuint glTex();
	GLenum glTarget() { return _target; }


protected:

	veTexture(GLenum target);

	unsigned int perPixelSize();
	void releaseTextureData();

protected:

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
	GLint           _internalFormat;
	GLenum          _pixelFormat;
	GLenum          _dataType;
	unsigned char  *_data;
	unsigned int    _dataSize;
	unsigned int    _usage;
	bool            _isExchanged;

	veTextureManager *_manager;
};

class VE_EXPORT veTexture2D : public veTexture
{
	friend class veTextureManager;
public:

	~veTexture2D();

	virtual void bind(unsigned int textureUnit) override;

protected:
	veTexture2D();

};

class VE_EXPORT veTextureRECT : public veTexture
{
	friend class veTextureManager;
public:

	~veTextureRECT();

	virtual void bind(unsigned int textureUnit) override;

protected:
	veTextureRECT();

};


class VE_EXPORT veTextureCube : public veTexture
{
	friend class veTextureManager;
public:

	enum CubeMapTexType
	{
		CUBE_MAP_POSITIVE_X = 0,
		CUBE_MAP_NEGATIVE_X = 1,
		CUBE_MAP_POSITIVE_Y = 2,
		CUBE_MAP_NEGATIVE_Y = 3,
		CUBE_MAP_POSITIVE_Z = 4,
		CUBE_MAP_NEGATIVE_Z = 5,
	};

	~veTextureCube();

	virtual void bind(unsigned int textureUnit) override;

	void setTexture(CubeMapTexType texType, veTexture *texture);
	virtual unsigned int getTextureTotalMemory() override;

protected:
	veTextureCube();

	void storage(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat = GL_RGB, GLenum dataType = GL_UNSIGNED_BYTE, unsigned char *data = nullptr){}

protected:

	VE_Ptr<veTexture> _textures[6];
};

#endif