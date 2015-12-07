#include "FrameBufferObject.h"

veFrameBufferObjectManager::veFrameBufferObjectManager()
{

}

veFrameBufferObjectManager::~veFrameBufferObjectManager()
{
	for (auto &iter : _fbos) {
		delete iter;
	}
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
			return iter;
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
	return _fbos[idx];
}

veFrameBufferObject* veFrameBufferObject::CURRENT_FBO = nullptr;

veFrameBufferObject::veFrameBufferObject()
	: USE_VE_PTR_INIT
	, _fbo(0)
	, _dsbo(0)
	, _size(0, 0)
	, _needRefreshAttachments(true)
	, _needRefreshBuffers(true)
{

}

veFrameBufferObject::veFrameBufferObject(const veVec2 &size)
	: USE_VE_PTR_INIT
	, _fbo(0)
	, _dsbo(0)
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

void veFrameBufferObject::bind(unsigned int clearMask)
{
	if (CURRENT_FBO == this) return;
	GLenum bufs = {GL_NONE};
	glDrawBuffers(1, &bufs);
	refreshBuffers(clearMask);
	refreshAttachments();
	CURRENT_FBO = this;
}

void veFrameBufferObject::unBind()
{
	if (CURRENT_FBO == nullptr) return;
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLenum bufs = {GL_BACK};
	glDrawBuffers(1, &bufs);
	CURRENT_FBO = nullptr;
	for (auto &iter : _attachments) {
		if (iter.second.needMipmap) {
			iter.second.texture->generateMipMaps();
		}
	}
}

void veFrameBufferObject::refreshBuffers(unsigned int clearMask)
{
	if (_needRefreshBuffers) {
		if (_dsbo)
			glDeleteRenderbuffers(1, &_dsbo);
		if (_fbo)
			glDeleteFramebuffers(1, &_fbo);
		_dsbo = _fbo = 0;
		_needRefreshBuffers = false;
	}

	if (!_fbo) {
		glGenFramebuffers(1, &_fbo);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	if (!_dsbo) {
		bool hasDepthBuffer = (clearMask & GL_DEPTH_BUFFER_BIT) != 0;
		bool hasStencilBuffer = (clearMask & GL_STENCIL_BUFFER_BIT) != 0;
		if (hasDepthBuffer || hasStencilBuffer) {
			glGenRenderbuffers(1, &_dsbo);
		}

		if (_dsbo) {
			glBindRenderbuffer(GL_RENDERBUFFER, _dsbo);
			if (hasDepthBuffer && !hasStencilBuffer)
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _size.x() * VE_DEVICE_PIXEL_RATIO, _size.y() * VE_DEVICE_PIXEL_RATIO);
			else
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.x() * VE_DEVICE_PIXEL_RATIO, _size.y() * VE_DEVICE_PIXEL_RATIO);

			if (hasDepthBuffer)
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _dsbo);
			if (hasStencilBuffer)
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _dsbo);
		}
	}
	glBindRenderbuffer(GL_RENDERBUFFER, _dsbo);

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
				iter.second.texture->bind(0);
				if (iter.second.layer < 0)
					glFramebufferTexture2D(GL_FRAMEBUFFER, iter.first, iter.second.target, iter.second.texture->glTex(), 0);
				else
					glFramebufferTextureLayer(GL_FRAMEBUFFER, iter.first, iter.second.texture->glTex(), 0, iter.second.layer);		
			}
			else {
				if (iter.second.layer < 0)
					glFramebufferTexture2D(GL_FRAMEBUFFER, iter.first, 0, 0, 0);
				else
					glFramebufferTextureLayer(GL_FRAMEBUFFER, iter.first, 0, 0, 0);
			}
		}
		if (!mrt.empty())
			glDrawBuffers(GLsizei(mrt.size()), &mrt[0]);

		//GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		//if (status == GL_FRAMEBUFFER_COMPLETE) {
		//	veLog("GL_FRAMEBUFFER_COMPLETE");
		//}
		//else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
		//	veLog("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		//}
		//else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
		//	veLog("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		//}
		//else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
		//	veLog("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
		//}
		//else if (status == GL_FRAMEBUFFER_UNSUPPORTED) {
		//	veLog("GL_FRAMEBUFFER_UNSUPPORTED");
		//}

		//int ec = glGetError();
		//if (ec != GL_NO_ERROR) {
		//	veLog("veFrameBufferObject: GL ERROR CODE: %d", ec);
		//}
		_needRefreshAttachments = false;
	}
}
