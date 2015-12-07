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
	, _wrapMode(CLAMP)
	, _filterMode(NEAREST_MIP_MAP_LINEAR)
	, _needRefreshTex(true)
	, _needRefreshSampler(true)
	, _texID(0)
	, _target(target)
	, _width(DEFAULT_WIDTH)
	, _height(DEFAULT_HEIGHT)
	, _depthOrLayer(DEFAULT_DEPTH)
	, _dataType(GL_UNSIGNED_BYTE)
	, _internalFormat(GL_RGBA8)
	, _pixelFormat(GL_RGBA)
	, _dataSize(0)
	, _isCompressedTex(false)
	, _usage(0)
	, _data(nullptr)
	, _manager(nullptr)
	, _isExchanged(false)
	, _mipMapLevelCount(1)
	, _needGenerateMipMaps(false)
{
	_swizzleMode[0] = SWIZZLE_R;
	_swizzleMode[1] = SWIZZLE_G;
	_swizzleMode[2] = SWIZZLE_B;
	_swizzleMode[3] = SWIZZLE_A;
}

unsigned int veTexture::perPixelSize() const
{
	switch (_internalFormat)
	{
	case GL_ALPHA:
	case GL_DEPTH_COMPONENT:
	case GL_R8:
#if (VE_PLATFORM != VE_PLATFORM_MAC)
	case GL_LUMINANCE:
#endif
		return 1;

	case GL_R16F:
	case GL_DEPTH_COMPONENT16:
	case GL_RG8:
		//case GL_LUMINANCE16:
		return 2;

	case GL_RGB:
	case GL_RGB8:
	case GL_DEPTH_COMPONENT24:
		return 3;

	case GL_RGBA:
	case GL_RGBA8:
	case GL_DEPTH_COMPONENT32F:
	case GL_DEPTH24_STENCIL8:
		return 4;

	//case GL_RGB16:
	case GL_RGB16F:
		return 6;

	//case GL_RGBA16:
	case GL_RGBA16F:
#if (VE_PLATFORM != VE_PLATFORM_MAC)
	case GL_COMPRESSED_R11_EAC:
	case GL_COMPRESSED_SIGNED_R11_EAC:
	case GL_COMPRESSED_RGB8_ETC2:
	case GL_COMPRESSED_SRGB8_ETC2:
	case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
	case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
#endif
		return 8;

	case GL_RGB32F:
		return 12;

	case GL_RGBA32F:
#if (VE_PLATFORM != VE_PLATFORM_MAC)
	case GL_COMPRESSED_RG11_EAC:
	case GL_COMPRESSED_SIGNED_RG11_EAC:
	case GL_COMPRESSED_RGBA8_ETC2_EAC:
	case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
#endif
		return 16;
	}
	return 0;
}

unsigned int veTexture::getImageSize(int width, int height) const
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
#if (VE_PLATFORM != VE_PLATFORM_MAC)
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
#endif
	return false;
}

bool veTexture::isSupportFormat(GLenum internalformat)
{
	switch (internalformat)
	{
	case GL_R8:
	case GL_R8_SNORM:
	case GL_R16F:
	case GL_R32F:
	case GL_R8UI:
	case GL_R8I:
	case GL_R16UI:
	case GL_R16I:
	case GL_R32UI:
	case GL_R32I:
	case GL_RG8:
	case GL_RG8_SNORM:
	case GL_RG16F:
	case GL_RG32F:
	case GL_RG8UI:
	case GL_RG8I:
	case GL_RG16UI:
	case GL_RG16I:
	case GL_RG32UI:
	case GL_RG32I:
	case GL_RGB8:
	case GL_SRGB8:
	case GL_RGB:
	case GL_RGB565:
	case GL_RGB8_SNORM:
	case GL_R11F_G11F_B10F:
	case GL_RGB9_E5:
	case GL_RGB16F:
	case GL_RGB32F:
	case GL_RGB8UI:
	case GL_RGB8I:
	case GL_RGB16UI:
	case GL_RGB16I:
	case GL_RGB32UI:
	case GL_RGB32I:
	case GL_RGBA:
	case GL_RGBA8:
	case GL_SRGB8_ALPHA8:
	case GL_RGBA8_SNORM:
	case GL_RGB5_A1:
	case GL_RGBA4:
	case GL_RGB10_A2:
	case GL_RGBA16F:
	case GL_RGBA32F:
	case GL_RGBA8UI:
	case GL_RGBA8I:
	case GL_RGB10_A2UI:
	case GL_RGBA16UI:
	case GL_RGBA16I:
	case GL_RGBA32I:
	case GL_RGBA32UI:
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32F:
	case GL_DEPTH24_STENCIL8:
#if (VE_PLATFORM != VE_PLATFORM_MAC)
	case GL_LUMINANCE_ALPHA:
	case GL_LUMINANCE:
	case GL_ALPHA:
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
#endif            
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
		glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, _filterMode);
		glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, (_filterMode == NEAREST || _filterMode == NEAREST_MIP_MAP_NEAREST) ? GL_NEAREST : GL_LINEAR);

		glTexParameteri(_target, GL_TEXTURE_WRAP_S, _wrapMode);
		glTexParameteri(_target, GL_TEXTURE_WRAP_T, _wrapMode);
		glTexParameteri(_target, GL_TEXTURE_WRAP_R, _wrapMode);

		glTexParameteri(_target, GL_TEXTURE_SWIZZLE_R, _swizzleMode[0]);
		glTexParameteri(_target, GL_TEXTURE_SWIZZLE_G, _swizzleMode[1]);
		glTexParameteri(_target, GL_TEXTURE_SWIZZLE_B, _swizzleMode[2]);
		glTexParameteri(_target, GL_TEXTURE_SWIZZLE_A, _swizzleMode[3]);

		for (auto &param : _texParameterList) {
			glTexParameteri(_target, param.first, param.second);
		}

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

