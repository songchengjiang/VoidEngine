#ifndef _VE_FRAME_BUFFER_OBJECT_
#define _VE_FRAME_BUFFER_OBJECT_
#include "Prerequisites.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector4.h"
#include "Texture.h"
#include <map>

class VE_EXPORT veFrameBufferObject
{
public:
	static veFrameBufferObject* CURRENT_FBO;

	veFrameBufferObject();
	~veFrameBufferObject();

	void setSize(const veVec2 &size);
	void attach(GLenum attchment);
	void detach(GLenum attchment);
	veTexture* attachTexture(GLenum attchment);

	void setClearColor(const veVec4 &color) { _clearColor = color; }
	const veVec4& getClearColor() const { return _clearColor; }
	void setClearMask(unsigned int mask) { _clearMask = mask; }
	unsigned int getClearMask() const { return _clearMask; }

	void bind();
	void unBind();

private:

	void attachToFBO();

private:
	GLuint _fbo;
	GLuint _dsbo;
	unsigned int _clearMask;
	veVec4 _clearColor;
	veVec2 _size;
	bool _needRefresh;
	std::map<GLenum, VE_Ptr<veTexture> > _attachments;
	std::vector< VE_Ptr<veTexture> >     _texturePool;
};

class VE_EXPORT veFrameBufferObjectManager
{
public:

	~veFrameBufferObjectManager();
	veFrameBufferObjectManager* instance();

	veFrameBufferObject* getOrCreateFrameBufferObject(unsigned int id);

private:

	veFrameBufferObjectManager();

private:

	std::vector<veFrameBufferObject *> _fbos;
};

#endif