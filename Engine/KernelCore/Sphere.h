#ifndef _VE_SPHERE_
#define _VE_SPHERE_
#include "RenderableObject.h"

class VE_EXPORT veSphere : public veRenderableObject
{
	friend class veSceneManager;
public:
	~veSphere();

private:
	veSphere(veSceneManager *sm);
};

#endif