#include "TextureManager.h"

veTextureManager::veTextureManager()
{

}

veTextureManager::~veTextureManager()
{

}

veTexture* veTextureManager::getTexture(const std::string &name)
{
	auto iter = _texturePool.find(name);
	if (iter == _texturePool.end()) return nullptr;

	return iter->second.get();
}

veTexture* veTextureManager::createTexture(const std::string &name, veTexture::TextureType texType /*= veTexture::TEXTURE_2D*/)
{
	veTexture *texture = nullptr;
	if (texType == veTexture::TEXTURE_2D) {
		texture = new veTexture2D;
	}
	else if (texType == veTexture::TEXTURE_RECT) {
		texture = new veTextureRECT;
	}
	texture->setName(name);
	_texturePool[name] = texture;
	return texture;
}