#include "Texture.h"
#include "TextureManager.h"
#include "FileCore/File.h"
#include "BaseCore/MathAlgorithm.h"
const int veTexture::DEFAULT_WIDTH = 0;
const int veTexture::DEFAULT_HEIGHT = 0;
const int veTexture::DEFAULT_DEPTH = 0;
const int veTexture::DEFAULT_INTERNAL_FORMAT = GL_RGBA32F;

veTexture::~veTexture()
{
	VE_SAFE_DELETE_ARRAY(_data);
	releaseTextureData();
	releaseMipmapData();
}

veTexture::veTexture(GLenum target)
	: USE_VE_PTR_INIT
	, _wrapMode(REPEAT)
	, _filterMode(NEAREST)
	, _needRefreshTex(true)
	, _needRefreshSampler(true)
	, _texID(0)
	, _target(target)
	, _width(DEFAULT_WIDTH)
	, _height(DEFAULT_HEIGHT)
	, _depth(DEFAULT_DEPTH)
	, _dataType(GL_UNSIGNED_BYTE)
	, _internalFormat(GL_RGBA8)
	, _pixelFormat(GL_RGBA)
	, _dataSize(0)
	, _isCompressedTex(false)
	, _usage(0)
	, _data(nullptr)
	, _manager(nullptr)
	, _isExchanged(false)
{
	_swizzleMode[0] = SWIZZLE_R;
	_swizzleMode[1] = SWIZZLE_G;
	_swizzleMode[2] = SWIZZLE_B;
	_swizzleMode[3] = SWIZZLE_A;
}

unsigned int veTexture::perPixelSize()
{
	switch (_internalFormat)
	{
	case GL_ALPHA:
	case GL_DEPTH_COMPONENT:
	case GL_R8:
	case GL_LUMINANCE:
		return 1;

	case GL_R16F:
	case GL_DEPTH_COMPONENT16:
	case GL_RG8:
		//case GL_LUMINANCE16:
		return 2;

	case GL_DEPTH_COMPONENT32F:
		return 4;

	case GL_RGB:
	case GL_RGB8:
		return 3;

	case GL_RGBA:
	case GL_RGBA8:
		return 4;

	//case GL_RGB16:
	case GL_RGB16F:
		return 6;

	//case GL_RGBA16:
	case GL_RGBA16F:
	case GL_COMPRESSED_R11_EAC:
	case GL_COMPRESSED_SIGNED_R11_EAC:
	case GL_COMPRESSED_RGB8_ETC2:
	case GL_COMPRESSED_SRGB8_ETC2:
	case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
	case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		return 8;

	case GL_RGB32F:
		return 12;

	case GL_RGBA32F:
	case GL_COMPRESSED_RG11_EAC:
	case GL_COMPRESSED_SIGNED_RG11_EAC:
	case GL_COMPRESSED_RGBA8_ETC2_EAC:
	case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
		return 16;
	}
	return 0;
}

unsigned int veTexture::getImageSize(int width, int height)
{
	return ((width + 3) / 4) * ((height + 3) / 4) * perPixelSize();
}

void veTexture::releaseTextureData()
{
	if (_texID) {
		glDeleteTextures(1, &_texID);
		_manager->releaseTextureMemory(this);
		_texID = 0;
		_needRefreshTex = true;
	}
}

void veTexture::releaseMipmapData()
{
	if (_mipmapLevels.empty())
		return;
	for (auto &level : _mipmapLevels) {
		VE_SAFE_DELETE_ARRAY(level.data);
	}
	_mipmapLevels.clear();
}

bool veTexture::isCompressedTex(GLint internalformat)
{
	switch (internalformat)
	{
	case GL_COMPRESSED_R11_EAC:
	case GL_COMPRESSED_SIGNED_R11_EAC:
	case GL_COMPRESSED_RGB8_ETC2:
	case GL_COMPRESSED_SRGB8_ETC2:
	case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
	case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
	case GL_COMPRESSED_RG11_EAC:
	case GL_COMPRESSED_SIGNED_RG11_EAC:
	case GL_COMPRESSED_RGBA8_ETC2_EAC:
	case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
		return true;
	default:
		return false;
	}

	return false;
}

