#ifndef _VE_LIGHT_MANAGER_
#define _VE_LIGHT_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Light.h"

class VE_EXPORT veLightManager : public veBaseManager
{
	friend class veSceneManager;
public:

	~veLightManager();

	veLight* findLight(const std::string &name);
	veLight* createLight(veLight::LightType type, const std::string &name);

	static std::string TYPE() { return "LIGHT"; }

private:
	veLightManager(veSceneManager *sm);

private:

	std::vector< VE_Ptr<veLight> > _lightPool;
};

#endif