#ifndef _VE_SHADER_DEFINATION_GENERATOR_
#define _VE_SHADER_DEFINATION_GENERATOR_
#include "NodeVisitor.h"
#include "Light.h"
#include "RenderCommand.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
//shader definations
static const std::string SHADER_VERSION = "#version";
static const std::string SHADER_DEFINE_BONES = "#define VE_USE_BONES";
static const std::string SHADER_DEFINE_LIGHTS = "#define VE_USE_LIGHTS";
static const std::string SHADER_DEFINE_TEXTURES = "#define VE_USE_TEXTURES";
static const std::string SHADER_DEFINE_POINT_LIGHTS = "#define VE_USE_POINT_LIGHTS";
static const std::string SHADER_DEFINE_ATTRIBUTE_ARRAY[] = {
	"#define ATTR_POSITION",
	"#define ATTR_NORMAL",
	"#define ATTR_TANGENT",
	"#define ATTR_BITANGENT",
	"#define ATTR_TEXTURE_COORD0",
	"#define ATTR_TEXTURE_COORD1",
	"#define ATTR_TEXTURE_COORD2",
	"#define ATTR_TEXTURE_COORD3",
	"#define ATTR_COLOR0",
	"#define ATTR_COLOR1",
	"#define ATTR_COLOR2",
	"#define ATTR_COLOR3",
	"#define ATTR_BONE_INDICES",
	"#define ATTR_BONE_WEIGHTS",
	"#define ATTR_USER0",
	"#define ATTR_USER1"
};
//static const std::string SHADER_DEFINE_ATTRIBUTE_POSITION = "#define ATTR_POSITION";
//static const std::string SHADER_DEFINE_ATTRIBUTE_NORMAL = "#define ATTR_NORMAL";
//static const std::string SHADER_DEFINE_ATTRIBUTE_TANGENT = "#define ATTR_TANGENT";
//static const std::string SHADER_DEFINE_ATTRIBUTE_BITANGENT = "#define ATTR_BITANGENT";
//static const std::string SHADER_DEFINE_ATTRIBUTE_TEXTURE_COORD0 = "#define ATTR_TEXTURE_COORD0";
//static const std::string SHADER_DEFINE_ATTRIBUTE_TEXTURE_COORD1 = "#define ATTR_TEXTURE_COORD1";
//static const std::string SHADER_DEFINE_ATTRIBUTE_TEXTURE_COORD2 = "#define ATTR_TEXTURE_COORD2";
//static const std::string SHADER_DEFINE_ATTRIBUTE_TEXTURE_COORD3 = "#define ATTR_TEXTURE_COORD3";
//static const std::string SHADER_DEFINE_ATTRIBUTE_COLOR0 = "#define ATTR_COLOR0";
//static const std::string SHADER_DEFINE_ATTRIBUTE_COLOR1 = "#define ATTR_COLOR1";
//static const std::string SHADER_DEFINE_ATTRIBUTE_COLOR2 = "#define ATTR_COLOR2";
//static const std::string SHADER_DEFINE_ATTRIBUTE_COLOR3 = "#define ATTR_COLOR3";
//static const std::string SHADER_DEFINE_ATTRIBUTE_BONE_INDICES = "#define ATTR_BONE_INDICES";
//static const std::string SHADER_DEFINE_ATTRIBUTE_BONE_WEIGHTS = "#define ATTR_BONE_WEIGHTS";
//static const std::string SHADER_DEFINE_ATTRIBUTE_USER0 = "#define ATTR_USER0";
//static const std::string SHADER_DEFINE_ATTRIBUTE_USER1 = "#define ATTR_USER1";

class ShaderDefinatiosGenerator : public veNodeVisitor
{
public:
	ShaderDefinatiosGenerator(const veRenderCommand &command)
		: _command(command)
		, _root(nullptr)
	{
		//_traversalMode = veNodeVisitor::TRAVERSE_PARENT;
	}

	std::string getDefinations(vePass *pass, veShader::Type type) {
		//_traversalMode = veNodeVisitor::TRAVERSE_CHILDREN;
		//_root->accept(*this);
		std::string definations;
		definations += SHADER_VERSION + std::string(" 430\n");
		if (!_lightList.empty()) {
			definations += SHADER_DEFINE_LIGHTS + std::string(" 1\n");
		}

		if (type == veShader::VERTEX_SHADER) {
			auto mesh = dynamic_cast<veMesh *>(_command.renderableObj);
			if (mesh) {
				if (mesh->getBoneNum())
					definations += SHADER_DEFINE_BONES + std::string(" 1\n");

				char str[8];
				for (unsigned int i = 0; i < mesh->getVertexAtrributeNum(); ++i) {
					sprintf(str, " %d\n", i);
					auto attr = mesh->getVertexAtrribute(i);
					definations += SHADER_DEFINE_ATTRIBUTE_ARRAY[attr.attributeType] + std::string(str);
				}
			}
		}

		if (type == veShader::FRAGMENT_SHADER) {
			if (0 < pass->getTextureNum()) {
				definations += SHADER_DEFINE_TEXTURES + std::string(" 1\n");
			}
		}

		return definations;
	}

	virtual void visit(veNode &node) {
		_root = &node;
	}

	virtual void visit(veLight &light) {
		if (_traversalMode == veNodeVisitor::TRAVERSE_CHILDREN)
			_lightList.push_back(&light);
	}

	veNode *getRoot() { return _root; }

private:

	const veRenderCommand &_command;
	std::vector<veLight *> _lightList;
	veNode                *_root;
};

#endif