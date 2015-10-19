#ifndef _VE_MESH_NODE_
#define _VE_MESH_NODE_
#include <unordered_map>
#include "Prerequisites.h"
#include "BaseCore/Array.h"
#include "Bone.h"
#include "VE_Ptr.h"

class veMesh;
class VE_EXPORT veMeshNode
{
public:
	typedef std::vector< VE_Ptr<veMeshNode> > Children;

	veMeshNode();
	virtual ~veMeshNode();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	int addChild(veMeshNode *child);
	bool removeChild(veMeshNode *child);
	veMeshNode* removeChild(unsigned int cIndex);
	veMeshNode* getChild(unsigned int cIndex);
	size_t getChildCount() const { return _children.size(); }

	void setMatrix(const veMat4 &mat);
	const veMat4& getMatrix() const { return _matrix; }

	veMat4 toMeshNodeRootMatrix();

	void addMeshRef(veMesh *mesh);
	bool removeMeshRef(veMesh *mesh);
	veMesh* removeMeshRef(unsigned int mIndex);
	veMesh* getMeshRef(size_t idx) { return _meshList[idx]; }
	size_t getMeshRefCount() { return _meshList.size(); }

	void needRefresh();

protected:

	veMat4 _matrix;
	veMat4 _worldMatrix;
	bool _needRefresh;
	std::vector<veMesh*> _meshList;

	Children _children;
	veMeshNode *_parent;
};

#endif