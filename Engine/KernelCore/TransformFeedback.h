#ifndef _VE_TRANSFORM_FEEDBACK_
#define _VE_TRANSFORM_FEEDBACK_
#include "Prerequisites.h"

class VE_EXPORT veTransformFeedback
{
public:
	veTransformFeedback();
	~veTransformFeedback();

	USE_VE_PTR;

	void addVarying(const char *name);
	void removeVarying(size_t idx);
	size_t getVaryingCount() const { return _tfVaryingList.size(); }

	void applyTransformFeedback(GLuint program);
	void bind(GLenum primitiveMode);
	void unBind();

	void setBufferSize(GLenum target, unsigned int sizeInByte);
	unsigned int getBufferSize() const { return _bufferSizeInByte; }
	void* mapingBuffer(GLenum access = GL_READ_ONLY);
	void unMapingBuffer();

	void setRasterizerDiscard(bool isDiscard) { _rasterizerDiscard = isDiscard; }

private:

	std::vector<char *>      _tfVaryingList;
	GLuint                   _buffer;
	GLenum                   _bufferTarget;
	unsigned int             _bufferSizeInByte;
	bool                     _needRefreshBuffer;
	bool                     _rasterizerDiscard;
};

#endif