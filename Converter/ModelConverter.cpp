#include "ModelConverter.h"
#include <assimp/include/cimport.h>
#include <assimp/include/postprocess.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <string>
#include "Constants.h"

std::vector<std::string> MESH_NAME_LIST;
std::vector<std::string> MATERIAL_NAME_LIST;
std::unordered_map<const aiMaterial*, const aiMesh *> MATERIAL_MESH_MAP;

#define IS_QNAN(f) (f != f)

ModelConverter::ModelConverter()
	: _modelWriter(_modelbuffer)
	, _matWriter(_matBuffer)
	, _animWriter(_animBuffer)
{
	aiLogStream stream;
	// get a handle to the predefined STDOUT log stream and attach
	// it to the logging system. It remains active for all further
	// calls to aiImportFile(Ex) and aiApplyPostProcessing.
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
	aiAttachLogStream(&stream);
}

ModelConverter::~ModelConverter()
{

}

void ModelConverter::convert(const std::string &filePath, const std::string &outFile)
{
	auto scene = aiImportFile(filePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene) return;

	setOutputFileInfo(filePath, outFile);

	writeModel(scene);
	writeMaterials(scene);
	writeAnimations(scene);

	outputFiles();
	//rapidjson::StringBuffer s;
	//rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);

	//writer.StartObject();
	//writer.String("hello");
	//writer.String("world");
	//writer.String("t");
	//writer.Bool(true);
	//writer.String("f");
	//writer.Bool(false);
	//writer.String("n");
	//writer.Null();
	//writer.String("i");
	//writer.Uint(123);
	//writer.String("pi");
	//writer.Double(3.1416);
	//writer.String("a");
	//writer.StartArray();
	//for (unsigned i = 0; i < 4; i++)
	//	writer.Uint(i);
	//writer.EndArray();
	//writer.EndObject();

	//std::cout << _buffer.GetString() << std::endl;
}

void ModelConverter::writeModel(const aiScene *scene)
{
	if (!scene->HasMeshes()) return;
	generateMeshNames(scene);
	generateMaterialNames(scene);
	generateMaterialMeshMap(scene);
	_modelWriter.StartObject();
	_modelWriter.String(VERSION_KEY.c_str(), VERSION_KEY.size()); _modelWriter.String(CT_VERSION);
	_modelWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _modelWriter.String(_outputModelName.c_str(), _outputModelName.size());
	if (scene->HasMaterials()) {
		_modelWriter.String(MATERIALS_KEY.c_str(), MATERIALS_KEY.size()); _modelWriter.String(_outputMaterialName.c_str(), _outputMaterialName.size());
	}	
	if (scene->HasAnimations()) {
		_modelWriter.String(ANIMATIONS_KEY.c_str(), ANIMATIONS_KEY.size()); _modelWriter.String(_outputAnimationName.c_str(), _outputAnimationName.size());
	}

	_modelWriter.String(MESHES_KEY.c_str(), MESHES_KEY.size());
	_modelWriter.StartArray();
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i){
		writeMesh(scene->mMeshes[i], MESH_NAME_LIST[i]);
		MATERIAL_MESH_MAP[scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]] = scene->mMeshes[i];
	}
	_modelWriter.EndArray();

	_modelWriter.String(NODES_KEY.c_str(), NODES_KEY.size());
	_modelWriter.StartArray();
	writeNode(scene->mRootNode);
	_modelWriter.EndArray();

	_modelWriter.EndObject();
}

void ModelConverter::writeMesh(const aiMesh *mesh, const std::string &name)
{
	_modelWriter.StartObject();
	_modelWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _modelWriter.String(name.c_str());
	_modelWriter.String(MATERIAL_KEY.c_str(), MATERIAL_KEY.size()); _modelWriter.String(MATERIAL_NAME_LIST[mesh->mMaterialIndex].c_str());

	_modelWriter.String(ATTRIBUTES_KEY.c_str(), ATTRIBUTES_KEY.size());
	_modelWriter.StartArray();
	writeMeshAttributes(mesh);
	_modelWriter.EndArray();

	_modelWriter.String(VERTICES_KEY.c_str(), VERTICES_KEY.size());
	_modelWriter.StartArray();
	writeMeshVertices(mesh);
	_modelWriter.EndArray();

	_modelWriter.String(PARTS_KEY.c_str(), PARTS_KEY.size());
	_modelWriter.StartArray();
	writeMeshPairs(mesh);
	_modelWriter.EndArray();

	if (mesh->HasBones()) {
		_modelWriter.String(BONES_KEY.c_str(), BONES_KEY.size());
		_modelWriter.StartArray();
		writeMeshBones(mesh);
		_modelWriter.EndArray();
	}

	_modelWriter.EndObject();
}

