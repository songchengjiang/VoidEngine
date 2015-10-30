#ifndef _VE_IMAGE_
#define _VE_IMAGE_
#include "Prerequisites.h"
#include "Surface.h"

class VE_EXPORT veImage : public veSurface
{
	friend class veSceneManager;
public:

	~veImage();

	virtual void update(veNode *node, veSceneManager *sm) override;

	void setTexture(veTexture *texture);
	veTexture* getTexture() { return _texture.get(); }

protected:
	veImage(veTexture *texture);
};

#endif