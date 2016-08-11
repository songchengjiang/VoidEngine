#ifndef _VE_TRANSFORM_FEEDBACK_
#define _VE_TRANSFORM_FEEDBACK_
#include "Prerequisites.h"
#include "GLDataBuffer.h"

class VE_EXPORT veTransformFeedback
{
public:
	veTransformFeedback();
	~veTransformFeedback();

	USE_VE_PTR;

	void addVarying(const char *name);
	void removeVarying(size_t idx);
	size_t getVaryingCount() const { return _tfVaryingList.size(); }
	const std::vector<char *>& getVaryingList() const { return _tfVaryingList; }

	void bind(unsigned int contextID, GLuint buffer, GLsizeiptr bufSize, GLenum primitiveMode);
	void unBind();

	void setRasterizerDiscard(bool isDiscard) { _rasterizerDiscard = isDiscard; }

private:

	std::vector<char *>      _tfVaryingList;
	bool                     _rasterizerDiscard;
    VE_Ptr<veGLDataBuffer>   _tfBuffer;
};

#endif