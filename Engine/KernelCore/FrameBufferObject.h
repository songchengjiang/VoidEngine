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
	veFrameBufferObject(const veVec2 &size);
	~veFrameBufferObject();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void setFrameBufferSize(const veVec2 &size);
	veTexture* attach(GLenum attchment);
	void detach(GLenum attchment);

	void bind();
	static void unBind();

private:

	void refreshBuffers();
	void refreshAttachments();

private:
	GLuint _fbo;
	GLuint _dsbo;
	veVec2 _size;
	bool _needRefresh;
	std::map<GLenum, VE_Ptr<veTexture>> _attachments;
	std::vector< VE_Ptr<veTexture> >     _texturePool;
};

class VE_EXPORT veFrameBufferObjectManager
{
public:

	~veFrameBufferObjectManager();
	static veFrameBufferObjectManager* instance();

	veFrameBufferObject* getOrCreateFrameBufferObject(const std::string &name);
	veFrameBufferObject* getFrameBufferObject(unsigned int idx);
	unsigned int getFrameBufferObjectNum() const { return _fbos.size(); }


private:

	veFrameBufferObjectManager();
	veFrameBufferObject* findfbo(const std::string &name);

private:

	std::vector<veFrameBufferObject *> _fbos;
};

#endif