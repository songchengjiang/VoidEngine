#ifndef _VE_LIGHT_MANAGER_
#define _VE_LIGHT_MANAGER_
#include "Prerequisites.h"
#include "BaseManager.h"
#include "Light.h"

class VE_EXPORT veLightManager : public veBaseManager
{
	friend class veSceneManager;
public:
	typedef std::vector< std::pair<std::string, std::string> > Parameters;
	struct LightTemplate
	{
		unsigned int limit;
		Parameters parameters;
	};
	typedef std::unordered_map<std::string, LightTemplate> LightTemplateList;

	~veLightManager();

	bool loadLightTemplates(const std::string &filePath);
	const LightTemplateList& getLightTemplateList() const { return _lightTemplate; }

	veLight* findLight(const std::string &name);
	veLight* createLight(const std::string &type, const std::string &name);

	static std::string TYPE() { return "LIGHT"; }

private:
	veLightManager();

private:

	LightTemplateList _lightTemplate;
	std::vector< VE_Ptr<veLight> > _lightPool;
};

#endif