#include "Texture.h"
#include "TextureManager.h"
#include "FileCore/File.h"
const int veTexture::DEFAULT_WIDTH = 512;
const int veTexture::DEFAULT_HEIGHT = 512;

const int veTexture::DEFAULT_DEPTH = 1;
const int veTexture::DEFAULT_INTERNAL_FORMAT = GL_RGBA32F;

veTexture::~veTexture()
{
	glDeleteTextures(1, &_texID);
	VE_SAFE_DELETE_ARRAY(_data);
}

veTexture::veTexture(GLenum target)
	: USE_VE_PTR_INIT
	, _wrapMode(REPEAT)
	, _filterMode(NEAREST)
	, _needRefreshTex(true)
	, _needRefreshSampler(true)
	, _texID(0)
	, _samplerID(0)
	, _target(target)
	, _width(DEFAULT_WIDTH)
	, _height(DEFAULT_HEIGHT)
	, _depth(DEFAULT_DEPTH)
	, _dataType(GL_UNSIGNED_BYTE)
	, _internalFormat(GL_RGBA8)
	, _pixelFormat(GL_RGBA)
	, _dataSize(0)
	, _usage(0)
	, _data(nullptr)
	, _manager(nullptr)
	, _isExchanged(false)
{
}

unsigned int veTexture::perPixelSize()
{
	switch (_internalFormat)
	{
	case GL_ALPHA:
	case GL_DEPTH_COMPONENT:
	case GL_R8:
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
		return 8;

	case GL_RGB32F:
		return 12;

	case GL_RGBA32F:
		return 16;
	}
	return 0;
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
	if (!_samplerID) {
		glGenSamplers(1, &_samplerID);
	}
	if (_needRefreshSampler) {
		glSamplerParameteri(_samplerID, GL_TEXTURE_MIN_FILTER, _filterMode);
		glSamplerParameteri(_samplerID, GL_TEXTURE_MAG_FILTER, _filterMode);
		glSamplerParameteri(_samplerID, GL_TEXTURE_WRAP_S, _wrapMode);
		glSamplerParameteri(_samplerID, GL_TEXTURE_WRAP_T, _wrapMode);
		glSamplerParameteri(_samplerID, GL_TEXTURE_WRAP_R, _wrapMode);
		_needRefreshSampler = false;
	}

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(_target, _texID);
	glBindSampler(textureUnit, _samplerID);
	_usage |= 1;
}

void veTexture::storage(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat, GLenum dataType, unsigned char *data)
{
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
	unsigned int pixelSize = perPixelSize();
	_dataSize = _width * _height * _depth * pixelSize;
	VE_SAFE_DELETE_ARRAY(_data);
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
			glTexStorage2D(_target, 1, _internalFormat, _width, _height);
			if (_data) {
				glTexSubImage2D(_target, 0, 0, 0, _width, _height, _pixelFormat, _dataType, _data);
			}
			_needRefreshTex = false;
		}
	}
}

//veTextureRECT::veTextureRECT()
//	: veTexture(GL_TEXTURE_RECTANGLE)
//{
//	_type = veTexture::TEXTURE_RECT;
//}
//
//veTextureRECT::~veTextureRECT()
//{
//
//}
//
//void veTextureRECT::bind(unsigned int textureUnit)
//{
//	veTexture::bind(textureUnit);
//	if (_needRefreshTex) {
//		if (_manager->exchangeTextureMemory(this)) {
//			glTexStorage2D(_target, 1, _internalFormat, _width, _height);
//			if (_data) {
//				glTexSubImage2D(_target, 0, 0, 0, _width, _height, _pixelFormat, _dataType, _data);
//			}
//			_needRefreshTex = false;
//		}
//	}
//}


veTextureCube::veTextureCube()
	: veTexture(GL_TEXTURE_CUBE_MAP)
{

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
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		if (_manager->exchangeTextureMemory(this)) {
			glTexStorage2D(_target, 1, _textures[0]->getInternalFormat(), _textures[0]->getWidth(), _textures[0]->getHeight());
			for (unsigned int i = 0; i < 6; ++i) {
				auto &tex = _textures[i];
				//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, tex->getInternalFormat(), tex->getWidth(), tex->getHeight(), 0, tex->getPixelFormat(), tex->getDataType(), tex->getData());
				if (tex->getData()) {
					glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, tex->getWidth(), tex->getHeight(), tex->getPixelFormat(), tex->getDataType(), tex->getData());
				}
			}
			_needRefreshTex = false;
		}
	}
}

void veTextureCube::setTexture(CubeMapTexType texType, veTexture *texture)
{
	_textures[texType] = texture;
	_needRefreshTex = true;
}