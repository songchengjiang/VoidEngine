#include "Debuger.h"
#include "KernelCore/RenderableObject.h"
#include "KernelCore/NodeVisitor.h"
#include "KernelCore/Node.h"
#include "KernelCore/Entity.h"
#include "KernelCore/BoudingBox.h"
#include "KernelCore/RenderCommand.h"
#include "KernelCore/RenderQueue.h"
#include "KernelCore/Camera.h"
#include "KernelCore/SceneManager.h"
#include "KernelCore/MatrixPtr.h"
#include "Constants.h"

class veDebugRenderer : public veRenderer
{
public:

	veDebugRenderer(veDebuger *debuger)
		: _debuger(debuger)
		, drawCount(0)
		, vao(0)
		, vbo(0)
	{}

	virtual void render(veNode *node, veRenderableObject *renderableObj, veCamera *camera) override {
		if (!isNeedRendering())
			return;
		if (!vao) {
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
		}

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		{
			std::unique_lock<std::mutex> lock(verticesMutex);
			if (!vertices.empty())
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.buffer(), GL_STATIC_DRAW);
			drawCount = vertices.size();
			vertices.clear();
		}


		GLsizei vertexStride = sizeof(GLfloat) * (3 + 4);
		//vertex
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexStride, 0);
		glEnableVertexAttribArray(0);

		//color
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexStride, (GLvoid *)(sizeof(GLfloat) * 3));
		glEnableVertexAttribArray(1);

		veRenderCommand rc;
		rc.priority = veRenderCommand::LOW_PRIORITY;
		rc.pass = _debuger->getMaterialArray()->getMaterial(0)->getTechnique(0)->getPass(0);
		rc.worldMatrix = new veMat4Ptr(veMat4::IDENTITY);
		rc.renderableObj = nullptr;
		rc.camera = camera;
		rc.sceneManager = camera->getSceneManager();
		rc.renderer = this;
		camera->getRenderQueue()->pushCommand(0, veRenderQueue::RENDER_QUEUE_ENTITY, rc);
	}

	virtual void draw(veRenderCommand &command) override {
		command.pass->apply(command);
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, drawCount);
	}

	std::mutex      verticesMutex;
	veRealArray     vertices;
	unsigned int    drawCount;
	GLuint          vao;
	GLuint          vbo;

private:

	veDebuger *_debuger;
};

class RenderableObjectFinder : public veNodeVisitor
{
public:
    
	RenderableObjectFinder(std::vector<veNode *> &renderableNodes)
		: _renderableNodes(renderableNodes)
	{}

	virtual void visit(veNode &node) {
		_renderableNodes.push_back(&node);
	}

private:

	std::vector<veNode *> &_renderableNodes;
};

veDebuger::veDebuger(veSceneManager *sm)
	: veRenderableObject(sm)
	, _isDrawMeshWireframe(false)
	, _isDrawBoundingBoxWireframe(false)
	, _isDrawFrustumPlane(false)
	, _needRefresh(true)
{
	_renderer = new veDebugRenderer(this);
	_renderer->setRenderStageMask(veRenderer::RENDERING);
}

veDebuger::~veDebuger()
{

}

void veDebuger::update(veNode *node, veSceneManager *sm)
{
	if (_needRefresh) {
		initMaterial(sm);
		_needRefresh = false;
	}
}

void veDebuger::render(veNode *node, veCamera *camera)
{
	//if (_isDrawBoundingBoxWireframe) {
	//	renderBoundingBoxWireframe(_attachedNode->getBoundingBox(), nTow);
	//}

	_renderableNodes.clear();
	RenderableObjectFinder finder(_renderableNodes);
	node->accept(finder);
	for (auto &iter : _renderableNodes) {
		if (_isDrawFrustumPlane) {
			auto camera = dynamic_cast<veCamera *>(iter);
			if (camera)
				renderFrustumPlanes(camera);
		}
		if (0 < iter->getRenderableObjectCount()) {
			if (_isDrawBoundingBoxWireframe)
				renderBoundingBoxWireframe(iter->getBoundingBox(), _drawBoundingBoxWireframeColor);

			veMat4 rnToNode = iter->getNodeToWorldMatrix();
			for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
				auto entity = dynamic_cast<veEntity *>(iter->getRenderableObject(i));
				if (entity) {
					if (_isDrawMeshWireframe)
						for (size_t i = 0; i < entity->getMeshCount(); ++i) {
							auto mesh = entity->getMesh(i);
							renderMeshWireframe(mesh, rnToNode * mesh->getAttachedNode()->toMeshNodeRootMatrix());
						}
					//if (_isDrawBoundingBoxWireframe)
					//	renderBoundingBoxWireframe(entity->getBoundingBox() * rnToNode);
				}
			}
		}
	}

	veRenderableObject::render(node, camera);
}

