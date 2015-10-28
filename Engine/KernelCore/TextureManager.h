#ifndef _VE_TEXTURE_MANAGER_
#define _VE_TEXTURE_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Texture.h"

class VE_EXPORT veTextureManager : public veBaseManager
{
	friend class veSceneManager;
	friend class veTexture;
public:

	~veTextureManager();

	virtual void update() override;

	veTexture* findTexture(const std::string &name);
	veTexture* createTexture(const std::string &name, veTexture::TextureType texType);
	bool requestTextureMemory(veTexture *texture) { return (_currentTextureMemory + texture->getTextureTotalMemory()) < _maxTextureMemory; }
	bool exchangeTextureMemory(veTexture *texture);
	bool assignTextureMemory(veTexture *texture);
	bool releaseTextureMemory(veTexture *texture);

	static std::string TYPE() { return "TEXTURE"; }

private:

	float getTextureUsageRate(veTexture *texture);

private:

	veTextureManager(veSceneManager *sm, unsigned int maxTextureMemory = 64 * 1024 * 1024);
	std::vector< VE_Ptr<veTexture> > _texturePool;
	std::vector<veTexture *> _allocatedTexturePool;
	unsigned int _maxTextureMemory;
	unsigned int _currentTextureMemory;
};

#endif