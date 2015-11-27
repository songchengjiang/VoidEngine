#ifndef _VE_SHADER_DEFINATION_GENERATOR_
#define _VE_SHADER_DEFINATION_GENERATOR_
#include "NodeVisitor.h"
#include "SceneManager.h"
#include "Light.h"
#include "RenderCommand.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "Constants.h"
#include <sstream>
//shader definations
static const std::string SHADER_VERSION = "#version";
static const std::string PRECISION_DEFINE_FLOAT = "precision mediump float;";
static const std::string SHADER_DEFINE_BONES = "#define VE_USE_BONES";
static const std::string SHADER_DEFINE_LIGHTS = "#define VE_USE_LIGHTS";
static const std::string SHADER_DEFINE_TEXTURES = "#define VE_USE_TEXTURES";
static const std::string SHADER_DEFINE_TRANSFORMFEEDBACK = "#define VE_USE_TRANSFORMFEEDBACK";
static const std::string SHADER_DEFINE_DEFERRED_PATH = "#define VE_USE_DEFERRED_PATH";
static const std::string SHADER_DEFINE_SKYBOX = "#define VE_USE_SKYBOX";
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

	std::string getDefinations(veShader::Type type) {
		//_traversalMode = veNodeVisitor::TRAVERSE_CHILDREN;
		//_root->accept(*this);
		std::string definations;
        char str[16];
#if (VE_PLATFORM == VE_PLATFORM_WIN32) || (VE_PLATFORM == VE_PLATFORM_MAC)
        sprintf(str, " %d%d0\n", VE_GL_VERSION_MAJOR, VE_GL_VERSION_MINOR);
#else
        sprintf(str, " %d%d0 es\n", VE_GLSL_ES_VERSION_MAJOR, VE_GLSL_ES_VERSION_MINOR);
#endif
        definations += SHADER_VERSION + std::string(str);
		if (type == veShader::FRAGMENT_SHADER) {
			definations += PRECISION_DEFINE_FLOAT + std::string("\n");
		}

		if (_command.camera->getRenderPath() == veCamera::RenderPath::DEFERRED_PATH) {
			definations += SHADER_DEFINE_DEFERRED_PATH + std::string(" 1\n");
		}
		else {
			if (!_command.sceneManager->getLightList().empty()) {
				for (auto &light : _command.sceneManager->getLightList()) {
					if (light->isInScene() && light->isVisible()) {
						definations += SHADER_DEFINE_LIGHTS + std::string(" 1\n");
						definations += getLightDefination();
						break;
					}
				}
			}
		}
		if (type == veShader::VERTEX_SHADER) {
			if (_command.renderableObj) {
				if (_command.userDataList.valid()) {
					auto mesh = static_cast<veMesh *>(_command.userDataList->buffer()[1]);
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
			}

			if (_command.pass->getTransformFeedback()) {
				definations += SHADER_DEFINE_TRANSFORMFEEDBACK + std::string(" 1\n");
			}
		}

		if (true) {
			bool hasTex = false;
			if (_command.camera->getSceneManager()->getSkyBox()) {
				if (0 < _command.pass->getTextureNum()) {
					for (size_t i = 0; i < _command.pass->getTextureNum(); ++i) {
						if (_command.pass->getTexture(i)->getName()
							== _command.camera->getSceneManager()->getSkyBox()->getMaterialArray()->getMaterial(0)->getTechnique(0)->getPass(0)->getTexture(0)->getName()) {
							definations += SHADER_DEFINE_SKYBOX + std::string(" 1\n");
						}else {
							hasTex = true;
						}
					}
				}
			}
			else {
				if (0 < _command.pass->getTextureNum()) {
					hasTex = true;
				}
			}
			if (hasTex) {
				definations += SHADER_DEFINE_TEXTURES + std::string(" 1\n");
			}
		}

		return definations;
	}


	std::string getLightDefination()
	{
		std::stringstream def;
		def << "\
uniform struct {\n\
    int   " << veLight::DEFUALT_LIGHT_UNIFORM_TYPE_NAME << ";\n\
    vec3  " << veLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME << ";\n\
    vec3  " << veLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME << ";\n\
    vec3  " << veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME << ";\n\
}" << veLight::DEFUALT_LIGHT_UNIFORM_NAME << "[" << _command.sceneManager->getLightList().size() << "];\n\
const int " << veLight::DEFUALT_LIGHT_UNIFORM_MAX_NAME << " = " << _command.sceneManager->getLightList().size() << ";\n\
uniform int " << veLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME << ";\n\
#define VE_DIRECTIONAL_LIGHT " << veLight::DIRECTIONAL << "\n\
#define VE_POINT_LIGHT       " << veLight::POINT << "\n\
#define VE_SPOT_LIGHT        " << veLight::SPOT << "\n\
#define VE_AREA_LIGHT        " << veLight::AREA << "\n\
";
		return def.str();
	}

	//virtual void visit(veNode &node) {
	//	_root = &node;
	//}

	//virtual void visit(veLight &light) {
	//	if (_traversalMode == veNodeVisitor::TRAVERSE_CHILDREN)
	//		_lightList[light.getType()].push_back(&light);
	//}

	//veNode *getRoot() { return _root; }

private:

	const veRenderCommand &_command;
	//std::map<std::string, std::vector<veLight *> > _lightList;
	veNode                *_root;
};

#endif