#ifndef _VE_SKY_BOX_
#define _VE_SKY_BOX_
#include "Prerequisites.h"
#include "RenderableObject.h"

class VE_EXPORT veSkyBox : public veRenderableObject
{
	friend class veSceneManager;
public:
	~veSkyBox();

	void setSize(veReal size);
	veReal getSize()const { return _size; }

protected:

	veSkyBox(veReal size);

protected:

	veReal _size;
};

#endif
