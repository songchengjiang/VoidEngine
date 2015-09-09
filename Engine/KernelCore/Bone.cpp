#include "Bone.h"

veBone::veBone()
	: USE_VE_PTR_INIT
{

}

veBone::~veBone()
{

}

void veBone::setWeight(unsigned int vertexIndex, float weight)
{
	_weights[vertexIndex] = weight;
}
