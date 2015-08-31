#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Material.h"
#include "KernelCore/Texture.h"
#include "KernelCore/FrameBufferObject.h"
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
		if (texVal.HasMember(SOURCE_KEY.c_str())) {
			std::string source = texVal[SOURCE_KEY.c_str()].GetString();
			veTexture *texture = nullptr;
			if (source.find_last_of(".") != std::string::npos) {
				veImage *image = static_cast<veImage *>(veFile::instance()->readFile(_fileFolder + source));
				texture = new veTexture2D(image);
			}
			else if (source.find_last_of(":") != std::string::npos) {
				std::string fboName = source.substr(0, source.find_last_of(":"));
				std::string attachName = source.substr(source.find_last_of(":") + 1);
				veFrameBufferObject *fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(fboName);
				texture = fbo->attach(getFrameBufferObjectAttach(attachName.c_str()));
			}

			if (!texture) return;
			std::string wrap = texVal[WRAP_KEY.c_str()].GetString();
			if (wrap == REPEAT_KEY) texture->setWrapMode(veTexture2D::REPEAT);
			else if (wrap == MIRROR_KEY) texture->setWrapMode(veTexture2D::MIRROR);
			else if (wrap == CLAMP_KEY) texture->setWrapMode(veTexture2D::CLAMP);
			else if (wrap == DECAL_KEY) texture->setWrapMode(veTexture2D::DECAL);
			else texture->setWrapMode(veTexture2D::REPEAT);

			std::string filter = texVal[FILTER_KEY.c_str()].GetString();
			if (filter == NEREAST_KEY) texture->setFilterMode(veTexture2D::NEAREST);
			else if (filter == LINEAR_KEY) texture->setFilterMode(veTexture2D::LINEAR);
			else texture->setFilterMode(veTexture2D::NEAREST);

			pass->addTexture(texture);
		}
		else if (texVal.HasMember(TARGET_KEY.c_str())) {
			std::string target = texVal[SOURCE_KEY.c_str()].GetString();
			if (target.find_last_of(":") != std::string::npos) {
				std::string fboName = target.substr(0, target.find_last_of(":"));
				std::string attachName = target.substr(target.find_last_of(":") + 1);
				veFrameBufferObject *fbo = veFrameBufferObjectManager::instance()->getOrCreateFrameBufferObject(fboName);
				veTexture *texture = fbo->attach(getFrameBufferObjectAttach(attachName.c_str()));
				int width = veTexture::DEFAULT_WIDTH;
				int height= veTexture::DEFAULT_HEIGHT;
				texture->autoWidth() = true;
				texture->autoHeight() = true;
				GLint internalFormat = veTexture::DEFAULT_INTERNAL_FORMAT;
				if (texVal.HasMember(WIDTH_KEY.c_str())) {
					width = texVal[WIDTH_KEY.c_str()].GetInt();
					texture->autoWidth() = false;
				}

				if (texVal.HasMember(HEIGHT_KEY.c_str())) {
					height = texVal[HEIGHT_KEY.c_str()].GetInt();
					texture->autoHeight() = false;
				}

				if (texVal.HasMember(FORMAT_KEY.c_str()) && texVal.HasMember(TYPE_KEY.c_str())) {
					const char* format = texVal[FORMAT_KEY.c_str()].GetString();
					const char* type = texVal[TYPE_KEY.c_str()].GetString();
					if (strcmp(format, "RGB") == 0 && strcmp(type, FLOAT_KEY.c_str()) == 0) {
						internalFormat = GL_RGB32F;
					}
					else if (strcmp(format, "RGBA") == 0 && strcmp(type, FLOAT_KEY.c_str()) == 0) {
						internalFormat = GL_RGBA32F;
					}
					else if (strcmp(format, "RGB") == 0 && strcmp(type, BYTE_KEY.c_str()) == 0) {
						internalFormat = GL_RGB8;
					}
					else if (strcmp(format, "RGBA") == 0 && strcmp(type, BYTE_KEY.c_str()) == 0) {
						internalFormat = GL_RGBA8;
					}
				}
				texture->storage(width, height, internalFormat);
			}
		}
	}

	GLenum getFrameBufferObjectAttach(const char* str) {
		if (strncmp(COLOR_KEY.c_str(), str, COLOR_KEY.size()) == 0) {
			return GL_COLOR_ATTACHMENT0 + atoi(&str[COLOR_KEY.size()]);
		}
		else if (strcmp(DEPTH_KEY.c_str(), str) == 0) {
			return GL_DEPTH_ATTACHMENT;
		}
		else if (strcmp(STENCIAL_KEY.c_str(), str) == 0) {
			return GL_STENCIL_ATTACHMENT;
		}
	}
private:

	Document _doucument;
	veMaterialArray *_materials;
	std::string _fileFolder;
};

VE_READERWRITER_REG("vemtl", veFileReaderWriterVEMTL);