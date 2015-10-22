#ifndef _VE_TEXTURE_MANAGER_
#define _VE_TEXTURE_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Texture.h"
#include <unordered_map>

class VE_EXPORT veTextureManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veTextureManager();

	veTexture* getTexture(const std::string &name);
	veTexture* createTexture(const std::string &name, veTexture::TextureType texType);

	static std::string TYPE() { return "TEXTURE"; }

private:

	veTextureManager();
	std::unordered_map< std::string, VE_Ptr<veTexture> > _texturePool;
};

#endif