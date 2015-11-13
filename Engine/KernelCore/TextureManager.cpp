#include "TextureManager.h"
#include "SceneManager.h"
#include <algorithm>

veTextureManager::veTextureManager(veSceneManager *sm, unsigned int maxTextureMemory)
	: veBaseManager(sm)
	, _maxTextureMemory(maxTextureMemory)
	, _currentTextureMemory(0)
{

}

veTextureManager::~veTextureManager()
{

}

void veTextureManager::update()
{
	for (auto &tex : _allocatedTexturePool) {
		tex->_usage = tex->_usage << 1;
		tex->_isExchanged = false;
	}
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
	//else if (texType == veTexture::TEXTURE_RECT) {
	//	texture = new veTextureRECT;
	//}
	else if (texType == veTexture::TEXTURE_CUBE) {
		texture = new veTextureCube;
	}
	texture->setName(name);
	texture->_manager = this;
	_texturePool.push_back(texture);
	return texture;
}

bool veTextureManager::exchangeTextureMemory(veTexture *texture)
{
	if (assignTextureMemory(texture)) 
		return true;
	std::map<float, std::vector<veTexture *> > pendingReplacetextureList;
	for (auto &iter : _allocatedTexturePool) {
		if (iter != texture && !iter->_isExchanged)
			pendingReplacetextureList[getTextureUsageRate(iter) * iter->getTextureTotalMemory()].push_back(iter);
	}

	for (auto &iter : pendingReplacetextureList) {
		for (auto &tex : iter.second) {
			if (_currentTextureMemory < tex->getTextureTotalMemory())
				return false;
			tex->releaseTextureData();
			if (assignTextureMemory(texture))
				return true;
		}
	}
	return false;
}

bool veTextureManager::assignTextureMemory(veTexture *texture)
{
	if (requestTextureMemory(texture)) {
		_currentTextureMemory += texture->getTextureTotalMemory();
		texture->_isExchanged = true;
		_allocatedTexturePool.push_back(texture);
		return true;
	}
	return false;
}

bool veTextureManager::releaseTextureMemory(veTexture *texture)
{
	auto iter = std::find(_allocatedTexturePool.begin(), _allocatedTexturePool.end(), texture);
	if (iter == _allocatedTexturePool.end()) return false;
	if (_currentTextureMemory < texture->getTextureTotalMemory()) return false;
	_currentTextureMemory -= texture->getTextureTotalMemory();
	texture->_isExchanged = false;
	_allocatedTexturePool.erase(iter);
	return true;
}

float veTextureManager::getTextureUsageRate(veTexture *texture)
{
	unsigned int usage = texture->_usage;
	usage = (usage & 0x55555555) + ((usage >> 1) & 0x55555555);
	usage = (usage & 0x33333333) + ((usage >> 2) & 0x33333333);
	usage = (usage & 0x0f0f0f0f) + ((usage >> 4) & 0x0f0f0f0f);
	usage = (usage & 0x00ff00ff) + ((usage >> 8) & 0x00ff00ff);
	usage = (usage & 0x0000ffff) + ((usage >> 16) & 0x0000ffff);

	return (float)usage / (float)32;
}
