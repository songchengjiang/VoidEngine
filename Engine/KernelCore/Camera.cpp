#include "Camera.h"
#include "Visualiser.h"
#include "Node.h"

veCamera::veCamera()
	: USE_VE_PTR_INIT
	, _viewMat(veMat4::IDENTITY)
	, _projectionMat(veMat4::IDENTITY)
	, _type(RENDER_TO_FRAME)
	, _clearColor(0.8f)
	, _clearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	, _viewport({0, 0, 800, 600})
	, _mask(0xffffffff)
	, _fbo(nullptr)
{

}

veCamera::veCamera(const veViewport &vp)
	: USE_VE_PTR_INIT
	, _viewMat(veMat4::IDENTITY)
	, _projectionMat(veMat4::IDENTITY)
	, _type(RENDER_TO_FRAME)
	, _clearColor(0.8f)
	, _clearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	, _viewport(vp)
	, _mask(0xffffffff)
	, _fbo(nullptr)
{

}

veCamera::~veCamera()
{

}

void veCamera::setProjectionMatrixAsOrtho(float left, float right, float bottom, float top, float zNear, float zFar)
{
	_projectionMat.set(2.0f / (right - left), 0.0f                 , 0.0f                 , -(right + left) / (right - left)
		             , 0.0f                 , 2.0f / (top - bottom), 0.0f                 , -(top + bottom) / (top - bottom)
		             , 0.0f                 , 0.0f                 , 2.0f / (zNear - zFar), (zNear + zFar) / (zNear - zFar)
		             , 0.0f                 , 0.0f                 , 0.0f                 , 1.0f);
}

void veCamera::setProjectionMatrixAsPerspective(float fovy, float aspectRatio, float zNear, float zFar)
{
	float top = zNear * tan(veMath::radian(fovy * 0.5f));
	float bottom = -top;
	float right = top * aspectRatio;
	float left = -right;
	float a = -(zFar + zNear) / (zFar - zNear);
	float b = -(2.0 * zNear * zFar) / (zFar - zNear);

	_projectionMat.set((2.0f * zNear) / (right - left), 0.0f                           , (right + left) / (right - left), 0.0f
		              , 0.0f                          , (2.0f * zNear) / (top - bottom), (top + bottom) / (top - bottom), 0.0f
		              , 0.0f                          , 0.0f                           , a                              , b
		              , 0.0f                          , 0.0f                           , -1.0f                          , 0.0f);
}

void veCamera::setViewMatrixAslookAt(const veVec3 &eye, const veVec3 &center, const veVec3 &up)
{
	veVec3 f = eye - center;
	f.normalize();
	veVec3 s = up.crossProduct(f);
	s.normalize();
	veVec3 u = f.crossProduct(s);
	if (f.isZeroLength() || s.isZeroLength() || u.isZeroLength()) return;
	_viewMat.set(s.x(), s.y(), s.z(), -s.dotProduct(eye)
		       , u.x(), u.y(), u.z(), -u.dotProduct(eye)
		       , f.x(), f.y(), f.z(), -f.dotProduct(eye)
		       , 0.0f , 0.0f , 0.0f , 1.0f);
}

void veCamera::setType(TargetType tarType)
{
	if (tarType == RENDER_TO_TEXTURE) {
		if (!_fbo.valid())
			_fbo = new veFrameBufferObject;
	}
	else {
		_fbo = nullptr;
	}
}

void veCamera::setViewport(const veViewport &vp)
{
	if (_viewport == vp) return;
	_viewport = vp;
}

void veCamera::render(veRenderQueue::RenderCommandList &renderList)
{
	if (_fbo.valid()) {
		_fbo->setSize(veVec2(_viewport.width - _viewport.x, _viewport.height - _viewport.y));
		_fbo->bind();
	}
	glViewport(_viewport.x, _viewport.y, _viewport.width, _viewport.height);
	glClear(_clearMask);
	glClearColor(_clearColor.r(), _clearColor.g(), _clearColor.b(), _clearColor.a());

	for (auto &iter : renderList) {
		auto &q = iter.second;
		q.sort([](const veRenderCommand &left, const veRenderCommand &right)->bool {
			return right.priority <= left.priority;
		});
		while (!q.empty()) {
			const auto &cmd = q.front();
			cmd.renderer->draw(cmd);
			q.pop_front();
		}
	}

	if (_fbo.valid()) {
		_fbo->unBind();
	}
}
