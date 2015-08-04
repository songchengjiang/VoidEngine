#ifndef _VE_COMPONENT_
#define _VE_COMPONENT_
#include "Prerequisites.h"

class veNode;
class veVisualiser;
class VE_EXPORT veComponent
{
public:
	veComponent();
	virtual ~veComponent();

	virtual void update(double deltaTime, veNode *node, const veVisualiser *vs) = 0;
};

#endif
