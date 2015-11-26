#ifndef _VE_TEXTURE_
#define _VE_TEXTURE_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include <functional>

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
		//TEXTURE_RECT,
		TEXTURE_CUBE,
	};

	enum WrapMode{
		REPEAT = GL_REPEAT,
		MIRROR = GL_MIRRORED_REPEAT,
		CLAMP = GL_CLAMP_TO_EDGE,
		DECAL = GL_CLAMP_TO_EDGE,
	};

	enum FilterMode{
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIP_MAP = GL_NEAREST_MIPMAP_NEAREST,
		LINEAR_MIP_MAP = GL_LINEAR_MIPMAP_LINEAR,
	};

	enum SwizzleMode
	{
		SWIZZLE_R = GL_RED,
		SWIZZLE_G = GL_GREEN,
		SWIZZLE_B = GL_BLUE,
		SWIZZLE_A = GL_ALPHA,
	};

	struct MipmapLevel
	{
		int width;
		int height;
		int depth;
		unsigned char  *data;
		unsigned int    dataSize;
	};
	typedef std::vector<MipmapLevel> MipmapLevels;

	virtual ~veTexture();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	TextureType getType() const { return _type; }

	virtual void bind(unsigned int textureUnit);

	void setWrapMode(WrapMode wrapMode) { _wrapMode = wrapMode; _needRefreshSampler = true; }
	WrapMode getWrapMode() const { return _wrapMode; }
	void setFilterMode(FilterMode filterMode){ _filterMode = filterMode; _needRefreshSampler = true; }
	FilterMode getFilterMode() const { return _filterMode; }
	void setSwizzleMode(SwizzleMode r, SwizzleMode g, SwizzleMode b, SwizzleMode a);
	void getSwizzleMode(SwizzleMode &r, SwizzleMode &g, SwizzleMode &b, SwizzleMode &a);

	virtual void storage(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat = GL_RGB, GLenum dataType = GL_UNSIGNED_BYTE
		, const unsigned char *data = nullptr);

	virtual void storage(const MipmapLevels &mipmaps, GLint internalFormat, GLenum pixelFormat = GL_RGB, GLenum dataType = GL_UNSIGNED_BYTE);

	int getWidth() const { return _width; }
	int getHeight() const { return _height; }
	int getDepth() const { return _depth; }
	GLint getInternalFormat() const { return _internalFormat; }
	GLenum getPixelFormat() const { return _pixelFormat; }
	GLenum getDataType() const { return _dataType; }
	const unsigned char* getData() const { return _data; }
	unsigned int getDataSize() const { return _dataSize; }
	virtual unsigned int getTextureTotalMemory();
	const MipmapLevels& getMipmapLevels() const { return _mipmapLevels; }
	unsigned int getImageSize(int width, int height) const;

	GLuint glTex();
	GLenum glTarget() { return _target; }

	static bool isCompressedTex(GLint internalformat);
	static bool isSupportFormat(GLenum pixelFormat);

protected:

	veTexture(GLenum target);

	unsigned int perPixelSize() const;
	void releaseTextureData();
	void releaseMipmapData();

protected:

	WrapMode        _wrapMode;
	FilterMode      _filterMode;
	SwizzleMode     _swizzleMode[4];
	bool            _needRefreshTex;
	bool            _needRefreshSampler;
	GLuint          _texID;
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
	bool            _isCompressedTex;
	bool            _isExchanged;

	MipmapLevels    _mipmapLevels;

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

class VE_EXPORT veTexture3D : public veTexture
{
	friend class veTextureManager;
public:

	~veTexture3D();

	virtual void bind(unsigned int textureUnit) override;

protected:
	veTexture3D();

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
	veTexture* getTexture(CubeMapTexType texType);
	virtual unsigned int getTextureTotalMemory() override;

protected:
	veTextureCube();

	virtual void storage(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat = GL_RGB, GLenum dataType = GL_UNSIGNED_BYTE
		, const unsigned char *data = nullptr) override {};
	virtual void storage(const MipmapLevels &mipmaps, GLint internalFormat, GLenum pixelFormat = GL_RGB, GLenum dataType = GL_UNSIGNED_BYTE) override {};

protected:

	VE_Ptr<veTexture> _textures[6];
};

#endif