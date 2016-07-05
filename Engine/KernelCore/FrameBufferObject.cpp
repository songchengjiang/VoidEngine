#include "FrameBufferObject.h"

veFrameBufferObjectManager::veFrameBufferObjectManager()
{

}

veFrameBufferObjectManager::~veFrameBufferObjectManager()
{
}

veFrameBufferObjectManager* veFrameBufferObjectManager::instance()
{
	static veFrameBufferObjectManager fboManager;
	return &fboManager;
}

veFrameBufferObject* veFrameBufferObjectManager::findFrameBufferObject(const std::string &name)
{
	for (auto &iter : _fbos) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veFrameBufferObject* veFrameBufferObjectManager::createFrameBufferObject(const std::string &name)
{
	auto fbo = new veFrameBufferObject;
	fbo->setName(name);
	_fbos.push_back(fbo);
	return fbo;
}

veFrameBufferObject* veFrameBufferObjectManager::getFrameBufferObject(unsigned int idx)
{
	veAssert(idx < _fbos.size());
	return _fbos[idx].get();
}

veFrameBufferObject* veFrameBufferObject::CURRENT_FBO = nullptr;

veFrameBufferObject::veFrameBufferObject()
	: USE_VE_PTR_INIT
    , _currentrbo(0)
    , _currentfbo(0)
	, _fbo(0)
	, _dsbo(0)
	, _size(0, 0)
	, _needRefreshAttachments(true)
	, _needRefreshBuffers(true)
{

}

veFrameBufferObject::veFrameBufferObject(const veVec2 &size)
	: USE_VE_PTR_INIT
    , _currentrbo(0)
    , _currentfbo(0)
	, _fbo(0)
	, _dsbo(0)
	, _target(GL_FRAMEBUFFER)
	, _size(size)
	, _needRefreshAttachments(true)
	, _needRefreshBuffers(true)
{

}

veFrameBufferObject::~veFrameBufferObject()
{
	if (_dsbo)
		glDeleteRenderbuffers(1, &_dsbo);
	if (_fbo)
		glDeleteFramebuffers(1, &_fbo);
}

void veFrameBufferObject::setFrameBufferSize(const veVec2 &size)
{
	if (size == _size) return;
	_size = size;
	_needRefreshBuffers = true;
	_needRefreshAttachments = true;
}

void veFrameBufferObject::attach(GLenum attachment, GLenum target, veTexture *attachTex, GLint layer, bool needMipmap)
{
	if (_attachments[attachment].target == target
		&& _attachments[attachment].texture == attachTex
		&& _attachments[attachment].needMipmap == needMipmap)
		return;
	_attachments[attachment] = AttachmentInfo{target, layer, attachTex, needMipmap};
	_needRefreshAttachments = true;
}

void veFrameBufferObject::bind(unsigned int clearMask, GLenum target)
{
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &_currentrbo);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_currentfbo);
	_target = target;
	refreshBuffers(clearMask);
	refreshAttachments();
}

void veFrameBufferObject::unBind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, _currentrbo);
    glBindFramebuffer(_target, _currentfbo);
    for (auto &iter : _attachments) {
        if (iter.second.needMipmap) {
            iter.second.texture->generateMipMaps();
        }
    }
}

