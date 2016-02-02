#ifndef _VE_CONSTANTS_
#define _VE_CONSTANTS_
#include <string>

static const std::string VERSION_KEY = "version";
static const std::string NAME_KEY = "name";
static const std::string MATERIALS_KEY = "materials";
static const std::string MATERIAL_KEY = "material";
static const std::string MESHES_KEY = "meshes";
static const std::string ATTRIBUTES_KEY = "attributes";
static const std::string ATTRIBUTE_KEY = "attribute";
static const std::string SIZE_KEY = "size";
static const std::string VERTICES_KEY = "vertices";
static const std::string PARTS_KEY = "parts";
static const std::string NODES_KEY = "nodes";
static const std::string TYPE_KEY = "type";
static const std::string INDICES_KEY = "indices";
static const std::string SKELETON_KEY = "skeleton";
static const std::string TRANSFORM_KEY = "transform";
static const std::string ANIMATIONS_KEY = "animations";
static const std::string DURATION_KEY = "duration";
static const std::string FRAMES_KEY = "frames";
static const std::string FRAMERATE_KEY = "frameRate";
static const std::string POSITIONS_KEY = "positions";
static const std::string ROTATIONS_KEY = "rotations";
static const std::string SCALES_KEY = "scales";
static const std::string BONES_KEY = "bones";
static const std::string WEIGHTS_KEY = "weights";

static const std::string TECHNIQUES_KEY = "techniques";
static const std::string PASSES_KEY = "passes";
static const std::string DEPTHTEST_KEY = "depthTest";
static const std::string DEPTHWRITE_KEY = "depthWrite";
static const std::string STENCIALTEST_KEY = "stencialTest";
static const std::string CULLFACE_KEY = "cullFace";
static const std::string CULLFACEMODE_KEY = "cullFaceMode";
static const std::string BLENDFUNC_KEY = "blendFunc";
static const std::string BLENDEQUATION_KEY = "blendEquation";
static const std::string STENCIALFUNC_KEY = "stencialFunc";
static const std::string STENCIALOP_KEY = "stencialOp";
static const std::string ONE_KEY = "ONE";
static const std::string ZERO_KEY = "ZERO";
static const std::string SRC_COLOR_KEY = "SRC_COLOR";
static const std::string ONE_MINUS_SRC_COLOR_KEY = "ONE_MINUS_SRC_COLOR";
static const std::string SRC_ALPHA_KEY = "SRC_ALPHA";
static const std::string ONE_MINUS_SRC_ALPHA_KEY = "ONE_MINUS_SRC_ALPHA";
static const std::string DST_ALPHA_KEY = "DST_ALPHA";
static const std::string ONE_MINUS_DST_ALPHA_KEY = "ONE_MINUS_DST_ALPHA";
static const std::string DST_COLOR_KEY = "DST_COLOR";
static const std::string ONE_MINUS_DST_COLOR_KEY = "ONE_MINUS_DST_COLOR";
static const std::string FUNC_ADD_KEY = "FUNC_ADD";
static const std::string FUNC_SUBTRACT_KEY = "FUNC_SUBTRACT";
static const std::string FUNC_REVERSE_SUBTRACT_KEY = "FUNC_REVERSE_SUBTRACT";
static const std::string MIN_KEY = "MIN";
static const std::string MAX_KEY = "MAX";
static const std::string SHADERS_KEY = "shaders";
static const std::string VERTEXSHADER_KEY = "VERTEX_SHADER";
static const std::string FRAGMENTSHADER_KEY = "FRAGMENT_SHADER";
static const std::string DEFINATION_KEY = "defination";
static const std::string SOURCE_KEY = "source";
static const std::string TARGET_KEY = "target";
static const std::string FORMAT_KEY = "format";
static const std::string TEXTUREUNITS_KEY = "textureUnits";
static const std::string WRAP_KEY = "wrap";
static const std::string REPEAT_KEY = "REPEAT";
static const std::string CLAMP_KEY = "CLAMP";
static const std::string MIRROR_KEY = "MIRROR";
static const std::string DECAL_KEY = "DECAL";
static const std::string FILTER_KEY = "filter";
static const std::string NEREAST_KEY = "NERAEST";
static const std::string LINEAR_KEY = "LINEAR";
static const std::string TEXTYPE_KEY = "texType";
static const std::string AMBIENT_TEXTURE_KEY = "AMBIENT_TEXTURE";
static const std::string DIFFUSE_TEXTURE_KEY = "DIFFUSE_TEXTURE";
static const std::string SPECULAR_TEXTURE_KEY = "SPECULAR_TEXTURE";
static const std::string EMISSIVE_TEXTURE_KEY = "EMISSIVE_TEXTURE";
static const std::string NORMAL_TEXTURE_KEY = "NORMAL_TEXTURE";
static const std::string HEIGHT_TEXTURE_KEY = "HEIGHT_TEXTURE";
static const std::string SHININESS_TEXTURE_KEY = "SHININESS_TEXTURE";
static const std::string OPACITYT_TEXTURE_KEY = "OPACITYT_TEXTURE";
static const std::string DISPLACEMENT_TEXTURE_KEY = "DISPLACEMENT_TEXTURE";
static const std::string LIGHTMAP_TEXTURE_KEY = "LIGHTMAP_TEXTURE";
static const std::string REFLECTION_TEXTURE_KEY = "REFLECTION_TEXTURE";
static const std::string NEAREST_MIP_MAP_NERAEST_KEY = "NEAREST_MIP_MAP_NERAEST";
static const std::string NEAREST_MIP_MAP_LINEAR_KEY = "NEAREST_MIP_MAP_LINEAR";
static const std::string LINEAR_MIP_MAP_NEAREST_KEY = "LINEAR_MIP_MAP_NEAREST";
static const std::string LINEAR_MIP_MAP_LINEAR_KEY = "LINEAR_MIP_MAP_LINEAR";
static const std::string MIPMAP_KEY = "mipmap";
static const std::string FLOAT_KEY = "FLOAT";
static const std::string UINT_KEY = "UINT";
static const std::string USHORT_KEY = "USHORT";
static const std::string BYTE_KEY = "BYTE";
static const std::string INT_KEY = "INT";
static const std::string BOOL_KEY = "BOOL";
static const std::string VEC2_KEY = "VEC2";
static const std::string VEC3_KEY = "VEC3";
static const std::string VEC4_KEY = "VEC4";
static const std::string MAT3_KEY = "MAT3";
static const std::string MAT4_KEY = "MAT4";
static const std::string POINTS_KEY = "POINTS";
static const std::string LINES_KEY = "LINES";
static const std::string TRIANGLES_KEY = "TRIANGLES";
static const std::string COLOR_ATTACHMENT_KEY = "COLOR";
static const std::string DEPTH_ATTACHMENT_KEY = "DEPTH";
static const std::string STENCIAL_ATTACHMENT_KEY = "STENCIAL";
static const std::string FRONT_KEY = "FRONT";
static const std::string BACK_KEY = "BACK";
static const std::string FRONTANDBACK_KEY = "FRONT_AND_BACK";
static const std::string WIDTH_KEY = "width";
static const std::string HEIGHT_KEY = "height";
static const std::string DRAWMASK_KEY = "drawMask";

