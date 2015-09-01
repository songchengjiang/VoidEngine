#include "FrameBufferObject.h"

veFrameBufferObjectManager::veFrameBufferObjectManager()
{

}

veFrameBufferObject* veFrameBufferObjectManager::findfbo(const std::string &name)
{
	for (auto &iter : _fbos) {
		if (iter->getName() == name)
			return iter;
	}

	return nullptr;
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

veFrameBufferObject* veFrameBufferObjectManager::getOrCreateFrameBufferObject(const std::string &name)
{
	auto fbo = findfbo(name);
	if (!fbo) {
		fbo = new veFrameBufferObject;
		fbo->setName(name);
		_fbos.push_back(fbo);
	}
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
	, _size(512, 512)
	, _clearColor(veVec4::WHITE)
	, _clearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
	, _needRefresh(true)
{

}

veFrameBufferObject::~veFrameBufferObject()
{

}

void veFrameBufferObject::setSize(const veVec2 &size)
{
	if (size == _size) return;
	_size = size;
	_needRefresh = true;
}

veTexture* veFrameBufferObject::attach(GLenum attchment)
{
	auto iter = _attachments.find(attchment);
	if (iter != _attachments.end() && iter->second.valid()) 
		return iter->second.get();

	veTexture *texture = nullptr;
	if (!_texturePool.empty()) {
		texture = _texturePool.back().get();
		_texturePool.pop_back();
	}
	else {
		texture = new veTexture2D;
	}
	_attachments[attchment] = texture;
	_needRefresh = true;
	return texture;
}

void veFrameBufferObject::detach(GLenum attchment)
{
	auto iter = _attachments.find(attchment);
	if (iter == _attachments.end()) return;
	_texturePool.push_back(iter->second);
	iter->second = nullptr;
	_needRefresh = true;
}

void veFrameBufferObject::bind()
{
	if (CURRENT_FBO == this) return;
	refreshBuffers();
	refreshAttachments();
	glClear(_clearMask);
	glClearColor(_clearColor.r(), _clearColor.g(), _clearColor.b(), _clearColor.a());
	CURRENT_FBO = this;
}

void veFrameBufferObject::unBind()
{
	if (CURRENT_FBO == nullptr) return;
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CURRENT_FBO = nullptr;
}

void veFrameBufferObject::refreshBuffers()
{
	if (!_fbo) {
		glGenFramebuffers(1, &_fbo);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	if (!_dsbo) {
		bool hasDepthBuffer = (_clearMask & GL_DEPTH_BUFFER_BIT) != 0;
		bool hasStencilBuffer = (_clearMask & GL_STENCIL_BUFFER_BIT) != 0;
		if (hasDepthBuffer || hasStencilBuffer) {
			glGenRenderbuffers(1, &_dsbo);
		}

		if (_dsbo) {
			glBindRenderbuffer(GL_RENDERBUFFER, _dsbo);
			if (hasDepthBuffer && !hasStencilBuffer)
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _size.x(), _size.y());
			else
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.x(), _size.y());

			if (hasDepthBuffer)
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _dsbo);
			if (hasStencilBuffer)
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _dsbo);
		}
	}
	glBindRenderbuffer(GL_RENDERBUFFER, _dsbo);
}

void veFrameBufferObject::refreshAttachments()
{
	if (_needRefresh) {

		std::vector<GLenum> mrt;
		for (auto &iter : _attachments) {
			if (iter.second.valid()) {
				if (iter.first >= GL_COLOR_ATTACHMENT0 && iter.first <= GL_COLOR_ATTACHMENT15)
					mrt.push_back(iter.first);
				glFramebufferTexture(GL_FRAMEBUFFER, iter.first, iter.second->glTex(), 0);
			}
			else {
				glFramebufferTexture(GL_FRAMEBUFFER, iter.first, 0, 0);
			}
		}
		if (!mrt.empty())
			glDrawBuffers(mrt.size(), &mrt[0]);

		_needRefresh = false;
	}
}
