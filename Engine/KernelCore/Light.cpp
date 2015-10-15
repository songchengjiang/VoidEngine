#include "Light.h"
#include "NodeVisitor.h"
#include "Camera.h"

veLight::veLight(const std::string &type, const veParameterList &params)
	: _type(type)
	, _parameters(params)
	, _camera(nullptr)
{
}

veLight::~veLight()
{

}

void veLight::visit(veNodeVisitor &visitor)
{
	visitor.visit(*this);
}

void veLight::render(veCamera *camera)
{
	if (!_isVisible) return;
	if (_mask & camera->getMask()) {
		_camera = camera;
		if (!_children.empty()) {
			for (auto &child : _children) {
				child->render(camera);
			}
		}

		if (!_renderableObjects.empty()) {
			for (auto &iter : _renderableObjects) {
				iter->render(this, camera);
			}
		}
	}
}

veParameter* veLight::getParameter(const std::string &name)
{
	for (auto &iter : _parameters) {
		if (iter->getName() == name)
			return iter.get();
	}

	return nullptr;
}

veMat4 veLight::getLightViewMatrix()
{
	return _camera == nullptr ? veMat4::IDENTITY : _camera->viewMatrix() * this->getNodeToWorldMatrix();
}
