#include "Light.h"
#include "NodeVisitor.h"

veLight::veLight()
{

}

veLight::~veLight()
{

}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}
