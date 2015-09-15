#include "Light.h"
#include "NodeVisitor.h"
#include <sstream>

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

void veLight::addParameter(veUniform *parameter)
{
	_parameters.push_back(parameter);
}

veUniform* veLight::getParameter(unsigned int idx)
{
	veAssert(idx < _parameters.size());
	return _parameters[idx].get();
}

veUniform* veLight::removeParameter(unsigned int idx)
{
	veAssert(idx < _parameters.size());
	auto parameter = _parameters[idx];
	_parameters.erase(_parameters.begin() + idx);
	return parameter.get();
}

std::string veLight::getDefination()
{
	std::stringstream ss;

	ss << "struct " << "ve_" << _name << "{" << std::endl;
	for (auto &iter : _parameters) {
		switch (iter->getType())
		{
		case veUniform::INT:
			break;

		case veUniform::BOOL:
			break;

		case veUniform::REAL:
			break;

		case veUniform::VEC2:
			break;

		case veUniform::VEC3:
			break;

		case veUniform::VEC4:
			break;

		case veUniform::MAT3:
			break;

		case veUniform::MAT4:
			break;
		default:
			break;
		}
	}
	ss << "};"<<std::endl;

	return ss.str();
}

void veLight::apply(const veRenderCommand &command)
{
	for (auto &iter : _parameters) {
		iter->apply(command);
	}
}