static const std::string NEVER_KEY = "NEVER";
static const std::string LESS_KEY = "LESS";
static const std::string LEQUAL_KEY = "LEQUAL";
static const std::string GREATER_KEY = "GREATER";
static const std::string GEQUAL_KEY = "GEQUAL";
static const std::string EQUAL_KEY = "EQUAL";
static const std::string NOTEQUAL_KEY = "NOTEQUAL";
static const std::string ALWAYS_KEY = "ALWAYS";
static const std::string KEEP_KEY = "KEEP";
static const std::string REPLACE_KEY = "REPLACE";
static const std::string INCR_KEY = "INCR";
static const std::string INCR_WRAP_KEY = "INCR_WRAP";
static const std::string DECR_KEY = "DECR";
static const std::string DECR_WRAP_KEY = "DECR_WRAP";
static const std::string INVERT_KEY = "INVERT";

static const std::string RENDERPATH_KEY = "renderPath";
static const std::string FORWARD_KEY = "FORWARD";
static const std::string DEFERRED_KEY = "DEFERRED";
static const std::string VIEWPORT_KEY = "viewport";
static const std::string PROJECTION_KEY = "projection";
static const std::string PERSPECTIVE_KEY = "PERSPECTIVE";
static const std::string ORTHO_KEY = "ORTHO";
static const std::string LEFT_KEY = "left";
static const std::string RIGHT_KEY = "right";
static const std::string BOTTOM_KEY = "bottom";
static const std::string TOP_KEY = "top";
static const std::string FOV_KEY = "fov";
static const std::string ASPECTRATIO_KEY = "aspectRatio";
static const std::string ZNEAR_KEY = "zNear";
static const std::string ZFAR_KEY = "zFar";
static const std::string LOOKAT_KEY = "lookAt";
static const std::string EYE_KEY = "eye";
static const std::string CENTER_KEY = "center";
static const std::string UP_KEY = "up";
static const std::string FBO_KEY = "fbo";
static const std::string ATTACHMENT_KEY = "attachment";
static const std::string TEX_2D_KEY = "TEX_2D";
static const std::string TEX_3D_KEY = "TEX_3D";
static const std::string TEX_RECT_KEY = "TEX_RECT";
static const std::string TEX_CUBE_KEY = "TEX_CUBE";
static const std::string TEX_CUBE_POSITIVE_X_KEY = "TEX_CUBE_POSITIVE_X";
static const std::string TEX_CUBE_NEGATIVE_X_KEY = "TEX_CUBE_NEGATIVE_X";
static const std::string TEX_CUBE_POSITIVE_Y_KEY = "TEX_CUBE_POSITIVE_Y";
static const std::string TEX_CUBE_NEGATIVE_Y_KEY = "TEX_CUBE_NEGATIVE_Y";
static const std::string TEX_CUBE_POSITIVE_Z_KEY = "TEX_CUBE_POSITIVE_Z";
static const std::string TEX_CUBE_NEGATIVE_Z_KEY = "TEX_CUBE_NEGATIVE_Z";