void veDebuger::debugDrawLine(const veVec3 &start, const veVec3 &end, const veVec4 &color)
{
	drawLine(start, end, color);
}

void veDebuger::initMaterial(veSceneManager *sm)
{
	const char *V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	layout(location = 1) in vec4 color; \n \
	uniform mat4 u_ModelViewProjectMat; \n \
	out vec4 v_color; \n \
	void main() \n \
	{                   \n \
	    v_color = color;               \n \
		gl_Position = u_ModelViewProjectMat * vec4(position, 1.0); \n \
	}";

	const char *F_SHADER = " \
	layout(location = 0) out vec4 fragColor; \n \
	in vec4 v_color; \n \
	void main() {  \n \
		fragColor = v_color; \n \
	}";

	_materials = sm->createMaterialArray(_name + "-matAry");
	auto material = new veMaterial;
	auto tech = new veTechnique;
	auto pass = new vePass;
	material->addTechnique(tech);
	tech->addPass(pass);

	pass->depthTest() = true;
	pass->depthWrite() = false;
	pass->cullFace() = true;
	pass->blendFunc() = veBlendFunc::ALPHA;

	auto vShader = new veShader(veShader::VERTEX_SHADER, V_SHADER);
	auto fShader = new veShader(veShader::FRAGMENT_SHADER, F_SHADER);
	pass->setShader(vShader);
	pass->setShader(fShader);

	pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));

	_materials->addMaterial(material);
}

void veDebuger::renderMeshWireframe(veMesh *mesh, const veMat4 &trans)
{
	auto va = mesh->getVertexArray();
	unsigned int offset = 0;
	unsigned int stride = 0;
	for (size_t i = 0; i < mesh->getVertexAtrributeNum(); ++i) {
		auto attr = mesh->getVertexAtrribute(i);
		if (attr.attributeType == veMesh::VertexAtrribute::VERTEX_ATTRIB_POSITION) {
			offset = stride;
		}
		if (attr.valueType == veMesh::VertexAtrribute::FLOAT) stride += sizeof(GLfloat) * attr.valueNum / sizeof(GLfloat);
		else if (attr.valueType == veMesh::VertexAtrribute::UINT) stride += sizeof(GLuint) * attr.valueNum / sizeof(GLfloat);
		else if (attr.valueType == veMesh::VertexAtrribute::USHORT) stride += sizeof(GLushort) * attr.valueNum / sizeof(GLfloat);
	}

	for (size_t p = 0; p < mesh->getPrimitiveNum(); ++p) {
		auto primitive = mesh->getPrimitive(p);
		if (primitive.primitiveType == veMesh::Primitive::LINES) {
			for (size_t i = 0; i < primitive.indices->size(); i += 2) {
				unsigned int v0ID = (*primitive.indices)[i] * stride + offset;
				unsigned int v1ID = (*primitive.indices)[i + 1] * stride + offset;
				veVec3 v0(va->buffer()[v0ID], va->buffer()[v0ID + 1], va->buffer()[v0ID + 2]);
				veVec3 v1(va->buffer()[v1ID], va->buffer()[v1ID + 1], va->buffer()[v1ID + 2]);
				v0 = trans * v0;
				v1 = trans * v1;
				drawLine(v0, v1, _drawMeshWireframeColor);
			}
		}
		else if (primitive.primitiveType == veMesh::Primitive::TRIANGLES) {
			for (size_t i = 0; i < primitive.indices->size(); i += 3) {
				unsigned int v0ID = (*primitive.indices)[i] * stride + offset;
				unsigned int v1ID = (*primitive.indices)[i + 1] * stride + offset;
				unsigned int v2ID = (*primitive.indices)[i + 2] * stride + offset;
				veVec3 v0(va->buffer()[v0ID], va->buffer()[v0ID + 1], va->buffer()[v0ID + 2]);
				veVec3 v1(va->buffer()[v1ID], va->buffer()[v1ID + 1], va->buffer()[v1ID + 2]);
				veVec3 v2(va->buffer()[v2ID], va->buffer()[v2ID + 1], va->buffer()[v2ID + 2]);
				v0 = trans * v0;
				v1 = trans * v1;
				v2 = trans * v2;
				drawLine(v0, v1, _drawMeshWireframeColor);
				drawLine(v0, v2, _drawMeshWireframeColor);
				drawLine(v1, v2, _drawMeshWireframeColor);
			}
		}
	}
}

