#ifndef _VE_ENTITY_
#define _VE_ENTITY_
#include "Prerequisites.h"
#include "BaseCore/Array.h"
#include "RenderableObject.h"
#include "Animation.h"
#include "Mesh.h"
#include "MeshNode.h"
#include "VE_Ptr.h"

class VE_EXPORT veEntity : public veRenderableObject
{
	friend class veEntityManager;
public:
	typedef std::vector< VE_Ptr<veMesh> > MeshList;

	virtual ~veEntity();

	virtual void update(veNode *node, veSceneManager *sm) override;
	virtual bool intersectWith(veRay *ray, veNode *node) override;

	bool& needRefresh();

	int addMesh(veMesh *mesh);
	bool removeMesh(veMesh *mesh);
	veMesh* removeMesh(unsigned int mIndex);
	veMesh* getMesh(unsigned int mIndex);
	size_t getMeshCount() const { return _meshList.size(); }

	void setRootMeshNode(veMeshNode *meshNode) { _meshNodeRoot = meshNode; }
	veMeshNode* getRootMeshNode() { return _meshNodeRoot.get(); }

	void setAnimationContainer(veAnimationContainer *animContainer) { _animationContainer = animContainer; }
	veAnimationContainer* getAnimationContainer() { return _animationContainer.get(); }

	void dirtyBoundingBox();

protected:

	veEntity();
	void refreshMeshes();

protected:

	MeshList           _meshList;
	VE_Ptr<veMeshNode> _meshNodeRoot;
	VE_Ptr<veAnimationContainer> _animationContainer;
	bool               _needRefresh;
};

#endif