void ModelConverter::writeMeshAttributes(const aiMesh *mesh)
{
	if (mesh->HasPositions()){
		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(3);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(VERTEX_ATTRIB_POSITION.c_str(), VERTEX_ATTRIB_POSITION.size());
		_modelWriter.EndObject();
	}

	if (mesh->HasNormals()){
		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(3);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(VERTEX_ATTRIB_NORMAL.c_str(), VERTEX_ATTRIB_NORMAL.size());
		_modelWriter.EndObject();
	}

	if (mesh->HasTangentsAndBitangents()){
		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(3);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(VERTEX_ATTRIB_TANGENT.c_str(), VERTEX_ATTRIB_TANGENT.size());
		_modelWriter.EndObject();

		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(3);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(VERTEX_ATTRIB_BITANGENT.c_str(), VERTEX_ATTRIB_BITANGENT.size());
		_modelWriter.EndObject();
	}

	char str[32];
	unsigned int uvNum = mesh->GetNumUVChannels();
	for (unsigned int i = 0; i < uvNum; ++i){
		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(2);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		sprintf(str, "%s%d", VERTEX_ATTRIB_TEX_COORD.c_str(), i);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(str);
		_modelWriter.EndObject();
	}

	unsigned int colNum = mesh->GetNumColorChannels();
	for (unsigned int i = 0; i < colNum; ++i){
		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(4);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		sprintf(str, "%s%d", VERTEX_ATTRIB_COLOR.c_str(), i);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(str);
		_modelWriter.EndObject();
	}

	if (mesh->HasBones()) {
		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(4);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(VERTEX_ATTRIB_BONE_INDICES.c_str(), VERTEX_ATTRIB_BONE_INDICES.size());
		_modelWriter.EndObject();

		_modelWriter.StartObject();
		_modelWriter.String(SIZE_KEY.c_str(), SIZE_KEY.size()); _modelWriter.Int(4);
		_modelWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size()); _modelWriter.String("FLOAT", 5);
		_modelWriter.String(ATTRIBUTE_KEY.c_str(), ATTRIBUTE_KEY.size()); _modelWriter.String(VERTEX_ATTRIB_BONE_WEIGHTS.c_str(), VERTEX_ATTRIB_BONE_WEIGHTS.size());
		_modelWriter.EndObject();
	}

}

void ModelConverter::writeMeshVertices(const aiMesh *mesh)
{
	unsigned int uvNum = mesh->GetNumUVChannels();
	unsigned int colNum = mesh->GetNumColorChannels();

	std::vector< std::pair<aiVector4D, aiVector4D> > results;
	if (mesh->HasBones()) {
		collectBoneIndiecsAndBoneWeights(mesh, results);
	}

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i){
		//position
		_modelbuffer.Put('\n');
		_modelWriter.WriteIndent();

		_modelWriter.Float(mesh->mVertices[i].x);
		_modelWriter.Float(mesh->mVertices[i].y);
		_modelWriter.Float(mesh->mVertices[i].z);

		//normal
		if (mesh->HasNormals()){
            _modelWriter.Float(IS_QNAN(mesh->mNormals[i].x)? 0.0f: mesh->mNormals[i].x);
			_modelWriter.Float(IS_QNAN(mesh->mNormals[i].y)? 0.0f: mesh->mNormals[i].y);
			_modelWriter.Float(IS_QNAN(mesh->mNormals[i].z)? 0.0f: mesh->mNormals[i].z);
		}

		//tangents and bitangents
		if (mesh->HasTangentsAndBitangents()){
			_modelWriter.Float(IS_QNAN(mesh->mTangents[i].x)? 0.0f: mesh->mTangents[i].x);
			_modelWriter.Float(IS_QNAN(mesh->mTangents[i].y)? 0.0f: mesh->mTangents[i].y);
			_modelWriter.Float(IS_QNAN(mesh->mTangents[i].z)? 0.0f: mesh->mTangents[i].z);

			_modelWriter.Float(IS_QNAN(mesh->mBitangents[i].x)? 0.0f: mesh->mBitangents[i].x);
			_modelWriter.Float(IS_QNAN(mesh->mBitangents[i].y)? 0.0f: mesh->mBitangents[i].y);
			_modelWriter.Float(IS_QNAN(mesh->mBitangents[i].z)? 0.0f: mesh->mBitangents[i].z);
		}

		//uvs
		for (unsigned int uv = 0; uv < uvNum; ++uv){
			_modelWriter.Float(mesh->mTextureCoords[uv][i].x);
			_modelWriter.Float(mesh->mTextureCoords[uv][i].y);
		}

		//colors
		for (unsigned int col = 0; col < colNum; ++col){
			_modelWriter.Float(mesh->mColors[col][i].r);
			_modelWriter.Float(mesh->mColors[col][i].g);
			_modelWriter.Float(mesh->mColors[col][i].b);
			_modelWriter.Float(mesh->mColors[col][i].a);
		}

		//boneindices and boneweights
		if (mesh->HasBones()) {
			auto indexAndweight = results[i];
			_modelWriter.Uint(indexAndweight.first[0]);
			_modelWriter.Uint(indexAndweight.first[1]);
			_modelWriter.Uint(indexAndweight.first[2]);
			_modelWriter.Uint(indexAndweight.first[3]);

			_modelWriter.Float(indexAndweight.second[0]);
			_modelWriter.Float(indexAndweight.second[1]);
			_modelWriter.Float(indexAndweight.second[2]);
			_modelWriter.Float(indexAndweight.second[3]);
		}
	}
}

