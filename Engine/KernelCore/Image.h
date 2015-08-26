#ifndef _VE_IMAGE_
#define _VE_IMAGE_
#include "Prerequisites.h"
#include "BaseCore/Vector4.h"

class VE_EXPORT veImage
{
public:
	veImage();
	~veImage();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void set(int width, int height, int depth, GLint internalFormat, GLenum pixelFormat, GLenum dataType, unsigned char *data);
	veVec4 getColor(int s, int t, int r) const;
	const unsigned char* data() const { return _data; }
	int width() const { return _width; }
	int height() const { return _height; }
	int depth() const { return _depth; }
	GLint internalFormat() const { return _internalFormat; }
	GLenum pixelFormat() const { return _pixelFormat; }
	GLenum dataType() const { return _dataType; }

private:

	unsigned int perPixelSize();

private:

	int            _width;
	int            _height;
	int            _depth;
	GLint          _internalFormat;
	GLenum         _pixelFormat;
	GLenum         _dataType;
	unsigned char *_data;
};

#endif