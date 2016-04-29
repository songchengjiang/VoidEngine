#ifndef _VE_BONE_
#define _VE_BONE_
#include "Prerequisites.h"
#include "BaseCore/Matrix4.h"
#include <unordered_map>
#include "BoudingBox.h"

class veMeshNode;
class VE_EXPORT veBone
{
public:
	veBone();
	~veBone();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void setOffsetMat(const veMat4 &offsetMat) { _offsetMat = offsetMat; }
	const veMat4& getOffsetMat() const { return _offsetMat; }

	//void setWeight(unsigned int vertexIndex, float weight);
	//float getWeight(unsigned int vertexIndex) { return _weights[vertexIndex]; };
	//const std::unordered_map<unsigned int, float>& getWeights() const { return _weights; }

	void setBoneNode(veMeshNode *node) { _boneNode = node; }
	veMeshNode* getBoneNode() { return _boneNode; }
	const veMeshNode* getBoneNode() const { return _boneNode; }

	void setBoundingBox(const veBoundingBox &bbox) { _boundingBox = bbox; }
	const veBoundingBox& getBoundingBox() const { return _boundingBox; }

	void setBindPosBoundingBox(const veBoundingBox &bbox) { _bindPosBoundingBox = bbox; }
	const veBoundingBox& getBindPosBoundingBox() const { return _bindPosBoundingBox; }

private:

	veMat4 _offsetMat;
	//std::unordered_map<unsigned int, float> _weights;
	veMeshNode *_boneNode;
	veBoundingBox      _boundingBox;
	veBoundingBox      _bindPosBoundingBox;
};

#endif