void ModelConverter::writeMeshPairs(const aiMesh *mesh)
{
	unsigned int currentNumIndices = 0;
	unsigned int writeCount = 0;
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i){
		auto face = mesh->mFaces[i];
		if (currentNumIndices != face.mNumIndices){
			if (currentNumIndices != 0){
				_modelWriter.EndArray();
				_modelWriter.EndObject();
			}
			_modelWriter.StartObject();
			_modelWriter.String(TYPE_KEY.c_str(), SIZE_KEY.size()); 
			if (face.mNumIndices == 1) _modelWriter.String(POINTS_KEY.c_str(), POINTS_KEY.size());
			else if (face.mNumIndices == 2) _modelWriter.String(LINES_KEY.c_str(), LINES_KEY.size());
			else if (face.mNumIndices == 3) _modelWriter.String(TRIANGLES_KEY.c_str(), TRIANGLES_KEY.size());
			_modelWriter.String(INDICES_KEY.c_str(), INDICES_KEY.size());
			_modelWriter.StartArray();
			currentNumIndices = face.mNumIndices;
		}
		
		if (23 < writeCount){
			_modelbuffer.Put('\n');
			_modelWriter.WriteIndent();
			writeCount = 0;
		}
		for (unsigned int f = 0; f < face.mNumIndices; ++f){
			_modelWriter.Uint(face.mIndices[f]);
			++writeCount;
		}
	}

	_modelWriter.EndArray();
	_modelWriter.EndObject();
}

void ModelConverter::writeMeshBones(const aiMesh *mesh)
{
	for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
		_modelWriter.StartObject();
		auto &bone = mesh->mBones[i];
		_modelWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _modelWriter.String(bone->mName.C_Str(), bone->mName.length);
		_modelWriter.String(TRANSFORM_KEY.c_str(), TRANSFORM_KEY.size());
		writeMat4(_modelWriter, &bone->mOffsetMatrix);
		//_modelWriter.String(WEIGHTS_KEY.c_str(), WEIGHTS_KEY.size());
		//_modelWriter.StartArray();
		//for (unsigned int w = 0; w < bone->mNumWeights; ++w) {
		//	auto &wt = bone->mWeights[w];
		//	_modelbuffer.Put('\n');
		//	_modelWriter.WriteIndent();
		//	_modelWriter.Uint(wt.mVertexId);
		//	_modelWriter.Float(wt.mWeight);
		//}
		//_modelWriter.EndArray();
		_modelWriter.EndObject();
	}
}

void ModelConverter::writeNode(const aiNode *node)
{
	_modelWriter.StartObject();

	_modelWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _modelWriter.String(node->mName.C_Str(), node->mName.length);

	_modelWriter.String(TRANSFORM_KEY.c_str(), TRANSFORM_KEY.size());
	writeMat4(_modelWriter, &node->mTransformation);

	if (node->mNumMeshes){
		_modelWriter.String(MESHES_KEY.c_str(), MESHES_KEY.size());
		_modelWriter.StartArray();
		for (unsigned int i = 0; i < node->mNumMeshes; ++i){
			_modelWriter.String(MESH_NAME_LIST[node->mMeshes[i]].c_str(), MESH_NAME_LIST[node->mMeshes[i]].size());
		}
		_modelWriter.EndArray();
	}

	if (node->mNumChildren){
		_modelWriter.String(NODES_KEY.c_str(), NODES_KEY.size());
		_modelWriter.StartArray();
		for (unsigned int i = 0; i < node->mNumChildren; ++i){
			writeNode(node->mChildren[i]);
		}
		_modelWriter.EndArray();
	}
	_modelWriter.EndObject();
}

void ModelConverter::writeMaterials(const aiScene *scene)
{
	if (!scene->HasMaterials()) return;
	_matWriter.StartObject();
	_matWriter.String(VERSION_KEY.c_str(), VERSION_KEY.size()); _matWriter.String(CT_VERSION);
	_matWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _matWriter.String(_outputMaterialName.c_str(), _outputMaterialName.size());
	_matWriter.String(MATERIALS_KEY.c_str(), MATERIALS_KEY.size());
	_matWriter.StartArray();
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i){
		writeMaterial(scene->mMaterials[i], MATERIAL_NAME_LIST[i]);
	}
	_matWriter.EndArray();

	_matWriter.EndObject();
}

void ModelConverter::writeMaterial(const aiMaterial *mat, const std::string &name)
{
	_matWriter.StartObject();
	_matWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _matWriter.String(name.c_str(), name.size());
	_matWriter.String(TECHNIQUES_KEY.c_str(), TECHNIQUES_KEY.size());
	_matWriter.StartArray();
	writeTechnique(mat);
	_matWriter.EndArray();
	_matWriter.EndObject();
}

void ModelConverter::writeTechnique(const aiMaterial *mat)
{
	_matWriter.StartObject();
	_matWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _matWriter.String("tech-0", 6);
	_matWriter.String(PASSES_KEY.c_str(), PASSES_KEY.size());
	_matWriter.StartArray();
	writePass(mat);
	_matWriter.EndArray();
	_matWriter.EndObject();
}

