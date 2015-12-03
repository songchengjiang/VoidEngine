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
					if (light->isInScene() && light->isVisible() && (light->getMask() & _command.mask)) {
						definations += SHADER_DEFINE_LIGHTS + std::string(" 1\n");
						definations += getLightDefination(type);
						break;
					}
				}
			}
		}
		if (type == veShader::VERTEX_SHADER) {
			bool hasAttriDef = false;
			if (_command.renderableObj) {
				if (_command.user2) {
					auto mesh = static_cast<veMesh *>(_command.user2);
					if (mesh) {
						if (mesh->getBoneNum())
							definations += SHADER_DEFINE_BONES + std::string(" 1\n");

						char str[8];
						for (unsigned int i = 0; i < mesh->getVertexAtrributeNum(); ++i) {
							sprintf(str, " %d\n", i);
							auto attr = mesh->getVertexAtrribute(i);
							definations += SHADER_DEFINE_ATTRIBUTE_ARRAY[attr.attributeType] + std::string(str);
						}
						hasAttriDef = true;
					}
				}
			}

			if (!hasAttriDef) {
				definations += SHADER_DEFINE_ATTRIBUTE_ARRAY[0] + std::string(" 0\n");
				definations += SHADER_DEFINE_ATTRIBUTE_ARRAY[1] + std::string(" 1\n");
				definations += SHADER_DEFINE_ATTRIBUTE_ARRAY[4] + std::string(" 2\n");
			}

			if (_command.pass->getTransformFeedback()) {
				definations += SHADER_DEFINE_TRANSFORMFEEDBACK + std::string(" 1\n");
			}
		}

		if (true) {
			bool hasTex = false;
			if (_command.sceneManager->getSkyBox()) {
				if (0 < _command.pass->getTextureNum()) {
					for (size_t i = 0; i < _command.pass->getTextureNum(); ++i) {
						if (_command.pass->getTexture(i)->getName()
							== _command.sceneManager->getSkyBox()->getMaterialArray()->getMaterial(0)->getTechnique(0)->getPass(0)->getTexture(0)->getName()) {
							if (_command.sceneManager->getSkyBox()->getMask() & _command.mask)
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


	std::string getLightDefination(veShader::Type type)
	{
		std::stringstream def;
		def << "\
#define VE_DIRECTIONAL_LIGHT " << veLight::DIRECTIONAL << "\n\
#define VE_POINT_LIGHT       " << veLight::POINT << "\n\
#define VE_SPOT_LIGHT        " << veLight::SPOT << "\n\
#define VE_AREA_LIGHT        " << veLight::AREA << "\n\
#define VE_LIGHT_MAX_NUM     " << _command.sceneManager->getLightList().size() << "\n\
uniform struct {\n\
    int   " << veLight::DEFUALT_LIGHT_UNIFORM_TYPE_NAME << ";\n\
    vec3  " << veLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME << ";\n\
    vec3  " << veLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME << ";\n\
    vec3  " << veLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME << ";\n\
    float " << veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME << ";\n\
    mat4 " << veLight::DEFUALT_LIGHT_UNIFORM_LIGHT_MATRIX_NAME << ";\n\
}" << veLight::DEFUALT_LIGHT_UNIFORM_NAME << "[VE_LIGHT_MAX_NUM];\n\
uniform int " << veLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME << ";\n\
";

		if (type == veShader::FRAGMENT_SHADER) {
			def << "\
uniform sampler2DShadow " << veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_2D_NAME << "[VE_LIGHT_MAX_NUM]; \n\
uniform samplerCubeShadow " << veLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_CUBE_NAME << "[VE_LIGHT_MAX_NUM]; \n\
";
		}

	
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