#ifndef _VE_CAMERA_
#define _VE_CAMERA_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include "BaseCore/Vector4.h"
#include "Node.h"
#include "RenderQueue.h"
#include "FrameBufferObject.h"

class veVisualiser;
class veLight;
struct VE_EXPORT veViewport
{
	int x;
	int y;
	int width;
	int height;

	inline bool operator == (const veViewport& rkVp) const {
		return (x == rkVp.x && y == rkVp.y && width == rkVp.width && height == rkVp.height);
	}

	inline bool operator != (const veViewport& rkVp) const {
		return (x != rkVp.x || y != rkVp.y || width != rkVp.width || height != rkVp.height);
	}
};

class VE_EXPORT veCamera : public veNode
{
public:

	enum class RenderPath
	{
		FORWARD_PATH,
		DEFERRED_PATH
	};

	~veCamera();

	void setProjectionMatrixAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
	void setProjectionMatrixAsPerspective(float fovy, float aspectRatio, float zNear, float zFar);
	veMat4& projectionMatrix() { return _projectionMat; }
	const veMat4& projectionMatrix() const { return _projectionMat; }

	void setViewMatrixAslookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up);
	veMat4& viewMatrix() { return _viewMat; }
	const veMat4& viewMatrix() const { return _viewMat; }

	void setFrameBufferObject(veFrameBufferObject *fbo);
	veFrameBufferObject* getFrameBufferObject() { return _fbo.get(); }

	void setViewport(const veViewport &vp);
	const veViewport& getViewport() const { return _viewport; }

	void setClearColor(const veVec4 &color) { _clearColor = color; }
	const veVec4& getClearColor() const { return _clearColor; }
	void setClearMask(unsigned int mask) { _clearMask = mask; }
	unsigned int getClearMask() const { return _clearMask; }

	void setRenderPath(RenderPath renderPath);
	RenderPath getRenderPath() const { return _renderPath; }

	bool isRenderStateChanged() { return _renderStateChanged; }

	virtual void setMatrix(const veMat4 &mat) override;

	void render(veRenderQueue::RenderCommandList &renderList);

	virtual bool routeEvent(const veEvent &event, veVisualiser *vs) override;
	virtual void visit(veNodeVisitor &visitor) override;

protected:

	veCamera();
	veCamera(const veViewport &vp);
	void renderQueue(veLoopQueue< veRenderCommand > &queue);
	void resize(int width, int height);

protected:

	veMat4 _projectionMat;
	veMat4 _viewMat;
	veViewport   _viewport;
	veVec4       _clearColor;
	unsigned int _clearMask;
	VE_Ptr<veFrameBufferObject> _fbo;

	RenderPath _renderPath;
	bool _renderStateChanged;
};

typedef std::vector< veCamera* > veCameraList;

#endif
