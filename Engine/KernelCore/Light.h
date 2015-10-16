#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Node.h"
#include "Parameter.h"

class VE_EXPORT veLight : public veNode
{
	friend class veLightManager;
	friend class vePass;
public:

	~veLight();

	virtual void visit(veNodeVisitor &visitor) override;
	//virtual void render(veCamera *camera) override;

	const std::string& getType() const { return _type; }
	const veParameterList& getParameters() const { return _parameters; }
	veParameter* getParameter(const std::string &name);

	veMat4 getLightViewMatrix();

protected:
	veLight(const std::string &type, const veParameterList &params);

	virtual void updateSceneManager() override;

protected:

	std::string _type;
	veParameterList _parameters;
	veCamera *_camera;
};

#endif