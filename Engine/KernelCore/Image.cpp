#include "Image.h"

veImage::veImage(veTexture *texture)
{
	setTexture(texture);
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
			_scaleMat->setValue(veMat4::scale(veVec3(_texture->getWidth() * 0.5f, _texture->getHeight() * 0.5f, 0.0f)));
			_boundingBox.min() = veVec3(-(_texture->getWidth() * 0.5f), -(_texture->getHeight() * 0.5f), -0.5f);
			_boundingBox.max() = veVec3(_texture->getWidth() * 0.5f, _texture->getHeight() * 0.5f, 0.5f);
		}
		_needRefreshMaterial = false;
	}
	veRenderableObject::update(node, sm);
}

void veImage::setTexture(veTexture *texture)
{
	_texture = texture;
	_needRefreshMaterial = true;
}
