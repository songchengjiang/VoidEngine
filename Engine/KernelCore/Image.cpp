#include "Image.h"

veImage::veImage(veTexture *texture)
{
	_texture = texture;
}

veImage::~veImage()
{

}

void veImage::update(veNode *node, veSceneManager *sm)
{
	if (!_isVisible) return;

	if (_needRefreshMaterial && _texture.valid()) {
		initDefaultMaterial();
		if (_type == veSurface::SURFACE || _type == veSurface::BILLBOARD) {
			_scaleMat->setValue(veMat4::scale(veVec3(_texture->getWidth() / 2.0f, _texture->getHeight() / 2.0f, 0.0f)));
		}
		_needRefreshMaterial = false;
	}
	veRenderableObject::update(node, sm);
}
