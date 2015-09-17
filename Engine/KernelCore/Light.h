#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Node.h"
#include "Parameter.h"
#include <unordered_map>

class VE_EXPORT veLight : public veNode
{
	friend class veLightManager;
	friend class vePass;
public:

	~veLight();

	virtual void visit(veNodeVisitor &visitor) override;

	const std::string& getType() const { return _type; }
	const veParameterList& getParameters() const { return _parameters; }
	veParameter* getParameter(const std::string &name);

protected:

	veLight(const std::string &type, const veParameterList &params);

protected:

	std::string _type;
	veParameterList _parameters;
};

typedef std::vector< veLight* > veLightList;
typedef std::unordered_map< std::string, veLightList > veLightMap;

class VE_EXPORT veLightManager
{
public:
	typedef std::vector< std::pair<std::string, std::string> > Parameters;
	struct LightTemplate
	{
		unsigned int limit;
		Parameters parameters;
	};
	typedef std::unordered_map<std::string, LightTemplate> LightTemplateList;

	~veLightManager();

	static veLightManager* instance();

	bool loadLightTemplates(const std::string &filePath);
	void addLightTemplate(const std::string &className, const LightTemplate &lightTemp);
	const LightTemplate& getLightTemplate(const std::string &className) const;
	const LightTemplateList& getLightTemplateList() const { return _lightTemplate; }

	veLight* instanceOneLight(const std::string &className);

private:
	veLightManager();

private:

	LightTemplateList _lightTemplate;
};

#endif