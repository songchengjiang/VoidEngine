#include "Texture.h"

const int veTexture::DEFAULT_WIDTH = 512;
const int veTexture::DEFAULT_HEIGHT = 512;

const int veTexture::DEFAULT_DEPTH = 1;
const int veTexture::DEFAULT_INTERNAL_FORMAT = GL_RGBA32F;

veTexture::~veTexture()
{
	glDeleteTextures(1, &_texID);
}

veTexture::veTexture(veImage *image, GLenum target)
	: USE_VE_PTR_INIT
	, _image(image)
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
	, _internalFormat(GL_RGBA8)
	, _autoWidth(false)
	, _autoHeight(false)
{
	if (_image.valid()) {
		_width = _image->width();
		_height = _image->height();
		_depth = _image->depth();
		_internalFormat = _image->internalFormat();
	}
}

void veTexture::bind(unsigned int textureUnit)
{
	if (!_texID) {
		glCreateTextures(_target, 1, &_texID);
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
}

void veTexture::setImage(veImage *image)
{
	_image = image; 
	_width = _image->width();
	_height = _image->height();
	_depth = _image->depth();
	_internalFormat = _image->internalFormat();
	_needRefreshTex = true;
}

void veTexture::storage(GLint internalFormat, int width, int height, int depth)
{
	_width = width;
	_height = height;
	_depth = depth;
	_internalFormat = internalFormat;
	_needRefreshTex = true;
}

GLuint veTexture::glTex()
{
	if (!_texID) {
		glCreateTextures(_target, 1, &_texID);
	}
	return _texID;
}

veTexture2D::veTexture2D(veImage *image)
	: veTexture(image, GL_TEXTURE_2D)
{
	_type = veTexture::TEXTURE_2D;
}

veTexture2D::~veTexture2D()
{
}

void veTexture2D::bind(unsigned int textureUnit)
{
	if (_needRefreshTex && _texID) {
		glDeleteTextures(1, &_texID);
		_texID = 0;
	}
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		glTexStorage2D(_target, 1, _internalFormat, _width, _height);
		if (_image.valid())
			glTexSubImage2D(_target, 0, 0, 0, _image->width(), _image->height(), _image->pixelFormat(), _image->dataType(), _image->data());
		_needRefreshTex = false;
	}
}

veTextureRECT::veTextureRECT(veImage *image /*= nullptr*/)
	: veTexture(image, GL_TEXTURE_RECTANGLE)
{
	_type = veTexture::TEXTURE_RECT;
}

veTextureRECT::~veTextureRECT()
{

}

void veTextureRECT::bind(unsigned int textureUnit)
{
	veTexture::bind(textureUnit);
	if (_needRefreshTex) {
		glTexStorage2D(_target, 1, _internalFormat, _width, _height);
		if (_image.valid())
			glTexSubImage2D(_target, 0, 0, 0, _image->width(), _image->height(), _image->pixelFormat(), _image->dataType(), _image->data());
		_needRefreshTex = false;
	}
}
