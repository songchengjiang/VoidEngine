#include "TextureManager.h"

veTextureManager::veTextureManager(veSceneManager *sm)
	: veBaseManager(sm)
{

}

veTextureManager::~veTextureManager()
{

}

veTexture* veTextureManager::findTexture(const std::string &name)
{
	for (auto &iter : _texturePool) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
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
	_texturePool.push_back(texture);
	return texture;
}