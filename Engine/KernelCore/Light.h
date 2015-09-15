#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Node.h"
#include "Shader.h"
#include <unordered_map>

class VE_EXPORT veLight : public veNode
{
public:
	veLight();
	~veLight();

	virtual void visit(veNodeVisitor &visitor) override;

	void addParameter(veUniform *parameter);
	veUniform* getParameter(unsigned int idx);
	veUniform* removeParameter(unsigned int idx);
	unsigned int getParameterNum() const { return _parameters.size(); }

	std::string getDefination();

	void apply(const veRenderCommand &command);

protected:

	std::vector< VE_Ptr<veUniform> > _parameters;
};

#endif