void ModelConverter::writePass(const aiMaterial *mat)
{
	int val = 0;
	_matWriter.StartObject();
	writeRenderState(mat);
    writeUniforms(mat);
	writeShaders(mat);
	writeTextures(mat);
	_matWriter.EndObject();
}

void ModelConverter::writeRenderState(const aiMaterial *mat)
{
	_matWriter.String(DEPTHTEST_KEY.c_str(), DEPTHTEST_KEY.size()); _matWriter.Bool(true);
	_matWriter.String(DEPTHWRITE_KEY.c_str(), DEPTHWRITE_KEY.size()); _matWriter.Bool(true);
	_matWriter.String(CULLFACE_KEY.c_str(), CULLFACE_KEY.size()); _matWriter.Bool(true);
}

void ModelConverter::writeUniforms(const aiMaterial *mat)
{
    _matWriter.String(UNIFORMS_KEY.c_str(), UNIFORMS_KEY.size());
    _matWriter.StartObject();
    
    _matWriter.String(MVP_MATRIX_KEY.c_str(), MVP_MATRIX_KEY.size());
    _matWriter.String(MVP_MATRIX.c_str(), MVP_MATRIX.size());
    _matWriter.String(MV_MATRIX_KEY.c_str(), MV_MATRIX_KEY.size());
    _matWriter.String(MV_MATRIX.c_str(), MV_MATRIX.size());
    _matWriter.String(M_MATRIX_KEY.c_str(), M_MATRIX_KEY.size());
    _matWriter.String(M_MATRIX.c_str(), M_MATRIX.size());
    _matWriter.String(NORMAL_MATRIX_KEY.c_str(), NORMAL_MATRIX_KEY.size());
    _matWriter.String(NORMAL_MATRIX.c_str(), NORMAL_MATRIX.size());
    _matWriter.String(NORMAL_WORLD_MATRIX_KEY.c_str(), NORMAL_WORLD_MATRIX_KEY.size());
    _matWriter.String(NORMAL_WROLD_MATRIX.c_str(), NORMAL_WROLD_MATRIX.size());
    _matWriter.String(CAMERA_WORLD_POS_KEY.c_str(), CAMERA_WORLD_POS_KEY.size());
    _matWriter.String(CAMERA_WORLD_POS.c_str(), CAMERA_WORLD_POS.size());
    
    if (MATERIAL_MESH_MAP[mat]->HasBones()) {
        _matWriter.String(BONE_MATRIXES_KEY.c_str(), BONE_MATRIXES_KEY.size());
        _matWriter.String(BONE_MATRIXES.c_str(), BONE_MATRIXES.size());
    }
    
    _matWriter.String(LIGHTMASK_KEY.c_str(), LIGHTMASK_KEY.size());
    _matWriter.Float(1.0f);
    
    aiColor4D col;
    
    aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &col);
    if (!col.IsBlack()){
        _matWriter.String(DIFFUSE_KEY.c_str(), DIFFUSE_KEY.size());
        aiVector3D vec3Col(col.r, col.g, col.b);
        writeVec3(_matWriter, &vec3Col);
    }
    
    aiGetMaterialColor(mat, AI_MATKEY_COLOR_EMISSIVE, &col);
    if (!col.IsBlack()){
        _matWriter.String(EMISSIVE_KEY.c_str(), EMISSIVE_KEY.size());
        aiVector3D vec3Col(col.r, col.g, col.b);
        writeVec3(_matWriter, &vec3Col);
    }
    
    float valFloat = 0.0f;
    aiGetMaterialFloat(mat, AI_MATKEY_SHININESS, &valFloat);
    _matWriter.String(ROUGHNESS_KEY.c_str(), ROUGHNESS_KEY.size());
    _matWriter.Float(valFloat <= 128.0f? 1.0f - valFloat / 128.0f: 0.0f);
    _matWriter.String(METALLIC_KEY.c_str(), METALLIC_KEY.size());
    _matWriter.Float(valFloat <= 128.0f ? valFloat / 128.0f : 1.0f);
    
    aiGetMaterialFloat(mat, AI_MATKEY_OPACITY, &valFloat);
    _matWriter.String(OPACITY_KEY.c_str(), OPACITY_KEY.size());
    _matWriter.Float(valFloat);
    
    if (hasTexture(mat, aiTextureType_DISPLACEMENT)) {
        _matWriter.String(DISPLACEMENT_KEY.c_str(), DISPLACEMENT_KEY.size());
        _matWriter.Float(0.1f);
    }
    
    unsigned int texUnit = 0;
    
    if (hasTexture(mat, aiTextureType_DIFFUSE)){
        _matWriter.String(DIFFUSETEX_KEY.c_str(), DIFFUSETEX_KEY.size());
        _matWriter.Uint(texUnit++);
    }
    
    if (hasTexture(mat, aiTextureType_EMISSIVE)){
        _matWriter.String(EMISSIVETEX_KEY.c_str(), EMISSIVETEX_KEY.size());
        _matWriter.Uint(texUnit++);
    }
    
    if (hasTexture(mat, aiTextureType_NORMALS)){
        _matWriter.String(NORMALTEX_KEY.c_str(), NORMALTEX_KEY.size());
        _matWriter.Uint(texUnit++);
    }
    
    if (hasTexture(mat, aiTextureType_OPACITY)){
        _matWriter.String(OPACITYTEX_KEY.c_str(), OPACITYTEX_KEY.size());
        _matWriter.Uint(texUnit++);
    }
    
    if (hasTexture(mat, aiTextureType_DISPLACEMENT)){
        _matWriter.String(DISPLACEMENTTEX_KEY.c_str(), DISPLACEMENTTEX_KEY.size());
        _matWriter.Uint(texUnit++);
    }
    
    if (hasTexture(mat, aiTextureType_LIGHTMAP)){
        _matWriter.String(LIGHTMAPTEX_KEY.c_str(), LIGHTMAPTEX_KEY.size());
        _matWriter.Uint(texUnit++);
    }
    
    _matWriter.EndObject();
}