static const std::string DIRECTIONAL_KEY = "DIRECTIONAL";
static const std::string POINT_KEY = "POINT";
static const std::string SPOT_KEY = "SPOT";
static const std::string AREA_KEY = "AREA";
static const std::string COLOR_KEY = "color";
static const std::string POSITION_KEY = "position";
static const std::string DIRECTION_KEY = "direction";
static const std::string INTENSITY_KEY = "intensity";
static const std::string ATTENUATION_KEY = "attenuation";
static const std::string INNER_ANGLE_KEY = "innerAngle";
static const std::string OUTER_ANGLE_KEY = "outerAngle";
static const std::string SHADOW_ENABLED_KEY = "shadowEnabled";
static const std::string SHADOW_SOFT_KEY = "shadowSoft";
static const std::string SHADOW_SOFTNESS_KEY = "shadowSoftness";
static const std::string SHADOW_MAP_KEY = "shadowMap";
static const std::string SHADOW_BIAS_KEY = "shadowBias";
static const std::string SHADOW_STRENGTH_KEY = "shadowStrength";
static const std::string SHADOW_RESOLUTION_KEY = "shadowResolution";
static const std::string SHADOW_AREA_KEY = "shadowArea";

static const std::string VERTEX_ATTRIB_POSITION = "VERTEX_ATTRIB_POSITION";
static const std::string VERTEX_ATTRIB_NORMAL = "VERTEX_ATTRIB_NORMAL";
static const std::string VERTEX_ATTRIB_TANGENT = "VERTEX_ATTRIB_TANGENT";
static const std::string VERTEX_ATTRIB_BITANGENT = "VERTEX_ATTRIB_BITANGENT";
static const std::string VERTEX_ATTRIB_TEX_COORD = "VERTEX_ATTRIB_TEX_COORD";
static const std::string VERTEX_ATTRIB_BONE_INDICES = "VERTEX_ATTRIB_BONE_INDICES";
static const std::string VERTEX_ATTRIB_BONE_WEIGHTS = "VERTEX_ATTRIB_BONE_WEIGHTS";
static const std::string VERTEX_ATTRIB_COLOR = "VERTEX_ATTRIB_COLOR";
static const std::string VERTEX_ATTRIB_USER = "VERTEX_ATTRIB_USER";

//////////////////////////////////////////////////////////////////////////

