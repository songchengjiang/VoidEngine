#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Mesh.h"
#include "KernelCore/SceneManager.h"
#include "BaseCore/Array.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterVEM : public veFileReaderWriter
{
public:
	veFileReaderWriterVEM()
		: _node(nullptr)
		, _doucument(nullptr)
	{};
	virtual ~veFileReaderWriterVEM(){
	};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);	
		_doucument = new Document;
		auto fileData = veFile::instance()->readFileToBuffer(fullPath);
        if (!fileData)
            return nullptr;
		_doucument->Parse(fileData->buffer);
		if (_doucument->HasParseError()) return nullptr;
		_sceneManager = sm;
		_name = name;
		_fileFolder = fullPath.substr(0, fullPath.find_last_of("/\\") + 1);
		parseDoc();
		_meshList.clear();
		_boneList.clear();
		VE_SAFE_DELETE(_doucument);
		return _node;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void parseDoc(){
		_node = _sceneManager->createNode(_name);
		loadMaterials();
		readMeshs();
		readNodes();
	}

	void loadMaterials(){
		std::string matFile = (*_doucument)[MATERIALS_KEY.c_str()].GetString();
		if (veFile::instance()->isFileExist(veFile::instance()->getFullFilePath(matFile))) {
			_materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, matFile, _name + std::string("-Materials")));
		}
		else {
			_materials = static_cast<veMaterialArray *>(veFile::instance()->readFile(_sceneManager, _fileFolder + matFile, _name + std::string("-Materials")));
		}
	}

	void readMeshs(){
		if ((*_doucument).HasMember(MESHES_KEY.c_str())){
			const Value &meshs = (*_doucument)[MESHES_KEY.c_str()];
			for (unsigned int i = 0; i < meshs.Size(); ++i){
				readMesh(meshs[i]);
			}
		}
	}

	void readMesh(const Value &meshVal){
		auto mesh = _sceneManager->createMesh(meshVal[NAME_KEY.c_str()].GetString());
		veMaterial *mat = _materials->getMaterial(meshVal[MATERIAL_KEY.c_str()].GetString());
		mesh->setMaterial(mat);

		if (meshVal.HasMember(ATTRIBUTES_KEY.c_str())){
			const Value &attris = meshVal[ATTRIBUTES_KEY.c_str()];
			for (unsigned int i = 0; i < attris.Size(); ++i){
				readMeshAttri(attris[i], mesh);
			}
		}

		if (meshVal.HasMember(VERTICES_KEY.c_str())){
			const Value &vertices = meshVal[VERTICES_KEY.c_str()];
			veRealArray *ary = new veRealArray(vertices.Size());
			for (unsigned int i = 0; i < vertices.Size(); ++i){
				(*ary)[i] = (veReal)vertices[i].GetDouble();
			}
			mesh->setVertexArray(ary);
		}

		if (meshVal.HasMember(PARTS_KEY.c_str())){
			const Value &parts = meshVal[PARTS_KEY.c_str()];
			for (unsigned int i = 0; i < parts.Size(); ++i){
				readMeshPart(parts[i], mesh);
			}
		}

		if (meshVal.HasMember(BONES_KEY.c_str())) {
			const Value &bones = meshVal[BONES_KEY.c_str()];
			for (unsigned int i = 0; i < bones.Size(); ++i) {
				readMeshBone(bones[i], mesh);
			}
		}

		//mesh->caculateBoundingBox();
		_meshList[mesh->getName()] = mesh;
	}

	void readMeshAttri(const Value &attriVal, veMesh *mesh){
		veMesh::VertexAtrribute vertexAttri;
		std::string attri = attriVal[ATTRIBUTE_KEY.c_str()].GetString();
		if (attri == VERTEX_ATTRIB_POSITION) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_POSITION;
		else if (attri == VERTEX_ATTRIB_NORMAL) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_NORMAL;
		else if (attri == VERTEX_ATTRIB_TANGENT) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_TANGENT;
		else if (attri == VERTEX_ATTRIB_BITANGENT) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_BITANGENT;
		else if (attri == (VERTEX_ATTRIB_TEX_COORD + std::string("0"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD0;
		else if (attri == (VERTEX_ATTRIB_TEX_COORD + std::string("1"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD1;
		else if (attri == (VERTEX_ATTRIB_TEX_COORD + std::string("2"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD2;
		else if (attri == (VERTEX_ATTRIB_TEX_COORD + std::string("3"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_TEX_COORD3;
		else if (attri == (VERTEX_ATTRIB_COLOR + std::string("0"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR0;
		else if (attri == (VERTEX_ATTRIB_COLOR + std::string("1"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR1;
		else if (attri == (VERTEX_ATTRIB_COLOR + std::string("2"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR2;
		else if (attri == (VERTEX_ATTRIB_COLOR + std::string("3"))) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_COLOR3;
		else if (attri == VERTEX_ATTRIB_BONE_INDICES) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_BONE_INDICES;
		else if (attri == VERTEX_ATTRIB_BONE_WEIGHTS) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_BONE_WEIGHTS;
		else if (attri == VERTEX_ATTRIB_USER + std::string("0")) vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_USER0;
		else vertexAttri.attributeType = veMesh::VertexAtrribute::VERTEX_ATTRIB_USER1;

		vertexAttri.valueNum = (unsigned short)attriVal[SIZE_KEY.c_str()].GetUint();

		std::string valueType = attriVal[TYPE_KEY.c_str()].GetString();
		if (valueType == FLOAT_KEY) vertexAttri.valueType = veMesh::VertexAtrribute::FLOAT;
		else if (valueType == UINT_KEY) vertexAttri.valueType = veMesh::VertexAtrribute::UINT;
		else if (valueType == USHORT_KEY) vertexAttri.valueType = veMesh::VertexAtrribute::USHORT;
		else vertexAttri.valueType = veMesh::VertexAtrribute::FLOAT;

		mesh->addVertexAtrribute(vertexAttri);
	}

	void readMeshPart(const Value &partVal, veMesh *mesh){
		veMesh::Primitive primitive;
		std::string primType = partVal[TYPE_KEY.c_str()].GetString();
		if (primType == POINTS_KEY) primitive.primitiveType = veMesh::Primitive::POINTS;
		else if (primType == LINES_KEY) primitive.primitiveType = veMesh::Primitive::LINES;
		else if (primType == TRIANGLES_KEY) primitive.primitiveType = veMesh::Primitive::TRIANGLES;
		else primitive.primitiveType = veMesh::Primitive::POINTS;

		if (partVal.HasMember(INDICES_KEY.c_str())){
			const Value &indices = partVal[INDICES_KEY.c_str()];
			primitive.indices = new veUint16Array(indices.Size());
			for (unsigned int i = 0; i < indices.Size(); ++i){
				(*primitive.indices)[i] = indices[i].GetUint();
			}
		}

		mesh->addPrimitive(primitive);
	}

	void readMeshBone(const Value &boneVal, veMesh *mesh) {
		std::string name = boneVal[NAME_KEY.c_str()].GetString();
		veBone *bone = nullptr;
		auto iter = _boneList.find(name);
		if (iter == _boneList.end())
		{
			bone = new veBone;
			bone->setName(name);
			veMat4 offsetMatrix;
			const Value &mat = boneVal[TRANSFORM_KEY.c_str()];
			for (unsigned int i = 0; i < mat.Size(); ++i) {
				offsetMatrix[i / 4][i % 4] = (veReal)mat[i].GetDouble();
			}
			bone->setOffsetMat(offsetMatrix);

			//const Value &weights = boneVal[WEIGHTS_KEY.c_str()];
			//for (unsigned int i = 0; i < weights.Size();) {
			//	bone->setWeight((unsigned int)weights[i].GetDouble(), (float)weights[i + 1].GetDouble());
			//	i += 2;
			//}
			_boneList[bone->getName()] = bone;
		}
		else
			bone = iter->second;
		mesh->addBone(bone);
	}

	void readNodes(){
		if ((*_doucument).HasMember(NODES_KEY.c_str())){
			const Value &nodes = (*_doucument)[NODES_KEY.c_str()];
			for (unsigned int i = 0; i < nodes.Size(); ++i){
				readNode(nodes[i], nullptr);
			}
		}
	}

	void readNode(const Value &nodeVal, veNode *parent){
		veNode *node = _sceneManager->createNode(nodeVal[NAME_KEY.c_str()].GetString());
		if (parent != nullptr){
			parent->addChild(node);
		}
		else{
            _node->addChild(node);
		}
		veMat4 matrix;
		const Value &mat = nodeVal[TRANSFORM_KEY.c_str()];
		for (unsigned int i = 0; i < mat.Size(); ++i){
			matrix[i / 4][i % 4] = (veReal)mat[i].GetDouble();
		}
		node->setMatrix(matrix);

		if (nodeVal.HasMember(MESHES_KEY.c_str())){
			const Value &meshs = nodeVal[MESHES_KEY.c_str()];
			for (unsigned int i = 0; i < meshs.Size(); ++i){
				std::string meshName = meshs[i].GetString();
                node->addRenderableObject(_meshList[meshName]);
			}
		}

		if (!_boneList.empty()) {
			auto iter = _boneList.find(node->getName());
			if (iter != _boneList.end()) {
				iter->second->setBoneNode(node);
			}
		}

		if (nodeVal.HasMember(NODES_KEY.c_str())){
			const Value &children = nodeVal[NODES_KEY.c_str()];
			for (unsigned int i = 0; i < children.Size(); ++i){
				readNode(children[i], node);
			}
		}
	}

private:

	veNode *_node;
	Document *_doucument;
	std::unordered_map<std::string, veMesh *> _meshList;
	std::unordered_map<std::string, veBone * > _boneList;
	VE_Ptr<veMaterialArray> _materials;
	std::string _fileFolder;
	std::string _name;
	veSceneManager *_sceneManager;
};
