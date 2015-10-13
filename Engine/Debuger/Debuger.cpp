#include "Debuger.h"
#include "KernelCore/RenderableObject.h"
#include "KernelCore/NodeVisitor.h"
#include "KernelCore/Node.h"
#include "KernelCore/Mesh.h"
#include "KernelCore/BoudingBox.h"
#include "KernelCore/RenderCommand.h"
#include "KernelCore/RenderQueue.h"
#include "Constants.h"

class RenderableObjectFinder : public veNodeVisitor
{
public:
    
	RenderableObjectFinder(std::vector<veNode *> &renderableNodes)
		: _renderableNodes(renderableNodes)
	{}

	virtual void visit(veNode &node) {
		if (0 < node.getRenderableObjectCount())
			_renderableNodes.push_back(&node);
	}

private:

	std::vector<veNode *> &_renderableNodes;
};

veDebuger::veDebuger()
	: _material(new veMaterial)
	, _vao(0)
	, _vbo(0)
	, _attachedNode(nullptr)
	, _color(veVec4::WHITE)
	, _isDrawMeshWireframe(false)
	, _isDrawBoundingBoxWireframe(false)
{
	initMaterial();
}

veDebuger::~veDebuger()
{

}

void veDebuger::update(veNode *node, veVisualiser *vs)
{
	_attachedNode = node;
	_renderableNodes.clear();
	_vertices.clear();
	RenderableObjectFinder finder(_renderableNodes);
	node->accept(finder);

	veMat4 nTow = node->getWorldToNodeMatrix();
	if (_isDrawBoundingBoxWireframe) {
		renderBoundingBoxWireframe(node->getBoundingBox(), nTow);
	}

	for (auto &iter : _renderableNodes) {
		veMat4 rnToNode = nTow * iter->getNodeToWorldMatrix();
		for (unsigned int i = 0; i < iter->getRenderableObjectCount(); ++i) {
			auto mesh = dynamic_cast<veMesh *>(iter->getRenderableObject(i));
			if (mesh) {
				if (_isDrawMeshWireframe)
					renderMeshWireframe(mesh, rnToNode);
				if (_isDrawBoundingBoxWireframe) {
					//if (!mesh->getBoneNum())
						renderBoundingBoxWireframe(mesh->getBoundingBox(), rnToNode);
					//else {
					//	for (size_t i = 0; i < mesh->getBoneNum(); ++i) {
					//		renderBoundingBoxWireframe(mesh->getBone(i)->getBoundingBox(), rnToNode);
					//	}
					//}
				}
			}
		}
	}
}

void veDebuger::render(veCamera *camera)
{
	veRenderCommand rc;
	rc.priority = veRenderCommand::LOW_PRIORITY;
	rc.pass = _material->getTechnique(0)->getPass(0);
	rc.worldMatrix = _attachedNode->getNodeToWorldMatrix();
	rc.renderableObj = nullptr;
	rc.camera = camera;
	rc.drawFunc = VE_CALLBACK_1(veDebuger::draw, this);
	veRenderQueue::CURRENT_RENDER_QUEUE->pushCommand(veRenderQueue::RENDER_QUEUE_ENTITY, rc);
}

void veDebuger::initMaterial()
{
	const char *V_SHADER = " \
	layout(location = 0) in vec3 position; \n \
	uniform mat4 u_ModelViewProjectMat; \n \
	void main() \n \
	{                   \n \
		gl_Position = u_ModelViewProjectMat * vec4(position, 1.0); \n \
	}";

	const char *F_SHADER = " \
	layout(location = 0) out vec4 fragColor; \n \
	uniform vec4 u_Color; \n \
	void main() {  \n \
		fragColor = u_Color; \n \
	}";

	auto tech = new veTechnique;
	auto pass = new vePass;
	_material->addTechnique(tech);
	tech->addPass(pass);

	pass->depthTest() = false;
	pass->depthWrite() = false;
	pass->cullFace() = true;

	auto vShader = new veShader(veShader::VERTEX_SHADER, V_SHADER);
	auto fShader = new veShader(veShader::FRAGMENT_SHADER, F_SHADER);
	pass->setShader(vShader);
	pass->setShader(fShader);

	pass->addUniform(new veUniform("u_ModelViewProjectMat", MVP_MATRIX));
	_colorUniform = new veUniform("u_Color", _color);
	pass->addUniform(_colorUniform.get());
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
			for (size_t i = 0; i < primitive.indices->size(); ++i) {
				unsigned int vID = (*primitive.indices)[i] * stride + offset;
				veVec3 v = veVec3(va->buffer()[vID], va->buffer()[vID + 1], va->buffer()[vID + 2]);
				v = trans * v;
				_vertices.push_back(v.x());
				_vertices.push_back(v.y());
				_vertices.push_back(v.z());
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
				_vertices.push_back(v0.x()); _vertices.push_back(v0.y()); _vertices.push_back(v0.z());
				_vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());
				_vertices.push_back(v0.x()); _vertices.push_back(v0.y()); _vertices.push_back(v0.z());
				_vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());
				_vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());
				_vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());
			}
		}
	}
}

