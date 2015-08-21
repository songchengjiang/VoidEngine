#include "Texture.h"

veTexture::~veTexture()
{

}

veTexture::veTexture(veImage *image)
	: USE_VE_PTR_INIT
	, _image(image)
	, _wrapMode(REPEAT)
	, _filterMode(NEAREST)
{

}

veTexture2D::veTexture2D(veImage *image)
	: veTexture(image)
{

}

veTexture2D::~veTexture2D()
{

}