void veTexture::setTexParameter(GLenum pname, GLint param)
{
	auto &currentParam = _texParameterList[pname];
	if (currentParam == param) return;
	currentParam = param;
	_needRefreshSampler = true;
}

void veTexture::storage(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat, GLenum dataType
	, const unsigned char *data, unsigned int requestMipMapLevels)
{
	if (!isSupportFormat(internalFormat))
		return;

	if (_width == width && _height == height && _depthOrLayer == depth
		&& _internalFormat == internalFormat && _pixelFormat == pixelFormat && _dataType == dataType
		&& _data == data
		&& _mipMapLevelCount == requestMipMapLevels)
		return;

	_width = width;
	_height = height;
	_depthOrLayer = depth;
	_internalFormat = internalFormat;
	_pixelFormat = pixelFormat;
	_dataType = dataType;
	_isCompressedTex = isCompressedTex(internalFormat);

	float wLog2 = log2(width);
	float hLog2 = log2(width);
	_mipMapLevelCount = (wLog2 != float(int(wLog2)) || hLog2 != float(int(hLog2)) || _isCompressedTex) ? 1: requestMipMapLevels;

	unsigned int pixelSize = perPixelSize();
	_dataSize = _width * _height * _depthOrLayer * pixelSize;
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
	if (!isSupportFormat(internalFormat))
		return;

	if (mipmaps.empty()) 
		return;

	if (_internalFormat == internalFormat && _pixelFormat == pixelFormat && _dataType == dataType)
		return;

	_width = mipmaps[0].width;
	_height = mipmaps[0].height;
	_depthOrLayer = mipmaps[0].depth;
	_internalFormat = internalFormat;
	_pixelFormat = pixelFormat;
	_dataType = dataType;
	_mipMapLevelCount = mipmaps.size();
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
			glTexStorage2D(_target, _mipMapLevelCount, _internalFormat, _width, _height);
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
					generateMipMaps();
				}
				else if (!_mipmapLevels.empty()) {
					for (size_t lev = 0; lev < _mipmapLevels.size(); ++lev) {
						glTexSubImage2D(_target, lev, 0, 0, _mipmapLevels[lev].width, _mipmapLevels[lev].height, _pixelFormat, _dataType, _mipmapLevels[lev].data);
					}
					generateMipMaps();
				}
			}
			_needRefreshTex = false;
		}
	}

	if (_needGenerateMipMaps && 1 < _mipMapLevelCount) {
		glGenerateMipmap(_target);
		_needGenerateMipMaps = false;
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
			glTexStorage3D(_target, _mipMapLevelCount, _internalFormat, _width, _height, _depthOrLayer);

			if (_isCompressedTex) {
				if (_data) {
					unsigned int imageSize = getImageSize(_width, _height);
					glCompressedTexSubImage3D(_target, 0, 0, 0, 0, _width, _height, _depthOrLayer, _internalFormat, imageSize, _data);
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
					glTexSubImage3D(_target, 0, 0, 0, 0, _width, _height, _depthOrLayer, _pixelFormat, _dataType, _data);
					generateMipMaps();
				}
				else if (!_mipmapLevels.empty()) {
					for (size_t lev = 0; lev < _mipmapLevels.size(); ++lev) {
						glTexSubImage3D(_target, lev, 0, 0, 0, _mipmapLevels[lev].width, _mipmapLevels[lev].height, _mipmapLevels[lev].depth, _pixelFormat, _dataType, _mipmapLevels[lev].data);
					}
					generateMipMaps();
				}
			}
			_needRefreshTex = false;
		}
	}

	if (_needGenerateMipMaps && 1 < _mipMapLevelCount) {
		glGenerateMipmap(_target);
		_needGenerateMipMaps = false;
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
	if (!_textures[0].valid() || !_textures[1].valid() || !_textures[2].valid()
		|| !_textures[3].valid() || !_textures[4].valid() || !_textures[5].valid()) {
		totalMemory = _dataSize * 6;
	}
	else {
		for (unsigned int i = 0; i < 6; ++i) {
			if (_textures[i].valid())
				totalMemory += _textures[i]->getTextureTotalMemory();
		}
	}
	return totalMemory;
}

