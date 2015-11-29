#ifndef _VE_MATERIAL_MANAGER_
#define _VE_MATERIAL_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Material.h"


class VE_EXPORT veMaterialManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veMaterialManager();

	virtual void update() override;
	virtual void resourceRecovery() override;

	veMaterialArray* findMaterialArray(const std::string &name);
	veMaterialArray* createMaterialArray(const std::string &name);

	static std::string TYPE() { return "MATERIAL"; }

private:

	veMaterialManager(veSceneManager *sm);
	std::vector< VE_Ptr<veMaterialArray> > _materialPool;
};


#endif