#ifndef _VE_FRAME_BUFFER_OBJECT_
#define _VE_FRAME_BUFFER_OBJECT_
#include "Prerequisites.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector4.h"
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

	void bind(unsigned int clearMask, GLenum target = GL_FRAMEBUFFER);
	void unBind();

private:

	veFrameBufferObject();
	veFrameBufferObject(const veVec2 &size);

	void refreshBuffers(unsigned int clearMask);
	void refreshAttachments();

private:

	struct AttachmentInfo
	{
		GLenum target;
		GLint layer;
		VE_Ptr<veTexture> texture;
		bool   needMipmap;
	};
    GLint  _currentrbo;
    GLint  _currentfbo;
	GLuint _fbo;
	GLuint _dsbo;
	GLenum _target;
	veVec2 _size;
	bool _needRefreshAttachments;
	bool _needRefreshBuffers;
	std::map<GLenum, AttachmentInfo> _attachments;
};

class VE_EXPORT veFrameBufferObjectManager
{
public:

	~veFrameBufferObjectManager();
	static veFrameBufferObjectManager* instance();

	veFrameBufferObject* findFrameBufferObject(const std::string &name);
	veFrameBufferObject* createFrameBufferObject(const std::string &name);
	veFrameBufferObject* getFrameBufferObject(unsigned int idx);
	size_t getFrameBufferObjectNum() const { return _fbos.size(); }


private:

	veFrameBufferObjectManager();

private:

	std::vector< VE_Ptr<veFrameBufferObject> > _fbos;
};

#endif