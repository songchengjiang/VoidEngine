#ifndef _VE_CONE_
#define _VE_CONE_
#include "RenderableObject.h"

class VE_EXPORT veCone : public veRenderableObject
{
	friend class veSceneManager;
public:
	~veCone();

private:
	veCone(veSceneManager *sm);
};

#endif