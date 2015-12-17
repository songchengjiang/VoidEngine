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
static const std::string PRECISION_DEFINE_SAMPLER2DARRAYSHADOW = "precision highp sampler2DArrayShadow;";

static const std::string SHADER_DEFINE_PLATFORM_IOS     = "#define VE_PLATFORM_IOS 1\n";
static const std::string SHADER_DEFINE_PLATFORM_ANDROID = "#define VE_PLATFORM_ANDROID 2\n";
static const std::string SHADER_DEFINE_PLATFORM_WIN32   = "#define VE_PLATFORM_WIN32 3\n";
static const std::string SHADER_DEFINE_PLATFORM_LINUX   = "#define VE_PLATFORM_LINUX 4\n";
static const std::string SHADER_DEFINE_PLATFORM_MAC     = "#define VE_PLATFORM_MAC 5\n";

static const std::string SHADER_DEFINE_BONES = "#define VE_USE_BONES";
static const std::string SHADER_DEFINE_LIGHTS = "#define VE_USE_LIGHTS";
static const std::string SHADER_DEFINE_TEXTURES = "#define VE_USE_TEXTURES";
static const std::string SHADER_DEFINE_TRANSFORMFEEDBACK = "#define VE_USE_TRANSFORMFEEDBACK";
static const std::string SHADER_DEFINE_DEFERRED_PATH = "#define VE_USE_DEFERRED_PATH";

static const std::string SHADER_DEFINE_AMBIENT_TEXTURE = "#define VE_USE_AMBIENT_TEXTURE";
static const std::string SHADER_DEFINE_DIFFUSE_TEXTURE = "#define VE_USE_DIFFUSE_TEXTURE";
static const std::string SHADER_DEFINE_SPECULAR_TEXTURE = "#define VE_USE_SPECULAR_TEXTURE";
static const std::string SHADER_DEFINE_EMISSIVE_TEXTURE = "#define VE_USE_EMISSIVE_TEXTURE";
static const std::string SHADER_DEFINE_NORMAL_TEXTURE = "#define VE_USE_NORMAL_TEXTURE";
static const std::string SHADER_DEFINE_HEIGHT_TEXTURE = "#define VE_USE_HEIGHT_TEXTURE";
static const std::string SHADER_DEFINE_SHININESS_TEXTURE = "#define VE_USE_SHININESS_TEXTURE";
static const std::string SHADER_DEFINE_OPACITYT_TEXTURE = "#define VE_USE_OPACITYT_TEXTURE";
static const std::string SHADER_DEFINE_DISPLACEMENT_TEXTURE = "#define VE_USE_DISPLACEMENT_TEXTURE";
static const std::string SHADER_DEFINE_LIGHTMAP_TEXTURE = "#define VE_USE_LIGHTMAP_TEXTURE";
static const std::string SHADER_DEFINE_REFLECTION_TEXTURE = "#define VE_USE_REFLECTION_TEXTURE";

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

#if VE_PLATFORM == VE_PLATFORM_ANDROID
		if (type == veShader::FRAGMENT_SHADER) {
			definations += PRECISION_DEFINE_FLOAT + std::string("\n");
			definations += PRECISION_DEFINE_SAMPLER2DARRAYSHADOW + std::string("\n");
		}
#endif

		definations += SHADER_DEFINE_PLATFORM_IOS;
		definations += SHADER_DEFINE_PLATFORM_ANDROID;
		definations += SHADER_DEFINE_PLATFORM_WIN32;
		definations += SHADER_DEFINE_PLATFORM_LINUX;
		definations += SHADER_DEFINE_PLATFORM_MAC;

#if VE_PLATFORM == VE_PLATFORM_IOS
		definations += std::string("#define VE_PLATFORM VE_PLATFORM_IOS\n");
#elif VE_PLATFORM == VE_PLATFORM_ANDROID
		definations += std::string("#define VE_PLATFORM VE_PLATFORM_ANDROID\n");
#elif VE_PLATFORM == VE_PLATFORM_WIN32
		definations += std::string("#define VE_PLATFORM VE_PLATFORM_WIN32\n");
#elif VE_PLATFORM == VE_PLATFORM_LINUX
		definations += std::string("#define VE_PLATFORM VE_PLATFORM_LINUX\n");
#elif VE_PLATFORM == VE_PLATFORM_MAC
		definations += std::string("#define VE_PLATFORM VE_PLATFORM_MAC\n");