void ModelConverter::writeShaders(const aiMaterial *mat)
{
	int valInt = 0;
	aiGetMaterialInteger(mat, AI_MATKEY_SHADING_MODEL, &valInt);
	writeShader(mat, std::string("system/") + getShaderName(valInt));
}

void ModelConverter::writeShader(const aiMaterial *mat, const std::string &shaderName)
{
	_matWriter.String(SHADERS_KEY.c_str(), SHADERS_KEY.size());
	_matWriter.StartArray();

	//vertex shader
	_matWriter.StartObject();
	_matWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size());
	_matWriter.String(VERTEXSHADER_KEY.c_str(), VERTEXSHADER_KEY.size());
	_matWriter.String(DEFINATION_KEY.c_str(), DEFINATION_KEY.size());
	_matWriter.String(getShaderDefinations(mat, VERTEXSHADER_KEY).c_str());
	_matWriter.String(SOURCE_KEY.c_str(), SOURCE_KEY.size());
	_matWriter.String((shaderName + std::string(".vert")).c_str());
	_matWriter.EndObject();

	//fragment shader
	_matWriter.StartObject();
	_matWriter.String(TYPE_KEY.c_str(), TYPE_KEY.size());
	_matWriter.String(FRAGMENTSHADER_KEY.c_str(), FRAGMENTSHADER_KEY.size());
	_matWriter.String(DEFINATION_KEY.c_str(), DEFINATION_KEY.size());
	_matWriter.String(getShaderDefinations(mat, FRAGMENTSHADER_KEY).c_str());
	_matWriter.String(SOURCE_KEY.c_str(), SOURCE_KEY.size());
	_matWriter.String((shaderName + std::string(".frag")).c_str());
	_matWriter.EndObject();

	_matWriter.EndArray();
}

void ModelConverter::writeTextures(const aiMaterial *mat)
{
	aiString texPath;
	_matWriter.String(TEXTUREUNITS_KEY.c_str(), TEXTUREUNITS_KEY.size());
	_matWriter.StartArray();
	writeTexture(mat, aiTextureType_DIFFUSE);
	writeTexture(mat, aiTextureType_EMISSIVE);
	writeTexture(mat, aiTextureType_NORMALS);
	writeTexture(mat, aiTextureType_OPACITY);
	writeTexture(mat, aiTextureType_DISPLACEMENT);
	writeTexture(mat, aiTextureType_LIGHTMAP);
	_matWriter.EndArray();
}

void ModelConverter::writeTexture(const aiMaterial *mat, aiTextureType texType)
{
	aiString texturePath;
	aiTextureMapMode mapMode = aiTextureMapMode_Wrap;
	unsigned int ct = mat->GetTextureCount(texType);
	mat->GetTexture(texType, 0, &texturePath, 0, 0, 0, 0, &mapMode);
	if (0 < texturePath.length){
        std::string texturePathSTD = texturePath.C_Str();
        std::string textureFileName = texturePathSTD.substr(texturePathSTD.find_last_of("\\/") + 1);
		_matWriter.StartObject();
		_matWriter.String(NAME_KEY.c_str(), NAME_KEY.size());
		_matWriter.String(textureFileName.c_str(), textureFileName.size());
		_matWriter.String(SOURCE_KEY.c_str(), SOURCE_KEY.size()); 
		_matWriter.String(textureFileName.c_str(), textureFileName.size());
		_matWriter.String(TEXTYPE_KEY.c_str(), TEXTYPE_KEY.size());
		_matWriter.String(getTextureTypeName(texType).c_str());

		_matWriter.String(WRAP_KEY.c_str(), WRAP_KEY.size());
		if (mapMode == aiTextureMapMode_Wrap) _matWriter.String(REPEAT_KEY.c_str(), REPEAT_KEY.size());
		else if (mapMode == aiTextureMapMode_Clamp) _matWriter.String(CLAMP_KEY.c_str(), CLAMP_KEY.size());
		else if (mapMode == aiTextureMapMode_Mirror) _matWriter.String(MIRROR_KEY.c_str(), MIRROR_KEY.size());
		else if (mapMode == aiTextureMapMode_Decal) _matWriter.String(DECAL_KEY.c_str(), DECAL_KEY.size());
		else _matWriter.String(CLAMP_KEY.c_str(), CLAMP_KEY.size());

		_matWriter.String(FILTER_KEY.c_str(), FILTER_KEY.size());
		_matWriter.String(NEAREST_MIP_MAP_LINEAR_KEY.c_str(), NEAREST_MIP_MAP_LINEAR_KEY.size());
		_matWriter.EndObject();
	}
}

