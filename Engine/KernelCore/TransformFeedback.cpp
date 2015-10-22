#include "TransformFeedback.h"

veTransformFeedback::veTransformFeedback()
	: USE_VE_PTR_INIT
	, _tfBuffer(0)
	, _rasterizerDiscard(false)
{

}

veTransformFeedback::~veTransformFeedback()
{
	for (auto &iter : _tfVaryingList) {
		delete[] iter;
	}
	glDeleteTransformFeedbacks(1, &_tfBuffer);
}

void veTransformFeedback::addVarying(const char *name)
{
	char *str = new char[64];
	strcpy(str, name);
	_tfVaryingList.push_back(str);
}

void veTransformFeedback::removeVarying(size_t idx)
{
	veAssert(idx < _tfVaryingList.size());
	delete[] _tfVaryingList[idx];
	_tfVaryingList.erase(_tfVaryingList.begin() + idx);
}

void veTransformFeedback::bind(GLuint buffer, GLenum primitiveMode)
{
	if (!_tfBuffer) {
		glGenTransformFeedbacks(1, &_tfBuffer);
	}

	if (_rasterizerDiscard)
		glEnable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, _tfBuffer);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer);
	glBeginTransformFeedback(primitiveMode);
}

void veTransformFeedback::unBind()
{
	if (_rasterizerDiscard)
		glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
	glEndTransformFeedback();
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}