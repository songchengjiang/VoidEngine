#ifndef _VE_BONE_
#define _VE_BONE_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include <unordered_map>

class veNode;
class VE_EXPORT veBone
{
public:
	veBone();
	~veBone();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void setOffsetMat(const veMat4 &offsetMat) { _offsetMat = offsetMat; }
	const veMat4& getOffsetMat() const { return _offsetMat; }

	void setWeight(unsigned int vertexIndex, float weight);
	float getWeight(unsigned int vertexIndex) { return _weights[vertexIndex]; };
	const std::unordered_map<unsigned int, float>& getWeights() const { return _weights; }

	void setBoneNode(veNode *node) { _boneNode = node; }
	veNode* getBoneNode() { return _boneNode; }
	const veNode* getBoneNode() const { return _boneNode; }

private:

	veMat4 _offsetMat;
	std::unordered_map<unsigned int, float> _weights;
	veNode *_boneNode;
};

#endif