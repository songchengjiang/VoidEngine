#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Material.h"
#include "KernelCore/Texture.h"
#include "KernelCore/TextureManager.h"
#include "KernelCore/FrameBufferObject.h"
#include "KernelCore/SceneManager.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterVEMTL : public veFileReaderWriter
{
public:
	veFileReaderWriterVEMTL()
		: _materials(nullptr)
		, _doucument(nullptr)
	{};
	~veFileReaderWriterVEMTL(){
		for (auto & iter : _doucumentMap) {
			VE_SAFE_DELETE(iter.second);
		}
	};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		_doucument = new Document;
		auto fileData = veFile::instance()->readFileToBuffer(fullPath);
		_doucument->Parse(fileData->buffer);
		if (_doucument->HasParseError()) return nullptr;
		_sceneManager = sm;
		_name = name;
		_fileFolder = fullPath.substr(0, fullPath.find_last_of("/\\") + 1);
		parseDoc();
		VE_SAFE_DELETE(_doucument);
		return _materials;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void parseDoc(){
		if (_doucument->HasMember(MATERIALS_KEY.c_str())){
			_materials = new veMaterialArray;
			_materials->setName(_name);
			const Value &mats = (*_doucument)[MATERIALS_KEY.c_str()];
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
		readRenderState(passVal, pass);
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

	void readRenderState(const Value &passVal, vePass *pass) {
		if (passVal.HasMember(DEPTHTEST_KEY.c_str()))
			pass->depthTest() = passVal[DEPTHTEST_KEY.c_str()].GetBool();
		if (passVal.HasMember(DEPTHWRITE_KEY.c_str()))
			pass->depthWrite() = passVal[DEPTHWRITE_KEY.c_str()].GetBool();
		if (passVal.HasMember(CULLFACE_KEY.c_str()))
			pass->cullFace() = passVal[CULLFACE_KEY.c_str()].GetBool();
		if (passVal.HasMember(CULLFACEMODE_KEY.c_str())) {
			std::string cfm = passVal[CULLFACEMODE_KEY.c_str()].GetString();
			if (cfm == FRONT_KEY) {
				pass->cullFaceMode() = GL_FRONT;
			}
			else if (cfm == BACK_KEY){
				pass->cullFaceMode() = GL_BACK;
			}
			else if (cfm == FRONTANDBACK_KEY) {
				pass->cullFaceMode() = GL_FRONT_AND_BACK;
			}
		}
		if (passVal.HasMember(DRAWMASK_KEY.c_str()))
			pass->drawMask() = passVal[DRAWMASK_KEY.c_str()].GetUint();
		if (passVal.HasMember(BLENDFUNC_KEY.c_str())) {
			const Value &bfVal = passVal[BLENDFUNC_KEY.c_str()];
			if (bfVal.Size() == 2) {
				pass->blendFunc().src = getBlendFuncParam(bfVal[0].GetString());
				pass->blendFunc().dst = getBlendFuncParam(bfVal[1].GetString());
			}
		}
	}

	void readShader(const Value &shaderVal, vePass *pass){
		veShader *shader = nullptr;

		if (shaderVal.HasMember(SOURCE_KEY.c_str())) {
			std::string name = shaderVal[SOURCE_KEY.c_str()].GetString();
			shader = new veShader;
		}

		if (!shader) return;
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
				pass->addUniform(uniform);
			}
		}
		pass->setShader(shader);
	}

	void readTexture(const Value &texVal, vePass *pass){

		veTexture::TextureType texType = veTexture::TEXTURE_2D;
		veTexture *texture = nullptr;
		std::string name;
		if (texVal.HasMember(NAME_KEY.c_str())) {
			name = texVal[NAME_KEY.c_str()].GetString();
		}
		if (texVal.HasMember(TYPE_KEY.c_str())) {
			texType = getTextureType(texVal[TYPE_KEY.c_str()].GetString());
		}

		do 
		{
			if (!name.empty() && !texVal.HasMember(SOURCE_KEY.c_str())) {
				texture = static_cast<veTextureManager *>(_sceneManager->getManager(veTextureManager::TYPE()))->findTexture(name);
				if (texture) break;
				texture = _sceneManager->createTexture(name, texType);
			}
			if (!texture) {
				if (texVal.HasMember(SOURCE_KEY.c_str())) {
					if (texType == veTexture::TEXTURE_CUBE) {
						const Value &sources = texVal[SOURCE_KEY.c_str()];
						if (sources.Size() != 6) return;
						if (name.empty()) name = sources[0].GetString();
						texture = _sceneManager->createTexture(name, texType);
						for (unsigned int i = 0; i < sources.Size(); ++i) {
							std::string source = sources[i].GetString();
							std::string subName = name + std::string("-") + source;
							veTexture *subTexture = nullptr;
							if (veFile::instance()->isSupportFile(source)) {
								subTexture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, _fileFolder + source, subName));
							}
							else {
								subTexture = static_cast<veTextureManager *>(_sceneManager->getManager(veTextureManager::TYPE()))->findTexture(source);
								if (!subTexture)
									subTexture = _sceneManager->createTexture(source);
							}
							static_cast<veTextureCube *>(texture)->setTexture((veTextureCube::CubeMapTexType)i, subTexture);
						}
					}
					else {
						std::string source = texVal[SOURCE_KEY.c_str()].GetString();
						if (name.empty()) name = source;
						//veTexture *texture = _sceneManager->createTexture(source, texType);					
						if (veFile::instance()->isSupportFile(source)) {
							texture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, _fileFolder + source, name));
						}
					}
				}

				if (!texture) return;
			}

			if (texVal.HasMember(WRAP_KEY.c_str())) {
				std::string wrap = texVal[WRAP_KEY.c_str()].GetString();
				if (wrap == REPEAT_KEY) texture->setWrapMode(veTexture2D::REPEAT);
				else if (wrap == MIRROR_KEY) texture->setWrapMode(veTexture2D::MIRROR);
				else if (wrap == CLAMP_KEY) texture->setWrapMode(veTexture2D::CLAMP);
				else if (wrap == DECAL_KEY) texture->setWrapMode(veTexture2D::DECAL);
				else texture->setWrapMode(veTexture2D::REPEAT);
			}
	
			if (texVal.HasMember(FILTER_KEY.c_str())) {
				std::string filter = texVal[FILTER_KEY.c_str()].GetString();
				if (filter == NEREAST_KEY) texture->setFilterMode(veTexture2D::NEAREST);
				else if (filter == LINEAR_KEY) texture->setFilterMode(veTexture2D::LINEAR);
				else texture->setFilterMode(veTexture2D::NEAREST);
			}


		} while (false);

		pass->addTexture(texture);
		//else if (texVal.HasMember(TARGET_KEY.c_str())) {
		//	std::string target = texVal[TARGET_KEY.c_str()].GetString();
		//	if (target.find_last_of(":") != std::string::npos) {
		//		std::string fboName = target.substr(0, target.find_last_of(":"));
		//		std::string attachName = target.substr(target.find_last_of(":") + 1);
		//		veTexture *texture = veTextureManager::instance()->getOrCreateTexture(target, veTexture::TEXTURE_2D);
		//		int width = veTexture::DEFAULT_WIDTH;
		//		int height = veTexture::DEFAULT_HEIGHT;
		//		texture->autoWidth() = true;
		//		texture->autoHeight() = true;
		//		GLint internalFormat = veTexture::DEFAULT_INTERNAL_FORMAT;
		//		if (texVal.HasMember(WIDTH_KEY.c_str())) {
		//			width = texVal[WIDTH_KEY.c_str()].GetInt();
		//			texture->autoWidth() = false;
		//		}

		//		if (texVal.HasMember(HEIGHT_KEY.c_str())) {
		//			height = texVal[HEIGHT_KEY.c_str()].GetInt();
		//			texture->autoHeight() = false;
		//		}

		//		if (texVal.HasMember(FORMAT_KEY.c_str()) && texVal.HasMember(TYPE_KEY.c_str())) {
		//			const char* format = texVal[FORMAT_KEY.c_str()].GetString();
		//			const char* type = texVal[TYPE_KEY.c_str()].GetString();
		//			if (strcmp(format, "RGB") == 0 && strcmp(type, FLOAT_KEY.c_str()) == 0) {
		//				internalFormat = GL_RGB32F;
		//			}
		//			else if (strcmp(format, "RGBA") == 0 && strcmp(type, FLOAT_KEY.c_str()) == 0) {
		//				internalFormat = GL_RGBA32F;
		//			}
		//			else if (strcmp(format, "RGB") == 0 && strcmp(type, BYTE_KEY.c_str()) == 0) {
		//				internalFormat = GL_RGB8;
		//			}
		//			else if (strcmp(format, "RGBA") == 0 && strcmp(type, BYTE_KEY.c_str()) == 0) {
		//				internalFormat = GL_RGBA8;
		//			}
		//		}
		//		texture->storage(internalFormat, width, height);
		//	}
		//}
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
		return 0;
	}

	GLenum getBlendFuncParam(const char* str) {
		if (strncmp(ONE_KEY.c_str(), str, ONE_KEY.size()) == 0) {
			return GL_ONE;
		}
		else if (strncmp(ZERO_KEY.c_str(), str, ZERO_KEY.size()) == 0) {
			return GL_ZERO;
		}
		else if (strncmp(SRC_COLOR_KEY.c_str(), str, SRC_COLOR_KEY.size()) == 0) {
			return GL_SRC_COLOR;
		}
		else if (strncmp(ONE_MINUS_SRC_COLOR_KEY.c_str(), str, ONE_MINUS_SRC_COLOR_KEY.size()) == 0) {
			return GL_ONE_MINUS_SRC_COLOR;
		}
		else if (strncmp(SRC_ALPHA_KEY.c_str(), str, SRC_ALPHA_KEY.size()) == 0) {
			return GL_SRC_ALPHA;
		}
		else if (strncmp(ONE_MINUS_SRC_ALPHA_KEY.c_str(), str, ONE_MINUS_SRC_ALPHA_KEY.size()) == 0) {
			return GL_ONE_MINUS_SRC_ALPHA;
		}
		else if (strncmp(DST_ALPHA_KEY.c_str(), str, DST_ALPHA_KEY.size()) == 0) {
			return GL_DST_ALPHA;
		}
		else if (strncmp(ONE_MINUS_DST_ALPHA_KEY.c_str(), str, ONE_MINUS_DST_ALPHA_KEY.size()) == 0) {
			return GL_ONE_MINUS_DST_ALPHA;
		}
		else if (strncmp(DST_COLOR_KEY.c_str(), str, DST_COLOR_KEY.size()) == 0) {
			return GL_DST_COLOR;
		}
		else if (strncmp(ONE_MINUS_DST_COLOR_KEY.c_str(), str, ONE_MINUS_DST_COLOR_KEY.size()) == 0) {
			return GL_ONE_MINUS_DST_COLOR;
		}
		return 0;
	}

	veTexture::TextureType getTextureType(const char *str) {
		if (strcmp(TEX_2D_KEY.c_str(), str) == 0) {
			return veTexture::TEXTURE_2D;
		}
		else if (strcmp(TEX_3D_KEY.c_str(), str) == 0) {
			return veTexture::TEXTURE_3D;
		}
		//else if (strcmp(TEX_RECT_KEY.c_str(), str) == 0) {
		//	return veTexture::TEXTURE_RECT;
		//}
		else if (strcmp(TEX_CUBE_KEY.c_str(), str) == 0) {
			return veTexture::TEXTURE_CUBE;
		}
		return veTexture::TEXTURE_2D;
	}

private:

	Document *_doucument;
	veMaterialArray *_materials;
	std::string _fileFolder;
	std::string _name;
	veSceneManager *_sceneManager;
	std::map<std::string, Document *> _doucumentMap;
};

VE_READERWRITER_REG("vemtl", veFileReaderWriterVEMTL);