//internal shader names
static const std::string M_MATRIX_KEY = "u_ModelMat";
static const std::string V_MATRIX_KEY = "u_ViewMat";
static const std::string INV_V_MATRIX_KEY = "u_InvViewMat";
static const std::string MV_MATRIX_KEY = "u_ModelViewMat";
static const std::string P_MATRIX_KEY = "u_ProjectMat";
static const std::string MVP_MATRIX_KEY = "u_ModelViewProjectMat";
static const std::string NORMAL_MATRIX_KEY = "u_NormalMat";
static const std::string NORMAL_WORLD_MATRIX_KEY = "u_NormalWorldMat";
static const std::string BONE_MATRIXES_KEY = "u_BoneMates";
static const std::string AMBIENT_KEY = "u_ambient";
static const std::string DIFFUSE_KEY = "u_diffuse";
static const std::string EMISSIVE_KEY = "u_emissive";
static const std::string SPECULAR_KEY = "u_specular";
static const std::string TRANSPARENT_KEY = "u_transparent";
static const std::string REFLECTIVE_KEY = "u_reflective";
static const std::string SHININESS_KEY = "u_shininess";
static const std::string OPACITY_KEY = "u_opacity";
static const std::string ROUGHNESS_KEY = "u_roughness";
static const std::string FRESNEL_KEY = "u_fresnel";
static const std::string REFLECTIVITY_KEY = "u_reflectivity";
static const std::string LIGHTMASK_KEY = "u_lightMask";
static const std::string AMBIENTTEX_KEY = "u_ambientTex";
static const std::string DIFFUSETEX_KEY = "u_diffuseTex";
static const std::string SPECULARTEX_KEY = "u_specularTex";
static const std::string EMISSIVETEX_KEY = "u_emissiveTex";
static const std::string HEIGHTTEX_KEY = "u_heightTex";
static const std::string NORMALTEX_KEY = "u_normalTex";
static const std::string SHININESSTEX_KEY = "u_shininessTex";
static const std::string OPACITYTEX_KEY = "u_opacityTex";
static const std::string DISPLACEMENTTEX_KEY = "u_displacementTex";
static const std::string LIGHTMAPTEX_KEY = "u_lightmapTex";
static const std::string REFLECTIONTEX_KEY = "u_reflectionTex";
static const std::string TF_VARYING_POSITION_KEY = "tf_position";
static const std::string TF_VARYING_NORMAL_KEY = "tf_normal";

static const std::string M_MATRIX      = "M_MATRIX";
static const std::string V_MATRIX      = "V_MATRIX";
static const std::string INV_V_MATRIX  = "INV_V_MATRIX";
static const std::string MV_MATRIX     = "MV_MATRIX";
static const std::string P_MATRIX      = "P_MATRIX";
static const std::string INV_P_MATRIX  = "INV_P_MATRIX";
static const std::string INV_VP_MATRIX  = "INV_VP_MATRIX";
static const std::string MVP_MATRIX    = "MVP_MATRIX";
static const std::string NORMAL_MATRIX = "NORMAL_MATRIX";
static const std::string NORMAL_WROLD_MATRIX = "NORMAL_WORLD_MATRIX";
static const std::string BONE_MATRIXES = "BONE_MATRIXES";
static const std::string SCREEN_WIDTH  = "SCREEN_WIDTH";
static const std::string SCREEN_HEIGHT = "SCREEN_HEIGHT";
static const std::string SIM_TIME      = "SIM_TIME";
static const std::string SIM_SIN_TIME  = "SIM_SIN_TIME";
static const std::string CAMERA_WORLD_POS  = "CAMERA_WORLD_POS";

//////////////////////////////////////////////////////////////////////////

static const std::string MODEL_EXT = ".vem";
static const std::string MATERIAL_EXT = ".vemtl";
static const std::string ANIMATION_EXT = ".veanim";

//////////////////////////////////////////////////////////////////////////

static const std::string SHADER_DEFINE_BONES             = "#define VE_USE_BONES 1\n";
static const std::string SHADER_DEFINE_LIGHTS            = "#define VE_USE_LIGHTS 1\n";
static const std::string SHADER_DEFINE_TEXTURES          = "#define VE_USE_TEXTURES 1\n";
static const std::string SHADER_DEFINE_NROMAL_MAPPING    = "#define VE_USE_NROMAL_MAPPING 1\n";

