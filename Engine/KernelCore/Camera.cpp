#include "Camera.h"
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "TextureManager.h"

veLoopQueue<veRenderCommand>::SortFunc PASS_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority? left.pass <= right.pass: right.priority <= left.priority;
};

veLoopQueue<veRenderCommand>::SortFunc ENTITY_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.pass <= right.pass : right.priority <= left.priority;
};

veLoopQueue<veRenderCommand>::SortFunc TRANSPARENT_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return right.priority == left.priority ? left.depthInCamera <= right.depthInCamera : right.priority <= left.priority;
};

veLoopQueue<veRenderCommand>::SortFunc OVERLAY_SORT = [](const veRenderCommand &left, const veRenderCommand &right)->bool {
	return left.priority <= right.priority;
};

veCamera::veCamera(veSceneManager *sm)
	: _viewMat(veMat4::IDENTITY)
	, _projectionMat(veMat4::IDENTITY)
	, _clearColor(0.2f)
	, _clearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	, _fbo(nullptr)
	, _renderPath(RenderPath::FORWARD_PATH)
	, _isDiscardRenderScene(false)
	, _needRefreshFrustumPlane(true)
	, _renderQueue(nullptr)
	, _isShadowCamera(false)
{
	_sceneManager = sm;
	setViewport({ 0, 0, 0, 0 });
}

veCamera::veCamera(veSceneManager *sm, const veViewport &vp)
	: _viewMat(veMat4::IDENTITY)
	, _projectionMat(veMat4::IDENTITY)
	, _clearColor(0.2f)
	, _clearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	, _fbo(nullptr)
	, _renderPath(RenderPath::FORWARD_PATH)
	, _isDiscardRenderScene(false)
	, _needRefreshFrustumPlane(true)
	, _renderQueue(nullptr)
	, _isShadowCamera(false)
{
	_sceneManager = sm;
	setViewport(vp);
}

veCamera::~veCamera()
{
	VE_SAFE_DELETE(_renderQueue);
}

void veCamera::update(veSceneManager *sm, const veMat4 &transform)
{
	bool needUpdateViewMat = _refresh? true: false;
	veNode::update(sm, transform);
	if (needUpdateViewMat) {
		_viewMat = getWorldToNodeMatrix();
	}
}

void veCamera::setProjectionMatrixAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	_projectionMat.set(2.0f / (right - left), 0.0f                 , 0.0f                 , -(right + left) / (right - left)
		             , 0.0f                 , 2.0f / (top - bottom), 0.0f                 , -(top + bottom) / (top - bottom)
		             , 0.0f                 , 0.0f                 , 2.0f / (zNear - zFar), (zNear + zFar) / (zNear - zFar)
		             , 0.0f                 , 0.0f                 , 0.0f                 , 1.0f);
	_needRefreshFrustumPlane = true;
}

void veCamera::setProjectionMatrixAsPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	float top = zNear * tan(veMath::veRadian(fovy * 0.5f));
	float bottom = -top;
	float right = top * aspectRatio;
	float left = -right;
	float a = -(zFar + zNear) / (zFar - zNear);
	float b = -(2.0 * zNear * zFar) / (zFar - zNear);

	_projectionMat.set((2.0f * zNear) / (right - left), 0.0f                           , (right + left) / (right - left), 0.0f
		              , 0.0f                          , (2.0f * zNear) / (top - bottom), (top + bottom) / (top - bottom), 0.0f
		              , 0.0f                          , 0.0f                           , a                              , b
		              , 0.0f                          , 0.0f                           , -1.0f                          , 0.0f);
	_needRefreshFrustumPlane = true;
}

void veCamera::setViewMatrixAslookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up)
{
	setMatrix(veMat4::lookAt(eye, center, up));
	//veVec3 f = eye - center;
	//f.normalize();
	//veVec3 s = up.crossProduct(f);
	//s.normalize();
	//veVec3 u = f.crossProduct(s);
	//if (f.isZeroLength() || s.isZeroLength() || u.isZeroLength()) return;

	//float sdote = s.dotProduct(eye);
	//float udote = u.dotProduct(eye);
	//float fdote = f.dotProduct(eye);
	//setMatrix(veMat4(s.x(), u.x(), f.x(), s.x() * sdote + u.x() * udote + f.x() * fdote
	//	, s.y(), u.y(), f.y(), s.y() * sdote + u.y() * udote + f.y() * fdote
	//	, s.z(), u.z(), f.z(), s.z() * sdote + u.z() * udote + f.z() * fdote
	//	, 0.0f, 0.0f, 0.0f, 1.0f));

	//_viewMat.set(s.x(), s.y(), s.z(), -s.dotProduct(eye)
	//	, u.x(), u.y(), u.z(), -u.dotProduct(eye)
	//	, f.x(), f.y(), f.z(), -f.dotProduct(eye)
	//	, 0.0f, 0.0f, 0.0f, 1.0f);
}

veVec3 veCamera::convertScreenCoordsToWorldCoords(const veVec2 &sCoords, veReal zDepth)
{
	veMat4 projMatInverse = _projectionMat;
	projMatInverse.inverse();
	return _matrix * projMatInverse * veVec3(sCoords.x(), sCoords.y(), zDepth);
}

void veCamera::setFrameBufferObject(veFrameBufferObject *fbo)
{
	_fbo = fbo;
	if (_fbo.valid())
		_fbo->setFrameBufferSize(veVec2(_viewport.width - _viewport.x, _viewport.height - _viewport.y));
}

void veCamera::setViewport(const veViewport &vp)
{
	if (_viewport == vp) return;
	_viewport = vp;
	if (_fbo.valid())
		_fbo->setFrameBufferSize(veVec2(_viewport.width - _viewport.x, _viewport.height - _viewport.y));
}

