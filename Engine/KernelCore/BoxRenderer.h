#ifndef _VE_BOX_RENDERER_
#define _VE_BOX_RENDERER_
#include "Renderer.h"
#include "BaseCore/Array.h"

class VE_EXPORT veBoxRenderer : public veRenderer
{
public:
	veBoxRenderer();
	~veBoxRenderer();

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera, unsigned int contextID) override;
	void draw(veRenderCommand &command) override;

protected:

	void updateBuffer(unsigned int contextID);

protected:

    VE_Ptr<veGLDataBuffer> _vaoBuffer;
    VE_Ptr<veGLDataBuffer> _vboBuffer;
    VE_Ptr<veGLDataBuffer> _iboBuffer;
	veRealArray     _vertices;
	veUint16Array     _indices;
	bool            _needRefresh;
};

#endif
