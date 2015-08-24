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

static const std::string TECHNIQUES_KEY = "techniques";
static const std::string PASSES_KEY = "passes";
static const std::string DEPTHTEST_KEY = "depthTest";
static const std::string DEPTHWRITE_KEY = "depthWrite";
static const std::string CULLFACE_KEY = "cullFace";
static const std::string SHADERS_KEY = "shaders";
static const std::string VERTEXSHADER_KEY = "VERTEX_SHADER";
static const std::string FRAGMENTSHADER_KEY = "FRAGMENT_SHADER";
static const std::string SOURCE_KEY = "source";
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
static const std::string POINTS_KEY = "POINTS";
static const std::string LINES_KEY = "LINES";
static const std::string TRIANGLES_KEY = "TRIANGLES";

static const std::string VERTEX_ATTRIB_POSITION = "VERTEX_ATTRIB_POSITION";
static const std::string VERTEX_ATTRIB_NORMAL = "VERTEX_ATTRIB_NORMAL";
static const std::string VERTEX_ATTRIB_TANGENT = "VERTEX_ATTRIB_TANGENT";
static const std::string VERTEX_ATTRIB_BITANGENT = "VERTEX_ATTRIB_BITANGENT";
static const std::string VERTEX_ATTRIB_TEX_COORD = "VERTEX_ATTRIB_TEX_COORD";
static const std::string VERTEX_ATTRIB_COLOR = "VERTEX_ATTRIB_COLOR";

//internal unifrom names
static const std::string MV_MATRIX_KEY = "u_ModelViewMat";
static const std::string P_MATRIX_KEY = "u_ProjectMat";
static const std::string MVP_MATRIX_KEY = "u_ModelViewProjectMat";
static const std::string NORMAL_MATRIX_KEY = "u_NormalMat";
static const std::string MV_MATRIX     = "MV_MATRIX";
static const std::string P_MATRIX      = "P_MATRIX";
static const std::string MVP_MATRIX    = "MVP_MATRIX";
static const std::string NORMAL_MATRIX = "NORMAL_MATRIX";

static const std::string MODEL_EXT = ".vem";
static const std::string MATERIAL_EXT = ".vemtl";

#endif