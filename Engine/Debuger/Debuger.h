#ifndef _VE_DEBUGER_
#define _VE_DEBUGER_
#include "Prerequisites.h"
#include "KernelCore/Component.h"
#include "BaseCore/Array.h"
#include "KernelCore/Material.h"
#include "KernelCore/Node.h"
#include <mutex>

class veNode;
class veMesh;
class vePlane;
class veBoundingBox;
class VE_EXPORT veDebuger : public veRenderableObject
{
public:
    veDebuger();
    virtual ~veDebuger();
    
	virtual void render(veNode *node, veCamera *camera) override;

	void debugDrawMeshWireframe(bool isDraw, const veVec4 &color = veVec4::WHITE) {
		_isDrawMeshWireframe = isDraw; 
		_drawMeshWireframeColor = color;
	}
	void debugDrawBoundingBoxWireframe(bool isDraw, const veVec4 &color = veVec4::WHITE) {
		_isDrawBoundingBoxWireframe = isDraw; 
		_drawBoundingBoxWireframeColor = color;
	}
	void debugDrawFrustumPlane(bool isDraw, const veVec4 &color = veVec4::WHITE) {
		_isDrawFrustumPlane = isDraw; 
		_drawFrustumPlaneColor = color;
	}
	void debugDrawLine(const veVec3 &start, const veVec3 &end, const veVec4 &color = veVec4::WHITE);

	void setLineWidth(float width) { _lineWidth = width; }

protected:

	void initMaterial();
	void renderMeshWireframe(veMesh *mesh, const veMat4 &trans);
	void renderBoundingBoxWireframe(const veBoundingBox &bbox, const veVec4 &color);
	void renderFrustumPlanes(veCamera *camera);
	void draw(const veRenderCommand &command);
	void drawLine(const veVec3 &start, const veVec3 &end, const veVec4 &color = veVec4::WHITE);

	veVec3 getPlaneCrossPoint(const vePlane &p0, const vePlane &p1, const vePlane &p2);
    
protected:
    
    std::vector<veNode *> _renderableNodes;
	veRealArray     _vertices;
	unsigned int    _drawCount;
	GLuint          _vao;
	GLuint          _vbo;

	unsigned int    _vertexStride;
	float           _lineWidth;

	bool            _isDrawMeshWireframe;
	veVec4          _drawMeshWireframeColor;

	bool            _isDrawBoundingBoxWireframe;
	veVec4          _drawBoundingBoxWireframeColor;

	bool            _isDrawFrustumPlane;
	veVec4          _drawFrustumPlaneColor;
	std::mutex      _verticesMutex;
};

#endif /* Debuger_h */