static const std::string SHADER_DEFINE_AMBIENT_TEXTURE      = "#define VE_USE_AMBIENT_TEXTURE 1\n";
static const std::string SHADER_DEFINE_DIFFUSE_TEXTURE      = "#define VE_USE_DIFFUSE_TEXTURE 1\n";
static const std::string SHADER_DEFINE_SPECULAR_TEXTURE     = "#define VE_USE_SPECULAR_TEXTURE 1\n";
static const std::string SHADER_DEFINE_EMISSIVE_TEXTURE     = "#define VE_USE_EMISSIVE_TEXTURE 1\n";
static const std::string SHADER_DEFINE_NORMAL_TEXTURE       = "#define VE_USE_NORMAL_TEXTURE 1\n";
static const std::string SHADER_DEFINE_HEIGHT_TEXTURE       = "#define VE_USE_HEIGHT_TEXTURE 1\n";
static const std::string SHADER_DEFINE_SHININESS_TEXTURE    = "#define VE_USE_SHININESS_TEXTURE 1\n";
static const std::string SHADER_DEFINE_OPACITYT_TEXTURE     = "#define VE_USE_OPACITYT_TEXTURE 1\n";
static const std::string SHADER_DEFINE_DISPLACEMENT_TEXTURE = "#define VE_USE_DISPLACEMENT_TEXTURE 1\n";
static const std::string SHADER_DEFINE_LIGHTMAP_TEXTURE     = "#define VE_USE_LIGHTMAP_TEXTURE 1\n";
static const std::string SHADER_DEFINE_REFLECTION_TEXTURE   = "#define VE_USE_REFLECTION_TEXTURE 1\n";

static const std::string SHADER_DEFINE_ATTR_POSITION       = "#define ATTR_POSITION";
static const std::string SHADER_DEFINE_ATTR_NORMAL         = "#define ATTR_NORMAL";
static const std::string SHADER_DEFINE_ATTR_TANGENT        = "#define ATTR_TANGENT";
static const std::string SHADER_DEFINE_ATTR_BITANGENT      = "#define ATTR_BITANGENT";
static const std::string SHADER_DEFINE_ATTR_TEXTURE_COORD  = "#define ATTR_TEXTURE_COORD";
static const std::string SHADER_DEFINE_ATTR_TEXTURE_COORD0 = "#define ATTR_TEXTURE_COORD0";
static const std::string SHADER_DEFINE_ATTR_TEXTURE_COORD1 = "#define ATTR_TEXTURE_COORD1";
static const std::string SHADER_DEFINE_ATTR_TEXTURE_COORD2 = "#define ATTR_TEXTURE_COORD2";
static const std::string SHADER_DEFINE_ATTR_TEXTURE_COORD3 = "#define ATTR_TEXTURE_COORD3";
static const std::string SHADER_DEFINE_ATTR_COLOR          = "#define ATTR_COLOR";
static const std::string SHADER_DEFINE_ATTR_COLOR0         = "#define ATTR_COLOR0";
static const std::string SHADER_DEFINE_ATTR_COLOR1         = "#define ATTR_COLOR1";
static const std::string SHADER_DEFINE_ATTR_COLOR2         = "#define ATTR_COLOR2";
static const std::string SHADER_DEFINE_ATTR_COLOR3         = "#define ATTR_COLOR3";
static const std::string SHADER_DEFINE_ATTR_BONE_INDICES   = "#define ATTR_BONE_INDICES";
static const std::string SHADER_DEFINE_ATTR_BONE_WEIGHTS   = "#define ATTR_BONE_WEIGHTS";
static const std::string SHADER_DEFINE_ATTR_USER0          = "#define ATTR_USER0";
static const std::string SHADER_DEFINE_ATTR_USER1          = "#define ATTR_USER1";

static const std::string SHADER_VERSION                        = "#version";
static const std::string PRECISION_DEFINE_FLOAT                = "precision mediump float;";
static const std::string PRECISION_DEFINE_SAMPLER2DARRAYSHADOW = "precision highp sampler2DArrayShadow;";

static const std::string SHADER_DEFINE_PLATFORM_IOS     = "#define VE_PLATFORM_IOS 1\n";
static const std::string SHADER_DEFINE_PLATFORM_ANDROID = "#define VE_PLATFORM_ANDROID 2\n";
static const std::string SHADER_DEFINE_PLATFORM_WIN32   = "#define VE_PLATFORM_WIN32 3\n";
static const std::string SHADER_DEFINE_PLATFORM_LINUX   = "#define VE_PLATFORM_LINUX 4\n";
static const std::string SHADER_DEFINE_PLATFORM_MAC     = "#define VE_PLATFORM_MAC 5\n";


#endif