void ModelConverter::writeAnimations(const aiScene *scene)
{
	if (!scene->HasAnimations()) return;

	_animWriter.StartObject();
	_animWriter.String(VERSION_KEY.c_str(), VERSION_KEY.size()); _animWriter.String(CT_VERSION);
	_animWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _animWriter.String(_outputAnimationName.c_str(), _outputAnimationName.size());
	_animWriter.String(ANIMATIONS_KEY.c_str(), ANIMATIONS_KEY.size());
	_animWriter.StartArray();
	for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
		writeAnimation(scene->mAnimations[i]);
	}
	_animWriter.EndArray();

	_animWriter.EndObject();
}

void ModelConverter::writeAnimation(const aiAnimation *animation)
{
	_animWriter.StartObject();
	_animWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _animWriter.String(animation->mName.C_Str(), animation->mName.length);
	_animWriter.String(FRAMES_KEY.c_str(), FRAMES_KEY.size()); _animWriter.Double(animation->mDuration);
	_animWriter.String(FRAMERATE_KEY.c_str(), FRAMERATE_KEY.size()); _animWriter.Double(animation->mTicksPerSecond);
	_animWriter.String(NODES_KEY.c_str(), NODES_KEY.size());
	_animWriter.StartArray();
	for (unsigned int i = 0; i < animation->mNumChannels; ++i) {
		writeNodeAnim(animation->mChannels[i]);
	}
	_animWriter.EndArray();
	_animWriter.EndObject();
}

void ModelConverter::writeNodeAnim(const aiNodeAnim *nodeAnim)
{
	_animWriter.StartObject();
	_animWriter.String(NAME_KEY.c_str(), NAME_KEY.size()); _animWriter.String(nodeAnim->mNodeName.C_Str(), nodeAnim->mNodeName.length);

	_animWriter.String(POSITIONS_KEY.c_str(), POSITIONS_KEY.size());
	_animWriter.StartArray();
	for (unsigned int i = 0; i < nodeAnim->mNumPositionKeys; ++i) {
		auto positions = nodeAnim->mPositionKeys[i];
		_animBuffer.Put('\n');
		_animWriter.WriteIndent();
		_animWriter.Float(positions.mTime); _animWriter.Float(positions.mValue.x); _animWriter.Float(positions.mValue.y); _animWriter.Float(positions.mValue.z);
	}
	_animWriter.EndArray();

	_animWriter.String(ROTATIONS_KEY.c_str(), ROTATIONS_KEY.size());
	_animWriter.StartArray();
	for (unsigned int i = 0; i < nodeAnim->mNumRotationKeys; ++i) {
		auto rotations = nodeAnim->mRotationKeys[i];
		_animBuffer.Put('\n');
		_animWriter.WriteIndent();
		_animWriter.Float(rotations.mTime); _animWriter.Float(rotations.mValue.w); _animWriter.Float(rotations.mValue.x); _animWriter.Float(rotations.mValue.y); _animWriter.Float(rotations.mValue.z);
	}
	_animWriter.EndArray();

	_animWriter.String(SCALES_KEY.c_str(), SCALES_KEY.size());
	_animWriter.StartArray();
	for (unsigned int i = 0; i < nodeAnim->mNumScalingKeys; ++i) {
		auto scales = nodeAnim->mScalingKeys[i];
		_animBuffer.Put('\n');
		_animWriter.WriteIndent();
		_animWriter.Float(scales.mTime);  _animWriter.Float(scales.mValue.x); _animWriter.Float(scales.mValue.y); _animWriter.Float(scales.mValue.z);
	}
	_animWriter.EndArray();

	_animWriter.EndObject();
}

void ModelConverter::writeVec4(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiColor4D *col)
{
	writer.StartArray();
	writer.Float(col->r); writer.Float(col->g); writer.Float(col->b); writer.Float(col->a);
	writer.EndArray();
}

void ModelConverter::writeVec3(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiVector3D *vec3)
{
	writer.StartArray();
	writer.Float(vec3->x); writer.Float(vec3->y); writer.Float(vec3->z);
	writer.EndArray();
}

void ModelConverter::writeVec2(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiVector2D *vec2)
{
	writer.StartArray();
	writer.Float(vec2->x); writer.Float(vec2->y);
	writer.EndArray();
}

