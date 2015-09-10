#include "Renderer.h"
#include "RenderableObject.h"
#include "Shader.h"
#include "Constants.h"
#include "Node.h"
#include "Camera.h"

veRenderer::veRenderer()
	: USE_VE_PTR_INIT
	, _technique(nullptr)
{
}

veRenderer::~veRenderer()
{
}

void veRenderer::visit(veNode *node, veRenderableObject *renderableObj, veVisualiser *vs)
{
	auto material = renderableObj->getMaterial();
	if (material)
		_technique = findOptimalTechnique(material);
}

void veRenderer::uniformUpdate(veUniform *uniform, const veRenderCommand &command)
{
	std::string autoBinding;
	uniform->getValue(autoBinding);
	if (!autoBinding.empty()) {
		veMat4 m = command.attachedNode->getNodeToWorldMatrix();
		veMat4 mv = command.camera->viewMatrix() * m;
		if (autoBinding == MVP_MATRIX) {
			veMat4 mvp = command.camera->projectionMatrix() * mv;
			uniform->setValue(mvp);
		}
		else if (autoBinding == MV_MATRIX) {
			uniform->setValue(mv);
		}
		else if (autoBinding == P_MATRIX) {
			const veMat4 &p = command.camera->projectionMatrix();
			uniform->setValue(p);
		}
		else if (autoBinding == NORMAL_MATRIX) {
			veMat3 normMat(mv[0][0], mv[0][1], mv[0][2]
				, mv[1][0], mv[1][1], mv[1][2]
				, mv[2][0], mv[2][1], mv[2][2]);
			normMat.inverse();
			normMat.transpose();
			uniform->setValue(normMat);
		}
		else if (autoBinding == M_MATRIX) {
			uniform->setValue(m);
		}

		//uniform->setValue(values);
	}
}

veTechnique* veRenderer::findOptimalTechnique(veMaterial *material)
{
	return material->activeTechnique();
}