veTextureCube::~veTextureCube()
{

}

void veTextureCube::bind(unsigned int textureUnit)
{
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		if (_manager->exchangeTextureMemory(this)) {
			_width = _textures[0].valid() ? _textures[0]->getWidth() : _width;
			_height = _textures[0].valid() ? _textures[0]->getHeight() : _height;
			_internalFormat = _textures[0].valid() ? _textures[0]->getInternalFormat() : _internalFormat;
			_pixelFormat = _textures[0].valid() ? _textures[0]->getPixelFormat() : _pixelFormat;
			_dataType = _textures[0].valid() ? _textures[0]->getDataType() : _dataType;
			_mipMapLevelCount = _textures[0].valid() ? _textures[0]->getMipMapLevelCount() : _mipMapLevelCount;
			glTexStorage2D(_target, _mipMapLevelCount, _internalFormat, _width, _height);
			do 
			{
				if (!_textures[0].valid() || !_textures[1].valid() || !_textures[2].valid()
					|| !_textures[3].valid() || !_textures[4].valid() || !_textures[5].valid())
					break;
				for (unsigned int i = 0; i < 6; ++i) {
					const auto &tex = _textures[i];

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
							generateMipMaps();
						}
						else if (!tex->getMipmapLevels().empty()) {
							const auto &mipmaps = tex->getMipmapLevels();
							for (size_t lev = 0; lev < mipmaps.size(); ++lev) {
								glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lev, 0, 0, mipmaps[lev].width, mipmaps[lev].height, tex->getPixelFormat(), tex->getDataType(), mipmaps[lev].data);
							}
							generateMipMaps();
						}
					}
				}
			} while (false);
			_needRefreshTex = false;
		}
	}

	if (_needGenerateMipMaps && 1 < _mipMapLevelCount) {
		glGenerateMipmap(_target);
		_needGenerateMipMaps = false;
	}
}

void veTextureCube::setTexture(CubeMapTexType texType, veTexture *texture)
{
	veAssert(texture->getType() == veTexture::TEXTURE_2D);
	_textures[texType] = texture;
	_needRefreshTex = true;
}

veTexture* veTextureCube::getTexture(CubeMapTexType texType)
{
	return _textures[texType].get();
}

veTexture2DArray::~veTexture2DArray()
{

}

veTexture2DArray::veTexture2DArray()
	: veTexture(GL_TEXTURE_2D_ARRAY)
{
	_type = veTexture::TEXTURE_2D_ARRAY;
}

void veTexture2DArray::bind(unsigned int textureUnit)
{
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		if (_manager->exchangeTextureMemory(this)) {
			glTexStorage3D(_target, _mipMapLevelCount, _internalFormat, _width, _height, _depthOrLayer);

			if (_isCompressedTex) {
				if (_data) {
					unsigned int imageSize = getImageSize(_width, _height);
					glCompressedTexSubImage3D(_target, 0, 0, 0, 0, _width, _height, _depthOrLayer, _internalFormat, imageSize, _data);
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
					glTexSubImage3D(_target, 0, 0, 0, 0, _width, _height, _depthOrLayer, _pixelFormat, _dataType, _data);
					generateMipMaps();
				}
				else if (!_mipmapLevels.empty()) {
					for (size_t lev = 0; lev < _mipmapLevels.size(); ++lev) {
						glTexSubImage3D(_target, lev, 0, 0, 0, _mipmapLevels[lev].width, _mipmapLevels[lev].height, _mipmapLevels[lev].depth, _pixelFormat, _dataType, _mipmapLevels[lev].data);
					}
					generateMipMaps();
				}
			}
			_needRefreshTex = false;
		}
	}

	if (_needGenerateMipMaps && 1 < _mipMapLevelCount) {
		glGenerateMipmap(_target);
		_needGenerateMipMaps = false;
	}
}