void ModelConverter::writeMat4(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiMatrix4x4 *mat4)
{
	writer.StartArray();
	//writer.Float(mat4->a1); writer.Float(mat4->b1); writer.Float(mat4->c1); writer.Float(mat4->d1);
	//writer.Float(mat4->a2); writer.Float(mat4->b2); writer.Float(mat4->c2); writer.Float(mat4->d2);
	//writer.Float(mat4->a3); writer.Float(mat4->b3); writer.Float(mat4->c3); writer.Float(mat4->d3);
	//writer.Float(mat4->a4); writer.Float(mat4->b4); writer.Float(mat4->c4); writer.Float(mat4->d4);

	writer.Float(mat4->a1); writer.Float(mat4->a2); writer.Float(mat4->a3); writer.Float(mat4->a4);
	writer.Float(mat4->b1); writer.Float(mat4->b2); writer.Float(mat4->b3); writer.Float(mat4->b4);
	writer.Float(mat4->c1); writer.Float(mat4->c2); writer.Float(mat4->c3); writer.Float(mat4->c4);
	writer.Float(mat4->d1); writer.Float(mat4->d2); writer.Float(mat4->d3); writer.Float(mat4->d4);
	writer.EndArray();
}

bool ModelConverter::hasTexture(const aiMaterial *mat, aiTextureType texType)
{
	return 0 < mat->GetTextureCount(texType);
}

void ModelConverter::collectBoneIndiecsAndBoneWeights(const aiMesh *mesh, std::vector< std::pair<aiVector4D, aiVector4D> > &results)
{
	results.assign(mesh->mNumVertices, std::make_pair(aiVector4D(0.0f), aiVector4D(0.0f)));
	std::vector<unsigned int> indices;
	indices.assign(mesh->mNumVertices, 0);
	for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
		const auto &bone = mesh->mBones[i];
		for (unsigned int w = 0; w < bone->mNumWeights; ++w) {
			const auto &weight = bone->mWeights[w];
			auto &indicesAndweights = results[weight.mVertexId];
			indicesAndweights.first[indices[weight.mVertexId]] = i;
			indicesAndweights.second[indices[weight.mVertexId]] = weight.mWeight;
			++indices[weight.mVertexId];
		}
	}
}

std::string ModelConverter::getTextureTypeName(aiTextureType texType)
{
	switch (texType)
	{

	case aiTextureType_DIFFUSE:
		return DIFFUSE_TEXTURE_KEY;

	case aiTextureType_EMISSIVE:
		return EMISSIVE_TEXTURE_KEY;

	case aiTextureType_NORMALS:
		return NORMAL_TEXTURE_KEY;

	case aiTextureType_OPACITY:
		return OPACITYT_TEXTURE_KEY;

	case aiTextureType_DISPLACEMENT:
		return DISPLACEMENT_TEXTURE_KEY;

	case aiTextureType_LIGHTMAP:
		return LIGHTMAP_TEXTURE_KEY;
            
	}

	return DIFFUSE_TEXTURE_KEY;
}

std::string ModelConverter::getShaderDefinations(const aiMaterial *mat, const std::string &type)
{
	std::string definations;

	const aiMesh *mesh = MATERIAL_MESH_MAP[mat];
	if (type == VERTEXSHADER_KEY) {
		if (mesh->HasBones()) {
			definations += SHADER_DEFINE_BONES;
		}

		unsigned short attributeIndex = 0;
		char str[8];
		if (mesh->HasPositions()) {
			sprintf(str, " %d\n", attributeIndex);
			definations += SHADER_DEFINE_ATTR_POSITION + str;
			++attributeIndex;
		}

		if (mesh->HasNormals()) {
			sprintf(str, " %d\n", attributeIndex);
			definations += SHADER_DEFINE_ATTR_NORMAL + str;
			++attributeIndex;
		}

		if (mesh->HasTangentsAndBitangents()) {
			sprintf(str, " %d\n", attributeIndex);
			definations += SHADER_DEFINE_ATTR_TANGENT + str;
			++attributeIndex;

			sprintf(str, " %d\n", attributeIndex);
			definations += SHADER_DEFINE_ATTR_BITANGENT + str;
			++attributeIndex;
		}

		unsigned int uvNum = mesh->GetNumUVChannels();
		uvNum = uvNum <= 4 ? uvNum : 4;
		for (unsigned int i = 0; i < uvNum; ++i) {
			sprintf(str, "%d %d\n", i, attributeIndex);
			definations += SHADER_DEFINE_ATTR_TEXTURE_COORD + str;
			++attributeIndex;
		}

		unsigned int colNum = mesh->GetNumColorChannels();
		colNum = colNum <= 4 ? colNum : 4;
		for (unsigned int i = 0; i < colNum; ++i) {
			sprintf(str, "%d %d\n", i, attributeIndex);
			definations += SHADER_DEFINE_ATTR_COLOR + str;
			++attributeIndex;
		}

		if (mesh->HasBones()) {
			sprintf(str, " %d\n", attributeIndex);
			definations += SHADER_DEFINE_ATTR_BONE_INDICES + str;
			++attributeIndex;

			sprintf(str, " %d\n", attributeIndex);
			definations += SHADER_DEFINE_ATTR_BONE_WEIGHTS + str;
			++attributeIndex;
		}
	}
	else if (type == FRAGMENTSHADER_KEY) {
		bool hasTex = false;
		if (hasTexture(mat, aiTextureType_AMBIENT)) {
			definations += SHADER_DEFINE_AMBIENT_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_DIFFUSE)) {
			definations += SHADER_DEFINE_DIFFUSE_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_SPECULAR)) {
			definations += SHADER_DEFINE_SPECULAR_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_EMISSIVE)) {
			definations += SHADER_DEFINE_EMISSIVE_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_NORMALS)) {
			definations += SHADER_DEFINE_NORMAL_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_HEIGHT)) {
			definations += SHADER_DEFINE_HEIGHT_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_SHININESS)) {
			definations += SHADER_DEFINE_SHININESS_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_OPACITY)) {
			definations += SHADER_DEFINE_OPACITYT_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_DISPLACEMENT)) {
			definations += SHADER_DEFINE_DISPLACEMENT_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_LIGHTMAP)) {
			definations += SHADER_DEFINE_LIGHTMAP_TEXTURE;
			hasTex = true;
		}

		if (hasTexture(mat, aiTextureType_REFLECTION)) {
			definations += SHADER_DEFINE_REFLECTION_TEXTURE;
			hasTex = true;
		}

		if (hasTex) {
			definations += SHADER_DEFINE_TEXTURES;
		}
	}

	bool useTangentSpace = false;
	if (mesh->HasTangentsAndBitangents() && hasTexture(mat, aiTextureType_NORMALS)) {
		definations += SHADER_DEFINE_NROMAL_MAPPING;
	}

	if (mesh->HasTangentsAndBitangents() && hasTexture(mat, aiTextureType_DISPLACEMENT)) {
		definations += SHADER_DEFINE_PARALLAX_MAPPING;
	}

	return definations;
}