#endif

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

		if (0 < _command.pass->getTextureNum()) {
			definations += SHADER_DEFINE_TEXTURES + std::string(" 1\n");

			auto tex = _command.pass->getTexture(vePass::AMBIENT_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_AMBIENT_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::DIFFUSE_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_DIFFUSE_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::SPECULAR_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_SPECULAR_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::EMISSIVE_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_EMISSIVE_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::NORMAL_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_NORMAL_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::HEIGHT_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_HEIGHT_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::SHININESS_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_SHININESS_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::OPACITYT_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_OPACITYT_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::DISPLACEMENT_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_DISPLACEMENT_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::LIGHTMAP_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_LIGHTMAP_TEXTURE + std::string(" 1\n");
			}

			tex = _command.pass->getTexture(vePass::REFLECTION_TEXTURE);
			if (tex) {
				definations += SHADER_DEFINE_REFLECTION_TEXTURE + std::string(" 1\n");
			}
		}


		if (!_command.sceneManager->getLightListMap().empty()) {
			bool needLighting = false;
			for (auto &lightList : _command.sceneManager->getLightListMap()) {
				for (auto &light : lightList.second) {
					if (light->isInScene()) {
						needLighting = true;
						break;
					}
				}
				if (needLighting)
					break;
			}

			if (needLighting) {
				definations += SHADER_DEFINE_LIGHTS + std::string(" 1\n");
				definations += getLightDefination(type);
			}
		}
		return definations;
	}


	std::string getLightDefination(veShader::Type type)
	{
		std::stringstream def;
		def << "uniform sampler2D " << veLight::DEFUALT_LIGHT_TEXTURE_UNIFORM_NAME << ";" << std::endl;

//		if (0 < veDirectionalLight::totalLightNum()) {
//			def << "#define VE_DIRECTIONAL_LIGHT_MAX_NUM " << veDirectionalLight::totalLightNum() << std::endl;
//			def << "uniform float   " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform vec3  " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];"<<std::endl;
//			def << "uniform vec3  " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			if (type == veShader::VERTEX_SHADER) {
//				def << "uniform mat4 " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME << "[VE_DIRECTIONAL_LIGHT_MAX_NUM];" << std::endl;
//			}
//			if (type == veShader::FRAGMENT_SHADER) {
//				def << "uniform sampler2DArrayShadow " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME << ";" << std::endl;
//			}
//			//def << "uniform int " << veDirectionalLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME << ";" << std::endl;
//		}
//
//		if (0 < vePointLight::totalLightNum()) {
//			def << "#define VE_POINT_LIGHT_MAX_NUM " << vePointLight::totalLightNum() << std::endl;
//			def << "uniform float   " << vePointLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform vec3  " << vePointLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform vec3  " << vePointLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << vePointLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << vePointLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			if (type == veShader::VERTEX_SHADER) {
//				def << "uniform mat4 " << vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME << "[VE_POINT_LIGHT_MAX_NUM];" << std::endl;
//			}
//#if VE_PLATFORM != VE_PLATFORM_ANDROID
//			if (type == veShader::FRAGMENT_SHADER) {
//				def << "uniform samplerCubeArrayShadow " << vePointLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME << ";" << std::endl;
//			}
//#endif
//			//def << "uniform int " << vePointLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME << ";" << std::endl;
//		}
//		if (0 < veSpotLight::totalLightNum()) {
//			def << "#define VE_SPOT_LIGHT_MAX_NUM " << veSpotLight::totalLightNum() << std::endl;
//			def << "uniform float   " << veSpotLight::DEFUALT_LIGHT_UNIFORM_VISIBLE_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform vec3  " << veSpotLight::DEFUALT_LIGHT_UNIFORM_POSITION_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform vec3  " << veSpotLight::DEFUALT_LIGHT_UNIFORM_DIRECTION_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform vec3  " << veSpotLight::DEFUALT_LIGHT_UNIFORM_COLOR_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_INTENSITY_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_ATTENUATION_RANGE_INVERSE_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_INNER_ANGLE_COS_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_OUTER_ANGLE_COS_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_ENABLED_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_BIAS_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_STRENGTH_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTSHADOW_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			def << "uniform float " << veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_SOFTNESS_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			if (type == veShader::VERTEX_SHADER) {
//				def << "uniform mat4 " << veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MATRIX_NAME << "[VE_SPOT_LIGHT_MAX_NUM];" << std::endl;
//			}
//			if (type == veShader::FRAGMENT_SHADER) {
//				def << "uniform sampler2DArrayShadow " << veSpotLight::DEFUALT_LIGHT_UNIFORM_SHADOW_MAP_NAME << ";" << std::endl;
//			}
//			//def << "uniform int " << veSpotLight::DEFUALT_LIGHT_UNIFORM_NUM_NAME << ";" << std::endl;
//		}

		return def.str();
	}

private:

	const veRenderCommand &_command;
	veNode                *_root;
};

#endif