#include "Image.h"

veImage::veImage()
	: USE_VE_PTR_INIT
	, _data(nullptr)
	, _needRefresh(true)
{

}

veImage::~veImage()
{
	VE_SAFE_DELETE_ARRAY(_data);
}

void veImage::set(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat, GLenum dataType, unsigned char *data)
{
	_width = width;
	_height = height;
	_depth = depth;
	_internalFormat = internalFormat;
	_pixelFormat = pixelFormat;
	_dataType = dataType;
	VE_SAFE_DELETE_ARRAY(_data);
	unsigned int pixelSize = perPixelSize();
	_data = new unsigned char[_width * _height * _depth * pixelSize];
	memcpy(_data, data, _width * _height * _depth * pixelSize);
	_needRefresh = true;
}

veVec4 veImage::getColor(int s, int t, int r) const
{
	unsigned char *ptr = &_data[s * _height * _depth + t * _height + r];
	return veVec4(ptr[0], ptr[1], ptr[2], ptr[3]);
}

unsigned int veImage::perPixelSize()
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

	case GL_RGB16:
	case GL_RGB16F:
		return 6;

	case GL_RGBA16:
	case GL_RGBA16F:
		return 8;

	case GL_RGB32F:
		return 12;

	case GL_RGBA32F:
		return 16;
	}
	return 0;
}
