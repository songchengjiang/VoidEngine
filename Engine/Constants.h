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
static const std::string CULLFACE_KEY = "cullFace";
static const std::string BLENDFUNC_KEY = "blendFunc";
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
static const std::string SHADERS_KEY = "shaders";
static const std::string VERTEXSHADER_KEY = "VERTEX_SHADER";
static const std::string FRAGMENTSHADER_KEY = "FRAGMENT_SHADER";
static const std::string SOURCE_KEY = "source";
static const std::string TARGET_KEY = "target";
static const std::string FORMAT_KEY = "format";
static const std::string AMBIENT_KEY = "u_ambient";
static const std::string DIFFUSE_KEY = "u_diffuse";
static const std::string EMISSIVE_KEY = "u_emissive";
static const std::string SPECULAR_KEY = "u_specular";
static const std::string TRANSPARENT_KEY = "u_transparent";
static const std::string REFLECTIVE_KEY = "u_reflective";
static const std::string SHININESS_KEY = "u_shininess";
static const std::string OPACITY_KEY = "u_opacity";
static const std::string REFLECTIVITY_KEY = "u_reflectivity";
static const std::string AMBIENTTEX_KEY = "u_ambientTex";
static const std::string DIFFUSETEX_KEY = "u_diffuseTex";
static const std::string SPECULARTEX_KEY = "u_specularTex";
static const std::string EMISSIVETEX_KEY = "u_emissiveTex";
static const std::string HEIGHTTEX_KEY = "u_heightTex";
static const std::string NORMALSTEX_KEY = "u_normalsTex";
static const std::string SHININESSTEX_KEY = "u_shininessTex";
static const std::string OPACITYTEX_KEY = "u_opacityTex";
static const std::string DISPLACEMENTTEX_KEY = "u_displacementTex";
static const std::string LIGHTMAPTEX_KEY = "u_lightmapTex";
static const std::string REFLECTIONTEX_KEY = "u_reflectionTex";
static const std::string TEXTUREUNITS_KEY = "textureUnits";
static const std::string WRAP_KEY = "wrap";
static const std::string REPEAT_KEY = "REPEAT";
static const std::string CLAMP_KEY = "CLAMP";
static const std::string MIRROR_KEY = "MIRROR";
static const std::string DECAL_KEY = "DECAL";
static const std::string FILTER_KEY = "filter";
static const std::string NEREAST_KEY = "NERAEST";
static const std::string LINEAR_KEY = "LINEAR";
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
static const std::string COLOR_KEY = "COLOR";
static const std::string DEPTH_KEY = "DEPTH";
static const std::string STENCIAL_KEY = "STENCIAL";
static const std::string WIDTH_KEY = "width";
static const std::string HEIGHT_KEY = "height";
static const std::string DRAWMASK_KEY = "drawMask";

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

static const std::string CLASS_KEY = "class";
static const std::string PARAMETERS_KEY = "parameters";
static const std::string VALUES_KEY = "values";
static const std::string LIMIT_KEY = "limit";
static const std::string LIGHT_TEMPLATES_KEY = "lightTemplates";
static const std::string POSITION_KEY = "position";
static const std::string DIRECTION_KEY = "direction";

static const std::string VERTEX_ATTRIB_POSITION = "VERTEX_ATTRIB_POSITION";
static const std::string VERTEX_ATTRIB_NORMAL = "VERTEX_ATTRIB_NORMAL";
static const std::string VERTEX_ATTRIB_TANGENT = "VERTEX_ATTRIB_TANGENT";
static const std::string VERTEX_ATTRIB_BITANGENT = "VERTEX_ATTRIB_BITANGENT";
static const std::string VERTEX_ATTRIB_TEX_COORD = "VERTEX_ATTRIB_TEX_COORD";
static const std::string VERTEX_ATTRIB_BONE_INDICES = "VERTEX_ATTRIB_BONE_INDICES";
static const std::string VERTEX_ATTRIB_BONE_WEIGHTS = "VERTEX_ATTRIB_BONE_WEIGHTS";
static const std::string VERTEX_ATTRIB_COLOR = "VERTEX_ATTRIB_COLOR";
static const std::string VERTEX_ATTRIB_USER = "VERTEX_ATTRIB_USER";

//internal unifrom names
static const std::string M_MATRIX_KEY = "u_ModelMat";
static const std::string MV_MATRIX_KEY = "u_ModelViewMat";
static const std::string P_MATRIX_KEY = "u_ProjectMat";
static const std::string MVP_MATRIX_KEY = "u_ModelViewProjectMat";
static const std::string NORMAL_MATRIX_KEY = "u_NormalMat";
static const std::string BONE_MATRIXES_KEY = "u_BoneMates";
static const std::string M_MATRIX      = "M_MATRIX";
static const std::string MV_MATRIX     = "MV_MATRIX";
static const std::string P_MATRIX      = "P_MATRIX";
static const std::string MVP_MATRIX    = "MVP_MATRIX";
static const std::string NORMAL_MATRIX = "NORMAL_MATRIX";
static const std::string BONE_MATRIXES = "BONE_MATRIXES";

static const std::string MODEL_EXT = ".vem";
static const std::string MATERIAL_EXT = ".vemtl";
static const std::string ANIMATION_EXT = ".veanim";

#endif