void ModelConverter::generateMeshNames(const aiScene *scene)
{
	MESH_NAME_LIST.resize(scene->mNumMeshes);
	char str[64];
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i){
		if (scene->mMeshes[i]->mName.length == 0){
			sprintf(str, "mesh-%d", i);
			MESH_NAME_LIST[i] = str;
		}
		else{
			MESH_NAME_LIST[i] = scene->mMeshes[i]->mName.C_Str();
		}
		
	}
}

void ModelConverter::setOutputFileInfo(const std::string &filePath, const std::string &outFile)
{
	std::string outFilePath;
	if (outFile.empty())
		outFilePath = filePath;
	else
		outFilePath = outFile;

	std::string::size_type s = outFilePath.find_last_of("/\\") + 1;
	std::string::size_type e = outFilePath.find_last_of(".");
	_outputFileFolder = outFilePath.substr(0, s);
	_outputModelName = outFilePath.substr(s, e - s) + MODEL_EXT;
	_outputMaterialName = outFilePath.substr(s, e - s) + MATERIAL_EXT;
	_outputAnimationName = outFilePath.substr(s, e - s) + ANIMATION_EXT;
}

void ModelConverter::generateMaterialNames(const aiScene *scene)
{
	MATERIAL_NAME_LIST.resize(scene->mNumMaterials);
	char str[64];
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i){
		sprintf(str, "mat-%d", i);
		MATERIAL_NAME_LIST[i] = str;
	}
}

void ModelConverter::generateMaterialMeshMap(const aiScene *scene)
{
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		MATERIAL_MESH_MAP[scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]] = scene->mMeshes[i];
	}
}

std::string ModelConverter::getShaderName(int shaderMode)
{
	switch (shaderMode)
	{
	case aiShadingMode_Flat:
		return "FlatShader";

	case aiShadingMode_Gouraud:
		return "GouraudShader";

	case aiShadingMode_Phong:
		return "PhongShader";

	case aiShadingMode_Blinn:
		return "BlinnShader";

	case aiShadingMode_Toon:
		return "ToonShader";

	case aiShadingMode_OrenNayar:
		return "OrenNayarShader";

	case aiShadingMode_Minnaert:
		return "MinnaertShader";

	case aiShadingMode_CookTorrance:
		return "CookTorranceShader";

	case aiShadingMode_Fresnel:
		return "FresnelShader";

	case aiShadingMode_NoShading:
	default: 
		return "StandardShader";
	}
}

void ModelConverter::outputFiles()
{
	FILE *file = nullptr;
	if (_modelbuffer.GetSize() != 0) {
		file = fopen((_outputFileFolder + _outputModelName).c_str(), "wt");
		if (file) {
			fwrite(_modelbuffer.GetString(), _modelbuffer.GetSize(), 1, file);
		}
		fclose(file);
	}

	if (_matBuffer.GetSize() != 0) {
		file = fopen((_outputFileFolder + _outputMaterialName).c_str(), "wt");
		if (file) {
			fwrite(_matBuffer.GetString(), _matBuffer.GetSize(), 1, file);
		}
		fclose(file);
	}

	if (_animBuffer.GetSize() != 0) {
		file = fopen((_outputFileFolder + _outputAnimationName).c_str(), "wt");
		if (file) {
			fwrite(_animBuffer.GetString(), _animBuffer.GetSize(), 1, file);
		}
		fclose(file);
	}
}