void veFrameBufferObject::refreshBuffers(unsigned int clearMask)
{
	bool needRenderBuffer = false;
	if (_needRefreshBuffers) {
		if (_dsbo)
			glDeleteRenderbuffers(1, &_dsbo);
		if (_fbo)
			glDeleteFramebuffers(1, &_fbo);
		_dsbo = _fbo = 0;

		auto depth = _attachments.find(GL_DEPTH_ATTACHMENT);
		auto depthAndstencil = _attachments.find(GL_DEPTH_STENCIL_ATTACHMENT);
		auto stencil = _attachments.find(GL_STENCIL_BUFFER_BIT);
		if (depth == _attachments.end() && depthAndstencil == _attachments.end() && stencil == _attachments.end()) {
			if ((clearMask & GL_DEPTH_BUFFER_BIT) || (clearMask & GL_STENCIL_BUFFER_BIT)) {
				needRenderBuffer = true;
			}
		}
		_needRefreshBuffers = false;
	}

	if (!_fbo) {
		glGenFramebuffers(1, &_fbo);
	}
	glBindFramebuffer(_target, _fbo);

	if (needRenderBuffer){
		if (!_dsbo) {
			bool hasDepthBuffer = (clearMask & GL_DEPTH_BUFFER_BIT) != 0;
			bool hasStencilBuffer = (clearMask & GL_STENCIL_BUFFER_BIT) != 0;
			if (hasDepthBuffer || hasStencilBuffer) {
				glGenRenderbuffers(1, &_dsbo);
			}

			if (_dsbo) {
				glBindRenderbuffer(GL_RENDERBUFFER, _dsbo);
				if (hasDepthBuffer && !hasStencilBuffer)
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _size.x(), _size.y());
				else
					glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.x(), _size.y());

				if (hasDepthBuffer)
					glFramebufferRenderbuffer(_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _dsbo);
				if (hasStencilBuffer)
					glFramebufferRenderbuffer(_target, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _dsbo);
			}
		}
		glBindRenderbuffer(GL_RENDERBUFFER, _dsbo);
	}
	//for (auto &iter : _attachments) {
	//	float clearColorZero[4] = { 0.f, 0.f, 0.f, 0.f };
	//	glClearBufferfv(GL_COLOR, iter.first - GL_COLOR_ATTACHMENT0, clearColorZero);
	//	float clearDepthOne[4] = { 1.f, 1.f, 1.f, 1.f };
	//	glClearBufferfv(GL_DEPTH, iter.first - GL_COLOR_ATTACHMENT0, clearDepthOne);
	//}
}

void veFrameBufferObject::refreshAttachments()
{
	if (_needRefreshAttachments) {
		std::vector<GLenum> mrt;
		for (auto &iter : _attachments) {
			if (iter.second.texture.valid()) {
				if (iter.first >= GL_COLOR_ATTACHMENT0 && iter.first <= GL_COLOR_ATTACHMENT15)
					mrt.push_back(iter.first);
				//iter.second->storage(iter.second->getWidth(), iter.second->getHeight(), 1
				//	, iter.second->getInternalFormat(), iter.second->getPixelFormat(), iter.second->getDataType(), nullptr);
				iter.second.texture->bind();
				if (iter.second.layer < 0)
					glFramebufferTexture2D(_target, iter.first, iter.second.target, iter.second.texture->glTex(), 0);
				else
					glFramebufferTextureLayer(_target, iter.first, iter.second.texture->glTex(), 0, iter.second.layer);
			}
			else {
				if (iter.second.layer < 0)
					glFramebufferTexture2D(_target, iter.first, 0, 0, 0);
				else
					glFramebufferTextureLayer(_target, iter.first, 0, 0, 0);
			}
		}
		if (!mrt.empty())
			glDrawBuffers(GLsizei(mrt.size()), &mrt[0]);
		else {
			GLenum bufs = { GL_NONE };
			glDrawBuffers(1, &bufs);
		}

//		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//		if (status == GL_FRAMEBUFFER_COMPLETE) {
//			veLog("GL_FRAMEBUFFER_COMPLETE\n");
//		}
//		else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
//			veLog("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
//		}
//		else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
//			veLog("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT\n");
//		}
//		else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
//			veLog("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT\n");
//		}
//		else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
//			veLog("GL_FRAMEBUFFER_UNSUPPORTED\n");
//		}
//
//		int ec = glGetError();
//		if (ec != GL_NO_ERROR) {
//			veLog("veFrameBufferObject: GL ERROR CODE: 0x%x\n", ec);
//		}
		_needRefreshAttachments = false;
	}
}
