#include "FrameBufferObject.h"

#if VE_PLATFORM == VE_PLATFORM_ANDROID
#if !defined( GL_EXT_multisampled_render_to_texture )
typedef void (GL_APIENTRY* PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (GL_APIENTRY* PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples);

PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC glRenderbufferStorageMultisampleEXT = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC)eglGetProcAddress( "glRenderbufferStorageMultisampleEXT" );
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)eglGetProcAddress( "glFramebufferTexture2DMultisampleEXT" );
#endif
#else
#if VE_PLATFORM != VE_PLATFORM_WIN32
#endif
void (*glFramebufferTexture2DMultisampleEXT)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLsizei samples) = nullptr;
#endif

veFrameBufferObject* veFrameBufferObject::CURRENT_FBO = nullptr;

veFrameBufferObject::veFrameBufferObject()
	: USE_VE_PTR_INIT
    , _currentrbo(0)
    , _currentfbo(0)
	, _size(0, 0)
	, _multisamples(0)
	, _needRefreshAttachments(true)
	, _needRefreshBuffers(true)
{
    _fboBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        return fbo;
    }, [](GLuint fbo){
        glDeleteFramebuffers(1, &fbo);
    });
    
    _dsboBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint dsbo;
        glGenRenderbuffers(1, &dsbo);
        return dsbo;
    }, [](GLuint dsbo){
        glDeleteRenderbuffers(1, &dsbo);
    });
}

veFrameBufferObject::veFrameBufferObject(const veVec2 &size)
	: USE_VE_PTR_INIT
    , _currentrbo(0)
    , _currentfbo(0)
	, _target(GL_FRAMEBUFFER)
	, _size(size)
	, _multisamples(0)
	, _needRefreshAttachments(true)
	, _needRefreshBuffers(true)
{
    _fboBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        return fbo;
    }, [](GLuint fbo){
        glDeleteFramebuffers(1, &fbo);
    });
    
    _dsboBuffer = veGLDataBufferManager::instance()->createGLDataBuffer([]() -> GLuint{
        GLuint dsbo;
        glGenRenderbuffers(1, &dsbo);
        return dsbo;
    }, [](GLuint dsbo){
        glDeleteRenderbuffers(1, &dsbo);
    });
}

veFrameBufferObject::~veFrameBufferObject()
{
}

void veFrameBufferObject::setFrameBufferSize(const veVec2 &size)
{
	if (size == _size) return;
	_size = size;
	refresh();
}

bool veFrameBufferObject::attach(GLenum attachment, GLenum target, veTexture *attachTex, GLint layer, bool needMipmap)
{
	auto iter = _attachments.find(attachment);
	if (iter != _attachments.end()) {
		if (iter->second.target == target
			&& iter->second.texture == attachTex
            && iter->second.layer == layer
			&& iter->second.needMipmap == needMipmap)
			return false;
	}
	_attachments[attachment] = AttachmentInfo{target, layer, 0, attachTex, needMipmap};
	_needRefreshAttachments = true;
//	if (attachment == GL_DEPTH_ATTACHMENT || attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
//		_needRefreshBuffers = true;
//	}
	return true;
}

bool veFrameBufferObject::attach(GLenum attachment, GLenum target, GLint texID, GLint layer, bool needMipmap)
{
	auto iter = _attachments.find(attachment);
	if (iter != _attachments.end()) {
		if (iter->second.target == target
			&& iter->second.texID == texID
            && iter->second.layer == layer
			&& iter->second.needMipmap == needMipmap)
			return false;
	}
	_attachments[attachment] = AttachmentInfo{target, layer, texID, nullptr, needMipmap};
	_needRefreshAttachments = true;
//	if (attachment == GL_DEPTH_ATTACHMENT || attachment == GL_DEPTH_STENCIL_ATTACHMENT) {
//		_needRefreshBuffers = true;
//	}
	return true;
}

void veFrameBufferObject::setMultisamplesLevel(int samples)
{
	if (_multisamples == samples)
		return;
	_multisamples = samples;
	refresh();
}

void veFrameBufferObject::refresh()
{
	_needRefreshBuffers = true;
	_needRefreshAttachments = true;
}

void veFrameBufferObject::bind(unsigned int contextID, unsigned int clearMask, GLenum target)
{
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &_currentrbo);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_currentfbo);
	_target = target;
    if (_needRefreshBuffers){
        _fboBuffer->destroyData(contextID);
        _dsboBuffer->destroyData(contextID);
    }
	refreshBuffers(contextID, clearMask);
	refreshAttachments(contextID);
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

void veFrameBufferObject::blitFramebuffer(GLbitfield mask, GLenum filter, unsigned int contextID)
{
//	GLint currentReadfbo;
//	glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &currentReadfbo);
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, _fboBuffer->getData(contextID));
//    glBlitFramebuffer(0, 0, _size.x(), _size.y(),
//                      0, 0, _size.x(), _size.y(), mask, filter);
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, currentReadfbo);
}