void veDebuger::renderBoundingBoxWireframe(const veBoundingBox &bbox, const veMat4 &trans)
{
	veBoundingBox transbbox = bbox * trans;
	veVec3 bbmin = transbbox.min();
	veVec3 bbmax = transbbox.max();
	veVec3 v0 = bbmin;
	veVec3 v1 = veVec3(bbmax.x(), bbmin.y(), bbmin.z());
	veVec3 v2 = veVec3(bbmax.x(), bbmax.y(), bbmin.z());
	veVec3 v3 = veVec3(bbmin.x(), bbmax.y(), bbmin.z());
	veVec3 v4 = veVec3(bbmin.x(), bbmin.y(), bbmax.z());
	veVec3 v5 = veVec3(bbmax.x(), bbmin.y(), bbmax.z());
	veVec3 v6 = bbmax;
	veVec3 v7 = veVec3(bbmin.x(), bbmax.y(), bbmax.z());

	_vertices.push_back(v0.x()); _vertices.push_back(v0.y()); _vertices.push_back(v0.z());
	_vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());

	_vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());
	_vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());

	_vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());
	_vertices.push_back(v3.x()); _vertices.push_back(v3.y()); _vertices.push_back(v3.z());

	_vertices.push_back(v3.x()); _vertices.push_back(v3.y()); _vertices.push_back(v3.z());
	_vertices.push_back(v0.x()); _vertices.push_back(v0.y()); _vertices.push_back(v0.z());

	_vertices.push_back(v4.x()); _vertices.push_back(v4.y()); _vertices.push_back(v4.z());
	_vertices.push_back(v5.x()); _vertices.push_back(v5.y()); _vertices.push_back(v5.z());

	_vertices.push_back(v5.x()); _vertices.push_back(v5.y()); _vertices.push_back(v5.z());
	_vertices.push_back(v6.x()); _vertices.push_back(v6.y()); _vertices.push_back(v6.z());

	_vertices.push_back(v6.x()); _vertices.push_back(v6.y()); _vertices.push_back(v6.z());
	_vertices.push_back(v7.x()); _vertices.push_back(v7.y()); _vertices.push_back(v7.z());

	_vertices.push_back(v7.x()); _vertices.push_back(v7.y()); _vertices.push_back(v7.z());
	_vertices.push_back(v4.x()); _vertices.push_back(v4.y()); _vertices.push_back(v4.z());

	_vertices.push_back(v0.x()); _vertices.push_back(v0.y()); _vertices.push_back(v0.z());
	_vertices.push_back(v4.x()); _vertices.push_back(v4.y()); _vertices.push_back(v4.z());

	_vertices.push_back(v1.x()); _vertices.push_back(v1.y()); _vertices.push_back(v1.z());
	_vertices.push_back(v5.x()); _vertices.push_back(v5.y()); _vertices.push_back(v5.z());

	_vertices.push_back(v2.x()); _vertices.push_back(v2.y()); _vertices.push_back(v2.z());
	_vertices.push_back(v6.x()); _vertices.push_back(v6.y()); _vertices.push_back(v6.z());

	_vertices.push_back(v3.x()); _vertices.push_back(v3.y()); _vertices.push_back(v3.z());
	_vertices.push_back(v7.x()); _vertices.push_back(v7.y()); _vertices.push_back(v7.z());
}

void veDebuger::draw(const veRenderCommand &command)
{
	_colorUniform->setValue(_color);
	command.pass->apply(command);

	if (!_vao) {
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
	}

	glBindVertexArray(_vao);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	if (!_vertices.empty())
		glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(_vertices[0]), _vertices.buffer(), GL_STATIC_DRAW);

	//v
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glDrawArrays(GL_LINES, 0, _vertices.size());
}
