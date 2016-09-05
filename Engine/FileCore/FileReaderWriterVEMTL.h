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
        if (!fileData)
            return nullptr;
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
			_materials = _sceneManager->createMaterialArray(_name);
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
        if (passVal.HasMember(RENDERPASS_KEY.c_str())){
            std::string renderPass = passVal[RENDERPASS_KEY.c_str()].GetString();
            if (renderPass == DEFERRED_KEY){
                pass->setRenderPass(vePass::DEFERRED_PASS);
            }else if(renderPass == FORWARD_KEY){
                pass->setRenderPass(vePass::FORWARD_PASS);
            }
        }
		if (passVal.HasMember(DEPTHTEST_KEY.c_str()))
			pass->depthTest() = passVal[DEPTHTEST_KEY.c_str()].GetBool();
		if (passVal.HasMember(DEPTHWRITE_KEY.c_str()))
			pass->depthWrite() = passVal[DEPTHWRITE_KEY.c_str()].GetBool();
		if (passVal.HasMember(STENCIALTEST_KEY.c_str()))
			pass->stencilTest() = passVal[STENCIALTEST_KEY.c_str()].GetBool();
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

		if (passVal.HasMember(BLENDEQUATION_KEY.c_str())) {
			const Value &beVal = passVal[BLENDEQUATION_KEY.c_str()];
			pass->blendEquation() = getBlendEquationParam(beVal.GetString());
		}

		if (passVal.HasMember(STENCIALFUNC_KEY.c_str())) {
			const Value &sfVal = passVal[STENCIALFUNC_KEY.c_str()];
			if (sfVal.Size() == 6) {
				pass->stencilFunc().frontfunc = getStencilFuncParam(sfVal[0].GetString());
				pass->stencilFunc().frontref  = sfVal[1].GetInt();
				pass->stencilFunc().frontmask = sfVal[2].GetInt();
				pass->stencilFunc().backfunc  = getStencilFuncParam(sfVal[3].GetString());
				pass->stencilFunc().backref   = sfVal[4].GetInt();
				pass->stencilFunc().backmask  = sfVal[5].GetInt();
			}
		}

		if (passVal.HasMember(STENCIALOP_KEY.c_str())) {
			const Value &soVal = passVal[STENCIALOP_KEY.c_str()];
			if (soVal.Size() == 6) {
				pass->stencilOp().frontsfail  = getStencilOpParam(soVal[0].GetString());
				pass->stencilOp().frontdpfail = getStencilOpParam(soVal[1].GetString());
				pass->stencilOp().frontdppass = getStencilOpParam(soVal[2].GetString());
				pass->stencilOp().backsfail   = getStencilOpParam(soVal[3].GetString());
				pass->stencilOp().backdpfail  = getStencilOpParam(soVal[4].GetString());
				pass->stencilOp().backdppass  = getStencilOpParam(soVal[5].GetString());
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
                if (veFile::instance()->isFileExist(veFile::instance()->getFullFilePath(source))) {
					shader->setSource(source);
				}
				else {
					shader->setSource(_fileFolder + source);
				}
			}else if (member->name.GetString() == DEFINATION_KEY) {
				shader->setShaderHeader(member->value.GetString());
			}else {
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
				}else {
					veRealArray vals;
					for (unsigned int i = 0; i < values.Size(); ++i) {
						vals.push_back(values[i].GetDouble());
					}
					uniform->setValue(vals);
				}
				pass->addUniform(uniform);
			}
		}
		pass->setShader(shader);
	}

	void readTexture(const Value &texVal, vePass *pass){
		veTexture::TextureType type = veTexture::TEXTURE_2D;
		veTexture *texture = nullptr;
		std::string name;
		if (texVal.HasMember(NAME_KEY.c_str())) {
			name = texVal[NAME_KEY.c_str()].GetString();
		}
		if (name.empty())
			return;
		if (texVal.HasMember(TYPE_KEY.c_str())) {
			type = getTextureType(texVal[TYPE_KEY.c_str()].GetString());
		}

		do 
		{
			if (true) {
				texture = static_cast<veTextureManager *>(_sceneManager->getManager(veTextureManager::TYPE()))->findTexture(name);
				if (!texture) {
					texture = _sceneManager->createTexture(name, type);
				}
                if (!texture) return;

				if (texVal.HasMember(SOURCE_KEY.c_str())) {
					if (type == veTexture::TEXTURE_CUBE) {
						const Value &sources = texVal[SOURCE_KEY.c_str()];
						if (sources.Size() != 6) return;
						//texture = _sceneManager->createTexture(name, texType);
						veTexture *subTexture = nullptr;
						for (unsigned int i = 0; i < sources.Size(); ++i) {
							std::string source = sources[i].GetString();
							std::string subName = name + std::string("-") + source;
							if (veFile::instance()->isSupportFile(source)) {
								if (veFile::instance()->isFileExist(veFile::instance()->getFullFilePath(source))) {
									subTexture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, source, subName));
								}
								else {
									subTexture = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, _fileFolder + source, subName));
								}							
							}
							else {
								subTexture = static_cast<veTextureManager *>(_sceneManager->getManager(veTextureManager::TYPE()))->findTexture(source);
								if (!subTexture)
									subTexture = _sceneManager->createTexture(source);
							}
							static_cast<veTextureCube *>(texture)->setTexture((veTextureCube::CubeMapTexType)i, subTexture);
						}
						veTexture::SwizzleMode r, g, b, a;
						subTexture->getSwizzleMode(r, g, b, a);
						static_cast<veTextureCube *>(texture)->setSwizzleMode(r, g, b, a);
					}
					else {
						std::string source = texVal[SOURCE_KEY.c_str()].GetString();
						//veTexture *texture = _sceneManager->createTexture(source, texType);					
						if (veFile::instance()->isSupportFile(source)) {
							veTexture *tempTex = nullptr;
							if (veFile::instance()->isFileExist(veFile::instance()->getFullFilePath(source))) {
								tempTex = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, source, _name + std::string("-temp")));
							}
							else {
								tempTex = static_cast<veTexture *>(veFile::instance()->readFile(_sceneManager, _fileFolder + source, _name + std::string("-temp")));
							}

							if (tempTex) {
								if (tempTex->getMipmapLevels().empty()) {
									texture->storage(tempTex->getWidth(), tempTex->getHeight(), tempTex->getDepth(), tempTex->getInternalFormat(), tempTex->getPixelFormat(), tempTex->getDataType(), tempTex->getData(), tempTex->getMipMapLevelCount());
								}
								else {
									texture->storage(tempTex->getMipmapLevels(), tempTex->getInternalFormat(), tempTex->getPixelFormat(), tempTex->getDataType());
								}
                                veTexture::SwizzleMode r, g, b, a;
                                tempTex->getSwizzleMode(r, g, b, a);
                                texture->setSwizzleMode(r, g, b, a);
							}
							//static_cast<veTextureManager *>(_sceneManager->getManager(veTextureManager::TYPE()))->removeTexture(tempTex);
						}
					}
				}

			}

		} while (false);

		if (texVal.HasMember(WRAP_KEY.c_str())) {
			std::string wrap = texVal[WRAP_KEY.c_str()].GetString();
			if (wrap == REPEAT_KEY) texture->setWrapMode(veTexture2D::REPEAT);
			else if (wrap == MIRROR_KEY) texture->setWrapMode(veTexture2D::MIRROR);
			else if (wrap == CLAMP_KEY) texture->setWrapMode(veTexture2D::CLAMP);
			else if (wrap == DECAL_KEY) texture->setWrapMode(veTexture2D::DECAL);
			else texture->setWrapMode(veTexture2D::CLAMP);
		}

		if (texVal.HasMember(FILTER_KEY.c_str())) {
			std::string filter = texVal[FILTER_KEY.c_str()].GetString();
			if (filter == NEREAST_KEY) texture->setFilterMode(veTexture2D::NEAREST);
			else if (filter == LINEAR_KEY) texture->setFilterMode(veTexture2D::LINEAR);
			else if (filter == NEAREST_MIP_MAP_NERAEST_KEY) texture->setFilterMode(veTexture2D::NEAREST_MIP_MAP_NEAREST);
			else if (filter == NEAREST_MIP_MAP_LINEAR_KEY) texture->setFilterMode(veTexture2D::NEAREST_MIP_MAP_LINEAR);
			else if (filter == LINEAR_MIP_MAP_NEAREST_KEY) texture->setFilterMode(veTexture2D::LINEAR_MIP_MAP_NEAREST);
			else if (filter == LINEAR_MIP_MAP_LINEAR_KEY) texture->setFilterMode(veTexture2D::LINEAR_MIP_MAP_LINEAR);
			else texture->setFilterMode(veTexture2D::NEAREST_MIP_MAP_LINEAR);
		}

		vePass::TextureType texType = vePass::DIFFUSE_TEXTURE;
		if (texVal.HasMember(TEXTYPE_KEY.c_str())) {
			texType = getPassTextureType(texVal[TEXTYPE_KEY.c_str()].GetString());
		}

		pass->setTexture(texType, texture);
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
		if (strncmp(COLOR_ATTACHMENT_KEY.c_str(), str, COLOR_ATTACHMENT_KEY.size()) == 0) {
			return GL_COLOR_ATTACHMENT0 + atoi(&str[COLOR_ATTACHMENT_KEY.size()]);
		}
		else if (strcmp(DEPTH_ATTACHMENT_KEY.c_str(), str) == 0) {
			return GL_DEPTH_ATTACHMENT;
		}
		else if (strcmp(STENCIAL_ATTACHMENT_KEY.c_str(), str) == 0) {
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
		return GL_ZERO;
	}

	GLenum getBlendEquationParam(const char* str) {
		if (strncmp(FUNC_ADD_KEY.c_str(), str, FUNC_ADD_KEY.size()) == 0) {
			return GL_FUNC_ADD;
		}
		else if (strncmp(FUNC_SUBTRACT_KEY.c_str(), str, FUNC_SUBTRACT_KEY.size()) == 0) {
			return GL_FUNC_SUBTRACT;
		}
		else if (strncmp(FUNC_REVERSE_SUBTRACT_KEY.c_str(), str, FUNC_REVERSE_SUBTRACT_KEY.size()) == 0) {
			return GL_FUNC_REVERSE_SUBTRACT;
		}
		else if (strncmp(MIN_KEY.c_str(), str, MIN_KEY.size()) == 0) {
			return GL_MIN;
		}
		else if (strncmp(MAX_KEY.c_str(), str, MAX_KEY.size()) == 0) {
			return GL_MAX;
		}
		return GL_ZERO;
	}

	GLenum getStencilFuncParam(const char* str) {
		if (strncmp(NEVER_KEY.c_str(), str, NEVER_KEY.size()) == 0) {
			return GL_NEVER;
		}
		else if (strncmp(LESS_KEY.c_str(), str, LESS_KEY.size()) == 0) {
			return GL_LESS;
		}
		else if (strncmp(LEQUAL_KEY.c_str(), str, LEQUAL_KEY.size()) == 0) {
			return GL_LEQUAL;
		}
		else if (strncmp(GREATER_KEY.c_str(), str, GREATER_KEY.size()) == 0) {
			return GL_GREATER;
		}
		else if (strncmp(GEQUAL_KEY.c_str(), str, GEQUAL_KEY.size()) == 0) {
			return GL_GEQUAL;
		}
		else if (strncmp(EQUAL_KEY.c_str(), str, EQUAL_KEY.size()) == 0) {
			return GL_EQUAL;
		}
		else if (strncmp(NOTEQUAL_KEY.c_str(), str, NOTEQUAL_KEY.size()) == 0) {
			return GL_NOTEQUAL;
		}
		else if (strncmp(ALWAYS_KEY.c_str(), str, ALWAYS_KEY.size()) == 0) {
			return GL_ALWAYS;
		}

		return GL_ALWAYS;
	}

	GLenum getStencilOpParam(const char* str) {
		if (strncmp(KEEP_KEY.c_str(), str, KEEP_KEY.size()) == 0) {
			return GL_KEEP;
		}
		else if (strncmp(ZERO_KEY.c_str(), str, ZERO_KEY.size()) == 0) {
			return GL_ZERO;
		}
		else if (strncmp(REPLACE_KEY.c_str(), str, REPLACE_KEY.size()) == 0) {
			return GL_REPLACE;
		}
		else if (strncmp(INCR_KEY.c_str(), str, INCR_KEY.size()) == 0) {
			return GL_INCR;
		}
		else if (strncmp(INCR_WRAP_KEY.c_str(), str, INCR_WRAP_KEY.size()) == 0) {
			return GL_INCR_WRAP;
		}
		else if (strncmp(DECR_KEY.c_str(), str, DECR_KEY.size()) == 0) {
			return GL_DECR;
		}
		else if (strncmp(DECR_WRAP_KEY.c_str(), str, DECR_WRAP_KEY.size()) == 0) {
			return GL_DECR_WRAP;
		}
		else if (strncmp(INVERT_KEY.c_str(), str, INVERT_KEY.size()) == 0) {
			return GL_INVERT;
		}

		return GL_KEEP;
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

	vePass::TextureType getPassTextureType(const char *str) {
		if (strcmp(AMBIENT_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::AMBIENT_TEXTURE;
		}
		else if (strcmp(DIFFUSE_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::DIFFUSE_TEXTURE;
		}
		else if (strcmp(SPECULAR_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::SPECULAR_TEXTURE;
		}
		else if (strcmp(EMISSIVE_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::EMISSIVE_TEXTURE;
		}
		else if (strcmp(NORMAL_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::NORMAL_TEXTURE;
		}
		else if (strcmp(HEIGHT_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::HEIGHT_TEXTURE;
		}
		else if (strcmp(SHININESS_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::SHININESS_TEXTURE;
		}
		else if (strcmp(OPACITYT_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::OPACITYT_TEXTURE;
		}
		else if (strcmp(DISPLACEMENT_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::DISPLACEMENT_TEXTURE;
		}
		else if (strcmp(LIGHTMAP_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::LIGHTMAP_TEXTURE;
		}
		else if (strcmp(REFLECTION_TEXTURE_KEY.c_str(), str) == 0) {
			return vePass::REFLECTION_TEXTURE;
		}

		return vePass::DIFFUSE_TEXTURE;
	}

private:

	Document *_doucument;
	veMaterialArray *_materials;
	std::string _fileFolder;
	std::string _name;
	veSceneManager *_sceneManager;
	std::map<std::string, Document *> _doucumentMap;
};