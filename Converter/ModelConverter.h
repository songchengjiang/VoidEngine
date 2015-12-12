#ifndef _MODEL_CONVERTER_
#define _MODEL_CONVERTER_
#include <string>
#include <vector>
#include <rapidjson/include/writer.h>
#include <rapidjson/include/stringbuffer.h>
#include <assimp/include/scene.h>
#include "PrettyWriterExt.h"
#include "Converter.h"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;
typedef aiColor4D aiVector4D;
class ModelConverter : public Converter
{
public:
	ModelConverter();
	virtual ~ModelConverter();

	virtual void convert(const std::string &filePath, const std::string &outFile) override;

private:

	void writeModel(const aiScene *scene);
	void writeMesh(const aiMesh *mesh, const std::string &name);
	void writeMeshAttributes(const aiMesh *mesh);
	void writeMeshVertices(const aiMesh *mesh);
	void writeMeshPairs(const aiMesh *mesh);
	void writeMeshBones(const aiMesh *mesh);
	void writeNode(const aiNode *node);

	void writeMaterials(const aiScene *scene);
	void writeMaterial(const aiMaterial *mat, const std::string &name);
	void writeTechnique(const aiMaterial *mat);
	void writePass(const aiMaterial *mat);
	void writeRenderState(const aiMaterial *mat);
	void writeShaders(const aiMaterial *mat);
	void writeShader(const aiMaterial *mat, const std::string &shaderName);

	void writeTextures(const aiMaterial *mat);
	void writeTexture(const aiMaterial *mat, aiTextureType texType);

	void writeAnimations(const aiScene *scene);
	void writeAnimation(const aiAnimation *animation);
	void writeNodeAnim(const aiNodeAnim *nodeAnim);

	void generateMeshNames(const aiScene *scene);
	void generateMaterialNames(const aiScene *scene);
	std::string getShaderName(int shaderMode);

	void outputFiles();

	void setOutputFileInfo(const std::string &filePath, const std::string &outFile);

	static void writeVec4(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiColor4D *col);
	static void writeVec3(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiVector3D *vec3);
	static void writeVec2(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiVector2D *vec2);
	static void writeMat4(PrettyWriterExt<rapidjson::StringBuffer> &writer, const aiMatrix4x4 *mat4);
	static bool hasTexture(const aiMaterial *mat, aiTextureType texType);

	void collectBoneIndiecsAndBoneWeights(const aiMesh *mesh, std::vector< std::pair<aiVector4D, aiVector4D> > &results);

private:

	std::string getTextureTypeName(aiTextureType texType);

private:

	rapidjson::StringBuffer _modelbuffer;
	PrettyWriterExt<rapidjson::StringBuffer> _modelWriter;

	rapidjson::StringBuffer _matBuffer;
	PrettyWriterExt<rapidjson::StringBuffer> _matWriter;

	rapidjson::StringBuffer _animBuffer;
	PrettyWriterExt<rapidjson::StringBuffer> _animWriter;

	std::string _outputFileFolder;
	std::string _outputModelName;
	std::string _outputMaterialName;
	std::string _outputAnimationName;
	bool        _hasBonesInfo;
};

#endif