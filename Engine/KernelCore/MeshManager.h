#ifndef _VE_MESH_MANAGER_
#define _VE_MESH_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Mesh.h"


class VE_EXPORT veMeshManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veMeshManager();

	virtual void resourceRecovery() override;

	veMesh* findMesh(const std::string &name);
	veMesh* createMesh(const std::string &name);

	static std::string TYPE() { return "MESH"; }

private:

	veMeshManager(veSceneManager *sm);
	std::vector< VE_Ptr<veMesh> > _meshPool;
};


#endif