void veCamera::setRenderPath(RenderPath renderPath)
{
	_renderPath = renderPath;
	_sceneManager->needReload();
}

void veCamera::setSkybox(veSkyBox *skybox)
{
	if (_skybox == skybox)
		return;
	_skybox = skybox;
}

const vePlane& veCamera::getFrustumPlane(FrustumPlane fp)
{
	updateFrustumPlane();
	return _frustumPlane[fp];
}

void veCamera::setMatrix(const veMat4 &mat)
{
	refresh();
	_matrix = mat;
	_viewMat = getWorldToNodeMatrix();
}

void veCamera::refresh()
{
	veNode::refresh();
	_needRefreshFrustumPlane = true;
}

void veCamera::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

bool veCamera::isOutOfFrustum(const veBoundingBox &bbox)
{
	updateFrustumPlane();
	veVec3 center = bbox.center();
	veVec3 halfSize = (bbox.max() - bbox.min()) * 0.5f;
	for (unsigned int i = 0; i < 6; ++i) {
		auto side = _frustumPlane[i].getSide(center, halfSize);
		if (side == vePlane::NEGATIVE_SIDE)
			return true;
	}

	return false;
}

void veCamera::visitQueue(veLoopQueue< veRenderCommand > &queue)
{
	size_t sz = queue.size();
	for (size_t i = 0; i < sz; ++i) {
		auto &cmd = queue[i];
		cmd.renderer->draw(cmd);
	}
}

void veCamera::resize(int width, int height)
{
	double widthChangeRatio = double(width) / double(_viewport.width - _viewport.x);
	double heigtChangeRatio = double(height) / double(_viewport.height - _viewport.y);
	double aspectInverseRatioChange = heigtChangeRatio / widthChangeRatio;

	_projectionMat = veMat4::scale(veVec3(aspectInverseRatioChange, 1.0f, 1.0f)) * _projectionMat;
	this->setViewport({ 0, 0, width, height });
}

void veCamera::updateFrustumPlane()
{
	if (_needRefreshFrustumPlane) {
		veMat4 vpMat = _projectionMat * _viewMat;
		_frustumPlane[FRUSTUM_PLANE_LEFT].normal().x() = vpMat[3][0] + vpMat[0][0];
		_frustumPlane[FRUSTUM_PLANE_LEFT].normal().y() = vpMat[3][1] + vpMat[0][1];
		_frustumPlane[FRUSTUM_PLANE_LEFT].normal().z() = vpMat[3][2] + vpMat[0][2];
		_frustumPlane[FRUSTUM_PLANE_LEFT].constantD() = vpMat[3][3] + vpMat[0][3];

		_frustumPlane[FRUSTUM_PLANE_RIGHT].normal().x() = vpMat[3][0] - vpMat[0][0];
		_frustumPlane[FRUSTUM_PLANE_RIGHT].normal().y() = vpMat[3][1] - vpMat[0][1];
		_frustumPlane[FRUSTUM_PLANE_RIGHT].normal().z() = vpMat[3][2] - vpMat[0][2];
		_frustumPlane[FRUSTUM_PLANE_RIGHT].constantD() = vpMat[3][3] - vpMat[0][3];

		_frustumPlane[FRUSTUM_PLANE_TOP].normal().x() = vpMat[3][0] - vpMat[1][0];
		_frustumPlane[FRUSTUM_PLANE_TOP].normal().y() = vpMat[3][1] - vpMat[1][1];
		_frustumPlane[FRUSTUM_PLANE_TOP].normal().z() = vpMat[3][2] - vpMat[1][2];
		_frustumPlane[FRUSTUM_PLANE_TOP].constantD() = vpMat[3][3] - vpMat[1][3];

		_frustumPlane[FRUSTUM_PLANE_BOTTOM].normal().x() = vpMat[3][0] + vpMat[1][0];
		_frustumPlane[FRUSTUM_PLANE_BOTTOM].normal().y() = vpMat[3][1] + vpMat[1][1];
		_frustumPlane[FRUSTUM_PLANE_BOTTOM].normal().z() = vpMat[3][2] + vpMat[1][2];
		_frustumPlane[FRUSTUM_PLANE_BOTTOM].constantD() = vpMat[3][3] + vpMat[1][3];

		_frustumPlane[FRUSTUM_PLANE_NEAR].normal().x() = vpMat[3][0] + vpMat[2][0];
		_frustumPlane[FRUSTUM_PLANE_NEAR].normal().y() = vpMat[3][1] + vpMat[2][1];
		_frustumPlane[FRUSTUM_PLANE_NEAR].normal().z() = vpMat[3][2] + vpMat[2][2];
		_frustumPlane[FRUSTUM_PLANE_NEAR].constantD() = vpMat[3][3] + vpMat[2][3];

		_frustumPlane[FRUSTUM_PLANE_FAR].normal().x() = vpMat[3][0] - vpMat[2][0];
		_frustumPlane[FRUSTUM_PLANE_FAR].normal().y() = vpMat[3][1] - vpMat[2][1];
		_frustumPlane[FRUSTUM_PLANE_FAR].normal().z() = vpMat[3][2] - vpMat[2][2];
		_frustumPlane[FRUSTUM_PLANE_FAR].constantD() = vpMat[3][3] - vpMat[2][3];

		// Renormalise any normals which were not unit length
		for (int i = 0; i < 6; i++)
		{
			veReal length = _frustumPlane[i].normal().normalize();
			_frustumPlane[i].constantD() /= length;
		}

		_needRefreshFrustumPlane = false;
	}
}

void veCamera::updateSceneManager()
{
}