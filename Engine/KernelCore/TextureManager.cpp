#include "TextureManager.h"

veTextureManager::veTextureManager()
{

}

veTextureManager::~veTextureManager()
{

}

veTextureManager* veTextureManager::instance()
{
	static veTextureManager manager;
	return &manager;
}

veTexture* veTextureManager::getOrCreateTexture(const std::string &name, veTexture::TextureType texType)
{
	auto iter = _texturePool.find(name);
	if (iter != _texturePool.end()) return iter->second.get();

	veTexture *texture = nullptr;
	if (texType == veTexture::TEXTURE_2D) {
		texture = new veTexture2D;
		_texturePool[name] = texture;
	}

	return texture;
}