bool veTexture::isSupportFormat(GLenum pixelFormat)
{
	switch (pixelFormat)
	{
	case GL_RED:
	case GL_RED_INTEGER:
	case GL_RG:
	case GL_RGB:
	case GL_RGB_INTEGER:
	case GL_RGBA:
	case GL_RGBA_INTEGER:
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_STENCIL:
	case GL_LUMINANCE_ALPHA:
	case GL_LUMINANCE:
	case GL_ALPHA:
		return true;
	default:
		return false;
	}

	return false;
}

unsigned int veTexture::getTextureTotalMemory()
{
	return _dataSize;
}

void veTexture::bind(unsigned int textureUnit)
{
	if (_needRefreshTex) {
		releaseTextureData();
	}
	if (!_texID) {
        glGenTextures(1, &_texID);
		//glCreateTextures(_target, 1, &_texID);
	}
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(_target, _texID);

	if (_needRefreshSampler) {
		glTextureParameteri(_texID, GL_TEXTURE_MIN_FILTER, _filterMode);
		glTextureParameteri(_texID, GL_TEXTURE_MAG_FILTER, (_filterMode == NEAREST || _filterMode == NEAREST_MIP_MAP) ? GL_NEAREST : GL_LINEAR);

		glTextureParameteri(_texID, GL_TEXTURE_WRAP_S, _wrapMode);
		glTextureParameteri(_texID, GL_TEXTURE_WRAP_T, _wrapMode);
		glTextureParameteri(_texID, GL_TEXTURE_WRAP_R, _wrapMode);

		glTextureParameteri(_texID, GL_TEXTURE_SWIZZLE_R, _swizzleMode[0]);
		glTextureParameteri(_texID, GL_TEXTURE_SWIZZLE_G, _swizzleMode[1]);
		glTextureParameteri(_texID, GL_TEXTURE_SWIZZLE_B, _swizzleMode[2]);
		glTextureParameteri(_texID, GL_TEXTURE_SWIZZLE_A, _swizzleMode[3]);

		_needRefreshSampler = false;
	}

	_usage |= 1;
}

void veTexture::setSwizzleMode(SwizzleMode r, SwizzleMode g, SwizzleMode b, SwizzleMode a)
{
	_swizzleMode[0] = r;
	_swizzleMode[1] = g;
	_swizzleMode[2] = b;
	_swizzleMode[3] = a;
	_needRefreshSampler = true;
}

void veTexture::getSwizzleMode(SwizzleMode &r, SwizzleMode &g, SwizzleMode &b, SwizzleMode &a)
{
	r = _swizzleMode[0];
	g = _swizzleMode[1];
	b = _swizzleMode[2];
	a = _swizzleMode[3];
}

void veTexture::storage(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat, GLenum dataType
	, unsigned char *data)
{
	if (!isSupportFormat(pixelFormat))
		return;
	if (_width == width && _height == height && _depth == depth
		&& _internalFormat == internalFormat && _pixelFormat == pixelFormat && _dataType == dataType
		&& _data == data)
		return;

	_width = width;
	_height = height;
	_depth = depth;
	_internalFormat = internalFormat;
	_pixelFormat = pixelFormat;
	_dataType = dataType;
	_isCompressedTex = isCompressedTex(internalFormat);

	unsigned int pixelSize = perPixelSize();
	_dataSize = _width * _height * _depth * pixelSize;
	VE_SAFE_DELETE_ARRAY(_data);
	releaseMipmapData();
	if (data) {
		_data = new unsigned char[_dataSize];
		memcpy(_data, data, _dataSize);
		//if (_manager->requestTextureMemory(getTextureTotalMemory())) {
		//	_data = new unsigned char[_dataSize];
		//	memcpy(_data, data, _dataSize);
		//	_manager->assignTextureMemory(getTextureTotalMemory());
		//}
	}
	_needRefreshTex = true;
}

