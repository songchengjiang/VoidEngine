#ifndef _VE_RENDERABLE_OBJECT_
#define _VE_RENDERABLE_OBJECT_
#include "Prerequisites.h"
#include "Renderer.h"

class veNode;
class veVisualiser;
class VE_EXPORT veRenderableObject
{
public:

	veRenderableObject();
	virtual ~veRenderableObject();

	void update(veNode *node, veVisualiser *vs);

	void setName(const std::string &name) { _name = name; }
	const std::string& getName() const { return _name; }
	inline void setRenderer(veRenderer *renderer);
	veRenderer* getRenderer() const { return _renderer.get(); }

protected:

	virtual void updateImp(veNode *node, veVisualiser *vs) = 0;

protected:

	VE_Ptr<veRenderer> _renderer;
	std::string _name;
};

#endif