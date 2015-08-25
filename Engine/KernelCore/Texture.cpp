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
	, _texID(0)
	, _target(0)
{
}

void veTexture::bind(unsigned int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	if (!_texID) {
		glCreateTextures(_target, 1, &_texID);
	}
	glBindTexture(_target, _texID);
	if (_needRefreshTex) {
		_needRefreshTex = false;
	}
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
}