#if VE_PLATFORM != VE_PLATFORM_ANDROID
veTextureCubeArray::veTextureCubeArray()
	: veTexture(GL_TEXTURE_CUBE_MAP_ARRAY)
{
	_type = veTexture::TEXTURE_CUBE_ARRAY;
}

veTextureCubeArray::~veTextureCubeArray()
{

}

void veTextureCubeArray::bind(unsigned int textureUnit)
{
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		if (_manager->exchangeTextureMemory(this)) {
			_width = !_textures.empty() ? _textures[0].faces[0]->getWidth() : _width;
			_height = !_textures.empty() ? _textures[0].faces[0]->getHeight() : _height;
			_depthOrLayer = !_textures.empty() ? _textures.size() : _depthOrLayer;
			_internalFormat = !_textures.empty() ? _textures[0].faces[0]->getInternalFormat() : _internalFormat;
			_pixelFormat = !_textures.empty() ? _textures[0].faces[0]->getPixelFormat() : _pixelFormat;
			_dataType = !_textures.empty() ? _textures[0].faces[0]->getDataType() : _dataType;
			_mipMapLevelCount = !_textures.empty() ? _textures[0].faces[0]->getMipMapLevelCount() : _mipMapLevelCount;
			glTexStorage3D(_target, _mipMapLevelCount, _internalFormat, _width, _height, _depthOrLayer);

			if (!_textures.empty()) {
				for (unsigned int ly = 0; ly < _textures.size(); ++ly) {
					for (unsigned int i = 0; i < 6; ++i) {
						const auto &tex = _textures[ly].faces[i];
						if (_isCompressedTex) {
							if (tex->getData()) {
								unsigned int imageSize = tex->getImageSize(tex->getWidth(), tex->getHeight());
								glCompressedTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, 0, tex->getWidth(), tex->getHeight(), ly, tex->getInternalFormat(), imageSize, tex->getData());
							}
							else if (!tex->getMipmapLevels().empty()) {
								const auto &mipmaps = tex->getMipmapLevels();
								for (size_t lev = 0; lev < mipmaps.size(); ++lev) {
									unsigned int imageSize = tex->getImageSize(mipmaps[lev].width, mipmaps[lev].height);
									glCompressedTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lev, 0, 0, 0, mipmaps[lev].width, mipmaps[lev].height, ly, tex->getInternalFormat(), imageSize, mipmaps[lev].data);
								}
							}
						}
						else {
							if (tex->getData()) {
								glTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, 0, tex->getWidth(), tex->getHeight(), ly, tex->getPixelFormat(), tex->getDataType(), tex->getData());
								generateMipMaps();
							}
							else if (!tex->getMipmapLevels().empty()) {
								const auto &mipmaps = tex->getMipmapLevels();
								for (size_t lev = 0; lev < mipmaps.size(); ++lev) {
									glTexSubImage3D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, lev, 0, 0, 0, mipmaps[lev].width, mipmaps[lev].height, ly, tex->getPixelFormat(), tex->getDataType(), mipmaps[lev].data);
								}
								generateMipMaps();
							}
						}
					}
				}
			}
			_needRefreshTex = false;
		}
	}

	if (_needGenerateMipMaps && 1 < _mipMapLevelCount) {
		glGenerateMipmap(_target);
		_needGenerateMipMaps = false;
	}
}

void veTextureCubeArray::setTexture(CubeMapTexType texType, unsigned int layer, veTexture *texture)
{
	veAssert(texture->getType() == veTexture::TEXTURE_2D);
	_textures.resize(layer + 1);
	_textures[layer].faces[texType]= texture;
	_needRefreshTex = true;
}

veTexture* veTextureCubeArray::getTexture(CubeMapTexType texType, unsigned int layer)
{
	veAssert(layer < _textures.size());
	return _textures[layer].faces[texType].get();
}

unsigned int veTextureCubeArray::getTextureTotalMemory()
{
	unsigned int totalMemory = 0;
	if (_textures.empty()) {
		totalMemory = _dataSize * _depthOrLayer;
	}
	else {
		for (auto &layer : _textures) {
			for (unsigned int i = 0; i < 6; ++i) {
				if (layer.faces[i].valid())
					totalMemory += layer.faces[i]->getTextureTotalMemory();
			}
		}
	}
	return totalMemory;
}
#endif