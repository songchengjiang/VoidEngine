#ifndef _VE_DEBUGER_
#define _VE_DEBUGER_
#include "Prerequisites.h"
#include "KernelCore/Component.h"
#include "BaseCore/Array.h"
#include "KernelCore/Material.h"
#include "KernelCore/Node.h"

class veNode;
class veMesh;
class vePlane;
class veBoundingBox;
class VE_EXPORT veDebuger : public veNode
{
public:
    veDebuger();
    ~veDebuger();
    
	void setDebugNode(veNode *node);
	virtual void update(veSceneManager *sm) override;
	virtual void render(veCamera *camera) override;

	void debugDrawMeshWireframe(bool isDraw) { _isDrawMeshWireframe = isDraw; }
	void debugDrawBoundingBoxWireframe(bool isDraw) { _isDrawBoundingBoxWireframe = isDraw; }
	void debugDrawFrustumPlane(bool isDraw) { _isDrawFrustumPlane = isDraw; }
	void setDebugDrawColor(const veVec4 &color) { _color = color; }

protected:

	void initMaterial();
	void renderMeshWireframe(veMesh *mesh, const veMat4 &trans);
	void renderBoundingBoxWireframe(const veBoundingBox &bbox, const veMat4 &trans);
	void renderFrustumPlanes(veCamera *camera, const veMat4 &trans);
	void draw(const veRenderCommand &command);
	void drawLine(const veVec3 &start, const veVec3 &end);

	veVec3 getPlaneCrossPoint(const vePlane &p0, const vePlane &p1, const vePlane &p2);
    
protected:
    
    std::vector<veNode *> _renderableNodes;
	veRealArray                       _vertices;
	VE_Ptr<veMaterial>                _material;
	GLuint          _vao;
	GLuint          _vbo;
	veNode         *_attachedNode;

	veVec4          _color;
	VE_Ptr<veUniform> _colorUniform;
	bool            _isDrawMeshWireframe;
	bool            _isDrawBoundingBoxWireframe;
	bool            _isDrawFrustumPlane;
};

#endif /* Debuger_h */
