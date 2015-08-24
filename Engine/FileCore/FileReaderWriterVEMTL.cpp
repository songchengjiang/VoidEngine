#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Material.h"
#include "KernelCore/Texture.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterVEMTL : public veFileReaderWriter
{
public:
	veFileReaderWriterVEMTL()
		: _materials(nullptr)
	{};
	~veFileReaderWriterVEMTL(){};

	virtual void* readFile(const std::string &filePath){
		_fileFolder = filePath.substr(0, filePath.find_last_of("/\\") + 1);
		std::string buffer = veFile::instance()->readFileToBuffer(filePath);
		_doucument.Parse(buffer.c_str());
		parseDoc();
		return _materials;
	}

	virtual bool writeFile(void *data, const std::string &filePath){
		return true;
	}

private:

	void parseDoc(){
		if (_doucument.HasMember(MATERIALS_KEY.c_str())){
			_materials = new veMaterialArray;
			const Value &mats = _doucument[MATERIALS_KEY.c_str()];
			for (unsigned int i = 0; i < mats.Size(); ++i){
				readMaterial(mats[i]);
			}
		}
	}

	void readMaterial(const Value &matVal){
		veMaterial *material = new veMaterial;
		material->setName(matVal[NAME_KEY.c_str()].GetString());
		if (matVal.HasMember(TECHNIQUES_KEY.c_str())){
			const Value &techs = matVal[TECHNIQUES_KEY.c_str()];
			for (unsigned int i = 0; i < techs.Size(); ++i){
				readTechnique(techs[i], material);
			}
		}
		_materials->addMaterial(material);
	}

	void readTechnique(const Value &techVal, veMaterial *material){
		veTechnique *tecnique = new veTechnique;
		tecnique->setName(techVal[NAME_KEY.c_str()].GetString());
		if (techVal.HasMember(PASSES_KEY.c_str())){
			const Value &passes = techVal[PASSES_KEY.c_str()];
			for (unsigned int i = 0; i < passes.Size(); ++i){
				readPass(passes[i], tecnique);
			}
		}
		material->addTechnique(tecnique);
	}

	void readPass(const Value &passVal, veTechnique *tecnique){
		vePass *pass = new vePass;
		pass->depthTest() = passVal[DEPTHTEST_KEY.c_str()].GetBool();
		pass->depthWrite() = passVal[DEPTHWRITE_KEY.c_str()].GetBool();
		pass->cullFace() = passVal[CULLFACE_KEY.c_str()].GetBool();

		if (passVal.HasMember(SHADERS_KEY.c_str())){
			const Value &shaders = passVal[SHADERS_KEY.c_str()];
			for (unsigned int i = 0; i < shaders.Size(); ++i){
				readShader(shaders[i], pass);
			}
		}

		if (passVal.HasMember(TEXTUREUNITS_KEY.c_str())){
			const Value &texs = passVal[TEXTUREUNITS_KEY.c_str()];
			for (unsigned int i = 0; i < texs.Size(); ++i){
				readTexture(texs[i], pass);
			}
		}
		tecnique->addPass(pass);
	}

	void readShader(const Value &shaderVal, vePass *pass){
		veShader *shader = new veShader;
		auto member = shaderVal.MemberBegin();
		for (; member != shaderVal.MemberEnd(); ++member){
			if (member->name.GetString() == TYPE_KEY){
				veShader::Type shaderType;
				std::string type = member->value.GetString();
				if (type == VERTEXSHADER_KEY) shaderType = veShader::VERTEX_SHADER;
				else if (type == FRAGMENTSHADER_KEY) shaderType = veShader::FRAGMENT_SHADER;
				else shaderType = veShader::VERTEX_SHADER;
				shader->setType(shaderType);
			}
			else if (member->name.GetString() == SOURCE_KEY){
				std::string source = member->value.GetString();
				shader->setSource(_fileFolder + source);
			}else{
				veUniform *uniform = new veUniform(member->name.GetString());
				const Value &values = member->value;
				if (values.IsInt()){
					uniform->setValue(values.GetInt());
				}else if (values.IsDouble()){
					uniform->setValue((veReal)values.GetDouble());
				}else if (values.IsString()){
					uniform->setValue(std::string(values.GetString()));
				}else if (values.Size() == 2){
					veVec2 vec2(values[0].GetDouble(), values[1].GetDouble());
					uniform->setValue(vec2);
				}else if (values.Size() == 3){
					veVec3 vec3(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble());
					uniform->setValue(vec3);
				}else if (values.Size() == 4){
					veVec4 vec4(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble(), values[3].GetDouble());
					uniform->setValue(vec4);
				}else if (values.Size() == 9){
					veMat3 mat(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble()
						, values[3].GetDouble(), values[4].GetDouble(), values[5].GetDouble()
						, values[6].GetDouble(), values[7].GetDouble(), values[8].GetDouble());
					uniform->setValue(mat);
				}else if (values.Size() == 16){
					veMat4 mat(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble(), values[3].GetDouble()
						, values[4].GetDouble(), values[5].GetDouble(), values[6].GetDouble(), values[7].GetDouble()
						, values[8].GetDouble(), values[9].GetDouble(), values[10].GetDouble(), values[11].GetDouble()
						, values[12].GetDouble(), values[13].GetDouble(), values[14].GetDouble(), values[15].GetDouble());
					uniform->setValue(mat);
				}
				shader->addUniform(uniform);
			}
		}
		pass->setShader(shader);
	}

	void readTexture(const Value &texVal, vePass *pass){
		std::string source = texVal[SOURCE_KEY.c_str()].GetString();
		veImage *image = new veImage(_fileFolder + source);
		veTexture2D *tex2D = new veTexture2D(image);
		std::string wrap = texVal[WRAP_KEY.c_str()].GetString();
		if (wrap == REPEAT_KEY) tex2D->setWrapMode(veTexture2D::REPEAT);
		else if (wrap == MIRROR_KEY) tex2D->setWrapMode(veTexture2D::MIRROR);
		else if (wrap == CLAMP_KEY) tex2D->setWrapMode(veTexture2D::CLAMP);
		else if (wrap == DECAL_KEY) tex2D->setWrapMode(veTexture2D::DECAL);
		else tex2D->setWrapMode(veTexture2D::REPEAT);

		std::string filter = texVal[FILTER_KEY.c_str()].GetString();
		if (filter == NEREAST_KEY) tex2D->setFilterMode(veTexture2D::NEAREST);
		else if (filter == LINEAR_KEY) tex2D->setFilterMode(veTexture2D::LINEAR);
		else tex2D->setFilterMode(veTexture2D::NEAREST);

		pass->addTexture(tex2D);
	}

private:

	Document _doucument;
	veMaterialArray *_materials;
	std::string _fileFolder;
};

VE_READERWRITER_REG("vemtl", veFileReaderWriterVEMTL);