void veFrameBufferObject::refreshBuffers(unsigned int contextID, unsigned int clearMask)
{
    auto fbo = _fboBuffer->getData(contextID);
    if (!fbo){
        fbo = _fboBuffer->createData(contextID);
        _needRefreshBuffers = true;
        _needRefreshAttachments = true;
    }
    
	bool needRenderBuffer = false;
	if (_needRefreshBuffers) {
		auto depth = _attachments.find(GL_DEPTH_ATTACHMENT);
		//auto depthAndstencil = _attachments.find(GL_DEPTH_STENCIL_ATTACHMENT);
		auto stencil = _attachments.find(GL_STENCIL_BUFFER_BIT);
		if (depth == _attachments.end() && /* depthAndstencil == _attachments.end() && */ stencil == _attachments.end()) {
			if ((clearMask & GL_DEPTH_BUFFER_BIT) || (clearMask & GL_STENCIL_BUFFER_BIT)) {
				needRenderBuffer = true;
			}
		}
		_needRefreshBuffers = false;
	}

	glBindFramebuffer(_target, fbo);

	if (needRenderBuffer){
        auto dsbo = _dsboBuffer->getData(contextID);
		if (!dsbo) {
			bool hasDepthBuffer = (clearMask & GL_DEPTH_BUFFER_BIT) != 0;
			bool hasStencilBuffer = (clearMask & GL_STENCIL_BUFFER_BIT) != 0;
			if (hasDepthBuffer || hasStencilBuffer) {
                dsbo = _dsboBuffer->createData(contextID);
			}

			if (dsbo) {
				glBindRenderbuffer(GL_RENDERBUFFER, dsbo);
				if (hasDepthBuffer && !hasStencilBuffer)
//					if (0 < _multisamples && glRenderbufferStorageMultisampleEXT != nullptr){
//						glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, _multisamples, GL_DEPTH_COMPONENT24, _size.x(), _size.y());
//						veLog("glRenderbufferStorageMultisampleEXT");
//					}
//					else
						glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _size.x(), _size.y());
				else
//					if (0 < _multisamples && glRenderbufferStorageMultisampleEXT != nullptr)
//						glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER, _multisamples, GL_DEPTH24_STENCIL8, _size.x(), _size.y());
//					else
						glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _size.x(), _size.y());

				if (hasDepthBuffer)
					glFramebufferRenderbuffer(_target, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, dsbo);
				if (hasStencilBuffer)
					glFramebufferRenderbuffer(_target, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, dsbo);
			}
		}
		glBindRenderbuffer(GL_RENDERBUFFER, dsbo);
	}
	//for (auto &iter : _attachments) {
	//	float clearColorZero[4] = { 0.f, 0.f, 0.f, 0.f };
	//	glClearBufferfv(GL_COLOR, iter.first - GL_COLOR_ATTACHMENT0, clearColorZero);
	//	float clearDepthOne[4] = { 1.f, 1.f, 1.f, 1.f };
	//	glClearBufferfv(GL_DEPTH, iter.first - GL_COLOR_ATTACHMENT0, clearDepthOne);
	//}
}

void veFrameBufferObject::refreshAttachments(unsigned int contextID)
{
	if (_needRefreshAttachments) {
		std::vector<GLenum> mrt;
		for (auto &iter : _attachments) {
			if (iter.second.texture.valid() || 0 < iter.second.texID) {
				//if (iter.first >= GL_COLOR_ATTACHMENT0 /* && iter.first <= GL_COLOR_ATTACHMENT15 */)
				//	mrt.push_back(iter.first);
				//iter.second->storage(iter.second->getWidth(), iter.second->getHeight(), 1
				//	, iter.second->getInternalFormat(), iter.second->getPixelFormat(), iter.second->getDataType(), nullptr);
				if (iter.second.texture.valid())
					iter.second.texture->bind(contextID);
				if (iter.second.layer < 0)
				//	if (0 < _multisamples && glFramebufferTexture2DMultisampleEXT != nullptr){
				//		glFramebufferTexture2DMultisampleEXT(_target, iter.first, iter.second.target, iter.second.texture.valid()? iter.second.texture->glTex(contextID): iter.second.texID, 0, _multisamples);
				//	}
				//	elseS {
						glFramebufferTexture2D(_target, iter.first, iter.second.target, iter.second.texture.valid()? iter.second.texture->glTex(contextID): iter.second.texID, 0);
				//	}
				//else
				//	glFramebufferTextureLayer(_target, iter.first, iter.second.texture.valid()? iter.second.texture->glTex(contextID): iter.second.texID, 0, iter.second.layer);
			}
			else {
				if (iter.second.layer < 0)
				//	if (0 < _multisamples && glFramebufferTexture2DMultisampleEXT != nullptr)
				//		glFramebufferTexture2DMultisampleEXT(_target, iter.first, 0, 0, 0, 0);
				//	else
						glFramebufferTexture2D(_target, iter.first, 0, 0, 0);
				//else
				//	glFramebufferTextureLayer(_target, iter.first, 0, 0, 0);
			}
		}
//		if (!mrt.empty())
//			glDrawBuffers(GLsizei(mrt.size()), &mrt[0]);
//		else {
//			GLenum bufs = { GL_NONE };
//			glDrawBuffers(1, &bufs);
//		}

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
