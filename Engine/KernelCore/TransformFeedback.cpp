#include "TransformFeedback.h"

veTransformFeedback::veTransformFeedback()
	: USE_VE_PTR_INIT
	, _needRefreshBuffer(true)
	, _bufferSizeInByte(0)
	, _buffer(0)
	, _rasterizerDiscard(false)
	, _bufferTarget(GL_ARRAY_BUFFER)
{

}

veTransformFeedback::~veTransformFeedback()
{
	for (auto &iter : _tfVaryingList) {
		delete[] iter;
	}
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

void veTransformFeedback::applyTransformFeedback(GLuint program)
{
	glTransformFeedbackVaryings(program, _tfVaryingList.size(), &_tfVaryingList[0], GL_INTERLEAVED_ATTRIBS);
}

void veTransformFeedback::bind(GLenum primitiveMode)
{
	if (!_buffer) {
		glGenBuffers(1, &_buffer);
	}
	if (!_buffer) return;
	if (_needRefreshBuffer) {
		glBindBuffer(_bufferTarget, _buffer);
		glBufferStorage(_bufferTarget, _bufferSizeInByte, nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
		_needRefreshBuffer = false;
	}
	if (_rasterizerDiscard)
		glEnable(GL_RASTERIZER_DISCARD);

	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, _buffer);
	glBeginTransformFeedback(primitiveMode);
}

void veTransformFeedback::unBind()
{
	if (_rasterizerDiscard)
		glDisable(GL_RASTERIZER_DISCARD);
	glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
	glEndTransformFeedback();
}

void veTransformFeedback::setBufferSize(GLenum target, unsigned int sizeInByte)
{
	_bufferTarget = target;
	_bufferSizeInByte = sizeInByte;
	_needRefreshBuffer = true;
}

void* veTransformFeedback::mapingBuffer(GLenum access)
{
	glBindBuffer(_bufferTarget, _buffer);
	return glMapBuffer(_bufferTarget, access);
}

void veTransformFeedback::unMapingBuffer()
{
	glUnmapBuffer(_bufferTarget);
	glBindBuffer(_bufferTarget, 0);
}