void veTexture::storage(const MipmapLevels &mipmaps, GLint internalFormat, GLenum pixelFormat, GLenum dataType)
{
	if (!isSupportFormat(pixelFormat))
		return;

	if (mipmaps.empty()) 
		return;

	if (_internalFormat == internalFormat && _pixelFormat == pixelFormat && _dataType == dataType)
		return;

	_width = mipmaps[0].width;
	_height = mipmaps[0].height;
	_depth = mipmaps[0].depth;
	_internalFormat = internalFormat;
	_pixelFormat = pixelFormat;
	_dataType = dataType;
	_isCompressedTex = isCompressedTex(internalFormat);

	VE_SAFE_DELETE_ARRAY(_data);
	releaseMipmapData();
	_dataSize = 0;
	for (auto &level : mipmaps) {
		MipmapLevel mipmap;
		mipmap.width = level.width;
		mipmap.height = level.height;
		mipmap.depth = level.depth;
		mipmap.data = new unsigned char[level.dataSize];
		mipmap.dataSize = level.dataSize;
		memcpy(mipmap.data, level.data, level.dataSize);
		_dataSize += level.dataSize;
		_mipmapLevels.push_back(mipmap);
	}

	_needRefreshTex = true;
}

GLuint veTexture::glTex()
{
	return _texID;
}

veTexture2D::veTexture2D()
	: veTexture(GL_TEXTURE_2D)
{
	_type = veTexture::TEXTURE_2D;
}

veTexture2D::~veTexture2D()
{
}

void veTexture2D::bind(unsigned int textureUnit)
{
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		if (_manager->exchangeTextureMemory(this)) {
			glTexStorage2D(_target, _mipmapLevels.empty()? 1: _mipmapLevels.size(), _internalFormat, _width, _height);

			if (_isCompressedTex) {
				if (_data) {
					unsigned int imageSize = getImageSize(_width, _height);
					glCompressedTexSubImage2D(_target, 0, 0, 0, _width, _height, _internalFormat, imageSize, _data);
				}
				else if (!_mipmapLevels.empty()) {
					for (size_t lev = 0; lev < _mipmapLevels.size(); ++lev) {
						unsigned int imageSize = getImageSize(_mipmapLevels[lev].width, _mipmapLevels[lev].height);
						glCompressedTexSubImage2D(_target, lev, 0, 0, _mipmapLevels[lev].width, _mipmapLevels[lev].height, _internalFormat, imageSize, _mipmapLevels[lev].data);
					} 
				}
			}
			else {
				if (_data) {
					glTexSubImage2D(_target, 0, 0, 0, _width, _height, _pixelFormat, _dataType, _data);
					glGenerateMipmap(_target);
				}
				else if (!_mipmapLevels.empty()) {
					for (size_t lev = 0; lev < _mipmapLevels.size(); ++lev) {
						glTexSubImage2D(_target, lev, 0, 0, _mipmapLevels[lev].width, _mipmapLevels[lev].height, _pixelFormat, _dataType, _mipmapLevels[lev].data);
					}
				}
			}
			_needRefreshTex = false;
		}
	}
}

veTexture3D::veTexture3D()
	: veTexture(GL_TEXTURE_3D)
{
	_type = veTexture::TEXTURE_3D;
}

veTexture3D::~veTexture3D()
{

}

