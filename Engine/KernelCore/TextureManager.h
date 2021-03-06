#ifndef _VE_TEXTURE_MANAGER_
#define _VE_TEXTURE_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Texture.h"
#include <mutex>

class VE_EXPORT veTextureManager : public veBaseManager
{
	friend class veSceneManager;
	friend class veTexture;
public:

	~veTextureManager();

	virtual void update() override;
	virtual void resourceRecovery(unsigned int contextID) override;

	veTexture* findTexture(const std::string &name);
	veTexture* createTexture(const std::string &name, veTexture::TextureType texType);
	void removeTexture(veTexture *tex);
	bool requestTextureMemory(veTexture *texture) { return (_currentTextureMemory + texture->getTextureTotalMemory()) < _maxTextureMemory; }
	bool exchangeTextureMemory(veTexture *texture, unsigned int contextID);
	bool assignTextureMemory(veTexture *texture);
	bool releaseTextureMemory(veTexture *texture, unsigned int contextID);

	static std::string TYPE() { return "TEXTURE"; }

private:

	float getTextureUsageRate(veTexture *texture);

private:

	veTextureManager(veSceneManager *sm, unsigned int maxTextureMemory = 512 * 1024 * 1024);
	std::vector< VE_Ptr<veTexture> > _texturePool;
	std::vector< std::pair<veTexture *, unsigned int> > _allocatedTexturePool;
	std::mutex               _texturePoolMutex;
	unsigned int _maxTextureMemory;
	unsigned int _currentTextureMemory;
};

#endif