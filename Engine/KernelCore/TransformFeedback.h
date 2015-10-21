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
	const std::vector<char *>& getVaryingList() const { return _tfVaryingList; }

	void bind(GLuint buffer, GLenum primitiveMode);
	void unBind();

	void setRasterizerDiscard(bool isDiscard) { _rasterizerDiscard = isDiscard; }

private:

	std::vector<char *>      _tfVaryingList;
	std::vector<GLuint>      _buffers;
	bool                     _rasterizerDiscard;
	GLuint                   _tfBuffer;
};

#endif