void veDebuger::renderBoundingBoxWireframe(const veBoundingBox &bbox, const veVec4 &color)
{
	if (bbox.isNull()) return;
	const veVec3 &bbmin = bbox.min();
	const veVec3 &bbmax = bbox.max();
	veVec3 v0 = bbmin;
	veVec3 v1 = veVec3(bbmax.x(), bbmin.y(), bbmin.z());
	veVec3 v2 = veVec3(bbmax.x(), bbmax.y(), bbmin.z());
	veVec3 v3 = veVec3(bbmin.x(), bbmax.y(), bbmin.z());
	veVec3 v4 = veVec3(bbmin.x(), bbmin.y(), bbmax.z());
	veVec3 v5 = veVec3(bbmax.x(), bbmin.y(), bbmax.z());
	veVec3 v6 = bbmax;
	veVec3 v7 = veVec3(bbmin.x(), bbmax.y(), bbmax.z());

	drawLine(v0, v1, color);
	drawLine(v1, v2, color);
	drawLine(v2, v3, color);
	drawLine(v3, v0, color);

	drawLine(v4, v5, color);
	drawLine(v5, v6, color);
	drawLine(v6, v7, color);
	drawLine(v7, v4, color);

	drawLine(v0, v4, color);
	drawLine(v1, v5, color);
	drawLine(v2, v6, color);
	drawLine(v3, v7, color);
}

void veDebuger::renderFrustumPlanes(veCamera *camera)
{
	auto leftPlane = camera->getFrustumPlane(veCamera::FRUSTUM_PLANE_LEFT);
	auto rightPlane = camera->getFrustumPlane(veCamera::FRUSTUM_PLANE_RIGHT);
	auto bottomPlane = camera->getFrustumPlane(veCamera::FRUSTUM_PLANE_BOTTOM);
	auto topPlane = camera->getFrustumPlane(veCamera::FRUSTUM_PLANE_TOP);
	auto nearPlane = camera->getFrustumPlane(veCamera::FRUSTUM_PLANE_NEAR);
	auto farPlane = camera->getFrustumPlane(veCamera::FRUSTUM_PLANE_FAR);

	veVec3 near0 = getPlaneCrossPoint(nearPlane, leftPlane, bottomPlane);
	veVec3 near1 = getPlaneCrossPoint(nearPlane, rightPlane, bottomPlane);
	veVec3 near2 = getPlaneCrossPoint(nearPlane, rightPlane, topPlane);
	veVec3 near3 = getPlaneCrossPoint(nearPlane, leftPlane, topPlane);

	veVec3 far0 = getPlaneCrossPoint(farPlane, leftPlane, bottomPlane);
	veVec3 far1 = getPlaneCrossPoint(farPlane, rightPlane, bottomPlane);
	veVec3 far2 = getPlaneCrossPoint(farPlane, rightPlane, topPlane);
	veVec3 far3 = getPlaneCrossPoint(farPlane, leftPlane, topPlane);


	drawLine(near0, near1, _drawFrustumPlaneColor);
	drawLine(near1, near2, _drawFrustumPlaneColor);
	drawLine(near2, near3, _drawFrustumPlaneColor);
	drawLine(near3, near0, _drawFrustumPlaneColor);

	drawLine(far0, far1, _drawFrustumPlaneColor);
	drawLine(far1, far2, _drawFrustumPlaneColor);
	drawLine(far2, far3, _drawFrustumPlaneColor);
	drawLine(far3, far0, _drawFrustumPlaneColor);

	drawLine(near0, far0, _drawFrustumPlaneColor);
	drawLine(near1, far1, _drawFrustumPlaneColor);
	drawLine(near2, far2, _drawFrustumPlaneColor);
	drawLine(near3, far3, _drawFrustumPlaneColor);
}

veVec3 veDebuger::getPlaneCrossPoint(const vePlane &p0, const vePlane &p1, const vePlane &p2)
{
	veMat3 mat;
	mat.set(p0.normal().x(), p0.normal().y(), p0.normal().z()
		, p1.normal().x(), p1.normal().y(), p1.normal().z()
		, p2.normal().x(), p2.normal().y(), p2.normal().z());
	mat.inverse();
	return mat * -veVec3(p0.constantD(), p1.constantD(), p2.constantD());
}

void veDebuger::drawLine(const veVec3 &start, const veVec3 &end, const veVec4 &color)
{
	veDebugRenderer *dr = static_cast<veDebugRenderer *>(_renderer.get());
	std::unique_lock<std::mutex> lock(dr->verticesMutex);
	dr->vertices.push_back(start.x()); dr->vertices.push_back(start.y()); dr->vertices.push_back(start.z());
	dr->vertices.push_back(color.x()); dr->vertices.push_back(color.y()); dr->vertices.push_back(color.z()); dr->vertices.push_back(color.w());
	dr->vertices.push_back(end.x()); dr->vertices.push_back(end.y()); dr->vertices.push_back(end.z());
	dr->vertices.push_back(color.x()); dr->vertices.push_back(color.y()); dr->vertices.push_back(color.z()); dr->vertices.push_back(color.w());
}