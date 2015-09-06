#ifndef _VE_TEXTURE_MANAGER_
#define _VE_TEXTURE_MANAGER_
#include "Prerequisites.h"
#include "Texture.h"
#include <unordered_map>

class VE_EXPORT veTextureManager
{
public:

	veTextureManager();
	~veTextureManager();

	static veTextureManager* instance();
	veTexture* getOrCreateTexture(const std::string &name, veTexture::TextureType texType = veTexture::TEXTURE_2D);

private:

	std::unordered_map< std::string, VE_Ptr<veTexture> > _texturePool;
};

#endif