void veTexture3D::bind(unsigned int textureUnit)
{
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		if (_manager->exchangeTextureMemory(this)) {
			glTexStorage3D(_target, _mipmapLevels.empty() ? 1 : _mipmapLevels.size(), _internalFormat, _width, _height, _depth);

			if (_isCompressedTex) {
				if (_data) {
					unsigned int imageSize = getImageSize(_width, _height);
					glCompressedTexSubImage3D(_target, 0, 0, 0, 0, _width, _height, _depth, _internalFormat, imageSize, _data);
				}
				else if (!_mipmapLevels.empty()) {
					for (size_t lev = 0; lev < _mipmapLevels.size(); ++lev) {
						unsigned int imageSize = getImageSize(_mipmapLevels[lev].width, _mipmapLevels[lev].height);
						glCompressedTexSubImage3D(_target, lev, 0, 0, 0, _mipmapLevels[lev].width, _mipmapLevels[lev].height, _mipmapLevels[lev].depth, _internalFormat, imageSize, _mipmapLevels[lev].data);
					}
				}
			}
			else {
				if (_data) {
					glTexSubImage3D(_target, 0, 0, 0, 0, _width, _height, _depth, _pixelFormat, _dataType, _data);
					glGenerateMipmap(_target);
				}
				else if (!_mipmapLevels.empty()) {
					for (size_t lev = 0; lev < _mipmapLevels.size(); ++lev) {
						glTexSubImage3D(_target, lev, 0, 0, 0, _mipmapLevels[lev].width, _mipmapLevels[lev].height, _mipmapLevels[lev].depth, _pixelFormat, _dataType, _mipmapLevels[lev].data);
					}
				}
			}
			_needRefreshTex = false;
		}
	}
}


veTextureCube::veTextureCube()
	: veTexture(GL_TEXTURE_CUBE_MAP)
{
	_type = veTexture::TEXTURE_CUBE;
}

unsigned int veTextureCube::getTextureTotalMemory()
{
	unsigned int totalMemory = 0;
	for (unsigned int i = 0; i < 6; ++i) {
		totalMemory += _textures[i]->getTextureTotalMemory();
	}
	return totalMemory;
}

veTextureCube::~veTextureCube()
{

}

void veTextureCube::bind(unsigned int textureUnit)
{
	if (_needRefreshSampler) {
		_textures[0]->getSwizzleMode(_swizzleMode[0], _swizzleMode[1], _swizzleMode[2], _swizzleMode[3]);
	}
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		if (_manager->exchangeTextureMemory(this)) {
			glTexStorage2D(_target, _textures[0]->getMipmapLevels().empty()? 1: _textures[0]->getMipmapLevels().size(), _textures[0]->getInternalFormat(), _textures[0]->getWidth(), _textures[0]->getHeight());
			bool needGenerateMipmap = false;
			for (unsigned int i = 0; i < 6; ++i) {
				auto &tex = _textures[i];

				if (_isCompressedTex) {
					if (tex->getData()) {
						unsigned int imageSize = tex->getImageSize(tex->getWidth(), tex->getHeight());
						glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, tex->getWidth(), tex->getHeight(), tex->getInternalFormat(), imageSize, tex->getData());
					}
					else if (!tex->getMipmapLevels().empty()) {
						const auto &mipmaps = tex->getMipmapLevels();
						for (size_t lev = 0; lev < mipmaps.size(); ++lev) {
							unsigned int imageSize = tex->getImageSize(mipmaps[lev].width, mipmaps[lev].height);
							glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lev, 0, 0, mipmaps[lev].width, mipmaps[lev].height, tex->getInternalFormat(), imageSize, mipmaps[lev].data);
						}
					}
				}
				else {
					if (tex->getData()) {
						glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, tex->getWidth(), tex->getHeight(), tex->getPixelFormat(), tex->getDataType(), tex->getData());
						needGenerateMipmap = true;
					}
					else if (!tex->getMipmapLevels().empty()) {
						const auto &mipmaps = tex->getMipmapLevels();
						for (size_t lev = 0; lev < mipmaps.size(); ++lev) {
							glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lev, 0, 0, mipmaps[lev].width, mipmaps[lev].height, tex->getPixelFormat(), tex->getDataType(), mipmaps[lev].data);
						}
					}
				}
			}
			if (needGenerateMipmap)
				glGenerateMipmap(_target);

			_needRefreshTex = false;
		}
	}
}

void veTextureCube::setTexture(CubeMapTexType texType, veTexture *texture)
{
	_textures[texType] = texture;
	_needRefreshTex = true;
}