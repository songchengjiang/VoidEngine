#include "Bone.h"
#include "Node.h"

veBone::veBone()
	: USE_VE_PTR_INIT
	, _boneNode(nullptr)
{

}

veBone::~veBone()
{

}

void veBone::setWeight(unsigned int vertexIndex, float weight)
{
	_weights[vertexIndex] = weight;
}
