#ifndef _VE_LIGHT_
#define _VE_LIGHT_
#include "Prerequisites.h"
#include "Node.h"

class VE_EXPORT veLight : public veNode
{
public:
	veLight();
	~veLight();

	void setColor(const veVec4 &color) { _color = color; }
	const veVec4& getColor() const { return _color; }

	virtual void visit(veNodeVisitor &visitor) override;

protected:

	veVec4 _color;
};

#endif