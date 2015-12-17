#ifndef _VE_RENDERER_
#define _VE_RENDERER_
#include "Component.h"
#include "RenderCommand.h"
#include "VE_Ptr.h"

class veRenderableObject;
class veTechnique;
class veMaterial;
class vePass;
class veUniform;
class VE_EXPORT veRenderer
{
public:

	enum RenderStage
	{
		LIGHTINGING = 1,
		FRAMEBUFFER = 1 << 1,
		RENDERING   = 1 << 2,
		POSTPROCESS = 1 << 3,
	};

	static unsigned short CURRENT_RENDER_STAGE;

	veRenderer();
	USE_VE_PTR;

	virtual ~veRenderer();
	virtual void visit(veNode *node, veRenderableObject *renderableObj, veSceneManager *sm);
	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) = 0;
	virtual void draw(const veRenderCommand &command) = 0;

	void setRenderStageMask(unsigned short mask) { _renderStageMask = mask; }
	unsigned short getRenderStageMask() const { return _renderStageMask; }

protected:

	bool isNeedRendering();

protected:

	unsigned short _renderStageMask;
};

#endif
