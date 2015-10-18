#include "Light.h"
#include "NodeVisitor.h"

veLight::veLight(const std::string &type, const veParameterList &params)
	: _type(type)
	, _parameters(params)
	, _lightMatrix(veMat4::IDENTITY)
{
}

veLight::~veLight()
{

}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

//void veLight::render(veCamera *camera)
//{
//	veNode::render(camera);
//	_camera = nullptr;
//	if (_mask & camera->getMask())
//		_camera = camera;
//}

veParameter* veLight::getParameter(const std::string &name)
{
	for (auto &iter : _parameters) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

void veLight::updateSceneManager()
{

}