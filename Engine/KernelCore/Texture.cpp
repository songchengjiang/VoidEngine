#include "Texture.h"

veTexture::~veTexture()
{

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
{
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

veTexture2D::veTexture2D(veImage *image)
	: veTexture(image, GL_TEXTURE_2D)
{

}

veTexture2D::~veTexture2D()
{
}

void veTexture2D::bind(unsigned int textureUnit)
{
	veTexture::bind(textureUnit);
	if (_image.valid() && _needRefreshTex) {
		glTexStorage2D(_target, 1, _image->internalFormat(), _image->width(), _image->height());
		glTexSubImage2D(_target, 0, 0, 0, _image->width(), _image->height(), _image->pixelFormat(), _image->dataType(), _image->data());
		_needRefreshTex = false;
	}
}
