#ifndef _VE_CAMERA_
#define _VE_CAMERA_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include "BaseCore/Vector4.h"
#include "Node.h"
#include "RenderQueue.h"
#include "FrameBufferObject.h"
#include "Plane.h"

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

	enum FrustumPlane
	{
		FRUSTUM_PLANE_NEAR   = 0,
		FRUSTUM_PLANE_FAR    = 1,
		FRUSTUM_PLANE_LEFT   = 2,
		FRUSTUM_PLANE_RIGHT  = 3,
		FRUSTUM_PLANE_TOP    = 4,
		FRUSTUM_PLANE_BOTTOM = 5
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

	const vePlane& getFrustumPlane(FrustumPlane fp);

	bool isRenderStateChanged() { return _renderStateChanged; }

	void render(veRenderQueue::RenderCommandList &renderList);

	virtual void setMatrix(const veMat4 &mat);
	virtual void refresh();

	virtual bool routeEvent(const veEvent &event, veSceneManager *sm) override;
	virtual void visit(veNodeVisitor &visitor) override;
	virtual bool isOutOfFrustum(const veBoundingBox &bbox);


protected:

	veCamera();
	veCamera(const veViewport &vp);
	void renderQueue(veLoopQueue< veRenderCommand > &queue);
	void resize(int width, int height);
	void updateFrustumPlane();
	virtual void updateSceneManager() override;

protected:

	veMat4 _projectionMat;
	veMat4 _viewMat;
	veViewport   _viewport;
	veVec4       _clearColor;
	unsigned int _clearMask;
	VE_Ptr<veFrameBufferObject> _fbo;

	vePlane _frustumPlane[6];
	bool    _needRefreshFrustumPlane;

	RenderPath _renderPath;
	bool _renderStateChanged;
};

typedef std::vector< veCamera* > veCameraList;

#endif
