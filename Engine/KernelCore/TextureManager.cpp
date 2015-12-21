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
	for (auto &tex : _texturePool) {
		releaseTextureMemory(tex.get());
	}
}

void veTextureManager::update()
{
	std::unique_lock<std::mutex> lock(_texturePoolMutex);
	for (auto &tex : _allocatedTexturePool) {
		tex->_usage = tex->_usage << 1;
		tex->_isExchanged = false;
	}
}

void veTextureManager::resourceRecovery()
{
	for (std::vector< VE_Ptr<veTexture> >::iterator iter = _texturePool.begin(); iter != _texturePool.end(); ) {
		if ((*iter)->refCount() <= 1) {
			iter = _texturePool.erase(iter);
		}else {
			++iter;
		}
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
	else if (texType == veTexture::TEXTURE_2D_ARRAY) {
		texture = new veTexture2DArray;
	}
	else if (texType == veTexture::TEXTURE_3D) {
		texture = new veTexture3D;
	}
	else if (texType == veTexture::TEXTURE_CUBE) {
		texture = new veTextureCube;
	}
#if VE_PLATFORM != VE_PLATFORM_ANDROID
	else if (texType == veTexture::TEXTURE_CUBE_ARRAY) {
		texture = new veTextureCubeArray;
	}
#endif
	texture->setName(name);
	texture->_manager = this;
	_texturePool.push_back(texture);
	return texture;
}
//
//void veTextureManager::removeTexture(veTexture *tex)
//{
//	auto iter = std::find(_texturePool.begin(), _texturePool.end(), tex);
//	if (iter != _texturePool.end()) {
//		tex->_manager = nullptr;
//		_texturePool.erase(iter);
//	}
//}

bool veTextureManager::exchangeTextureMemory(veTexture *texture)
{
	std::unique_lock<std::mutex> lock(_texturePoolMutex);
	if (assignTextureMemory(texture)) 
		return true;
	std::map<float, std::vector<veTexture *> > pendingReplacetextureList;
	for (auto &iter : _allocatedTexturePool) {
		if (iter != texture && !iter->_isExchanged)
			pendingReplacetextureList[getTextureUsageRate(iter) * iter->getTextureTotalMemory()].push_back(iter);
	}

	for (auto &iter : pendingReplacetextureList) {
		for (auto &tex : iter.second) {
			releaseTextureMemory(tex);
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
	texture->releaseTextureData();
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
