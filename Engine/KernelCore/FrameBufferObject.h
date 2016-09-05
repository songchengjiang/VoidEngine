#ifndef _VE_FRAME_BUFFER_OBJECT_
#define _VE_FRAME_BUFFER_OBJECT_
#include "Prerequisites.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector4.h"
#include "BaseManager.h"
#include "GLDataBuffer.h"
#include "Texture.h"
#include <map>

class VE_EXPORT veFrameBufferObject
{
	friend class veFrameBufferObjectManager;
public:
	static veFrameBufferObject* CURRENT_FBO;
	~veFrameBufferObject();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void setFrameBufferSize(const veVec2 &size);
	void attach(GLenum attachment, GLenum target, veTexture *attachTex, GLint layer = -1, bool needMipmap = false);
	void attach(GLenum attachment, GLenum target, GLint texID, GLint layer = -1, bool needMipmap = false);

	void setMultisamplesLevel(int samples);

	void bind(unsigned int contextID, unsigned int clearMask, GLenum target = GL_FRAMEBUFFER);
	void unBind();

private:

	veFrameBufferObject();
	veFrameBufferObject(const veVec2 &size);

	void refreshBuffers(unsigned int contextID, unsigned int clearMask);
	void refreshAttachments(unsigned int contextID);

private:

	struct AttachmentInfo
	{
		GLenum target;
		GLint layer;
		GLint texID;
		VE_Ptr<veTexture> texture;
		bool   needMipmap;
	};
    GLint  _currentrbo;
    GLint  _currentfbo;
    VE_Ptr<veGLDataBuffer> _fboBuffer;
    VE_Ptr<veGLDataBuffer> _dsboBuffer;
	GLenum _target;
	veVec2 _size;
	int    _multisamples;
	bool _needRefreshAttachments;
	bool _needRefreshBuffers;
	std::map<GLenum, AttachmentInfo> _attachments;
};

#endif