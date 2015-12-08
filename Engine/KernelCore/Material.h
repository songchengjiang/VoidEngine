#ifndef _VE_MATERIAL_
#define _VE_MATERIAL_
#include "Prerequisites.h"
#include "Shader.h"
#include "Texture.h"
#include "VE_Ptr.h"
#include "RenderCommand.h"
#include "TransformFeedback.h"
#include <unordered_map>

struct veRenderCommand;
class veRenderer;
class veFrameBufferObject;
class veLight;
struct VE_EXPORT veBlendFunc
{
	static veBlendFunc DISABLE;
	static veBlendFunc ADDITIVE;
	static veBlendFunc ALPHA;

	GLenum src;
	GLenum dst;

	inline bool operator == (const veBlendFunc& rkbf) const {
		return (src == rkbf.src && dst == rkbf.dst);
	}

	inline bool operator != (const veBlendFunc& rkbf) const {
		return (src != rkbf.src || dst != rkbf.dst);
	}
};

class VE_EXPORT vePass
{
	friend class veUniform;
public:
	static vePass* CURRENT_PASS;
	static bool CURRENT_DEPTH_TEST;
	static bool CURRENT_DEPTH_WRITE;
	static bool CURRENT_CULL_FACE;
	static GLenum CURRENT_CULL_FACE_MODE;
	static veBlendFunc CURRENT_BLEND_FUNC;
	//static GLenum CURRENT_POLYGON_MODE;

	vePass();
	~vePass();

	USE_VE_PTR;

	void visit(const veRenderCommand &command);
	bool apply(const veRenderCommand &command);

	const bool& depthTest() const { return _depthTest; };
	bool& depthTest() { return _depthTest; }

	const bool& depthWrite() const { return _depthWirte; }
	bool& depthWrite(){ return _depthWirte; }

	const bool& cullFace() const{ return _cullFace; }
	bool& cullFace(){ return _cullFace; }

	const GLenum& cullFaceMode() const { return _cullFaceMode; }
	GLenum& cullFaceMode() { return _cullFaceMode; }

	const veBlendFunc& blendFunc() const { return _blendFunc; }
	veBlendFunc& blendFunc() { return _blendFunc; }

	const unsigned int& drawMask() const { return _mask; }
	unsigned int& drawMask() { return _mask; }

	//const GLenum& polygonMode() const { return _polygonMode; }
	//GLenum& polygonMode() { return _polygonMode; }

	void setShader(veShader *shader);
	veShader* getShader(veShader::Type type);

	void addTexture(veTexture *texture);
	void setTexture(size_t idx, veTexture *texture);
	veTexture* getTexture(size_t idx);
	const veTexture* getTexture(size_t idx) const;
	veTexture* removeTexture(size_t idx);
	size_t getTextureNum() const { return _textures.size(); }

	void addUniform(veUniform *uniform);
	veUniform* getUniform(size_t idx);
	veUniform* removeUniform(size_t idx);
	size_t getUniformNum() const { return _uniforms.size(); }

	void setTransformFeedback(veTransformFeedback *transFeedback) { _transformFeedback = transFeedback; _needLinkProgram = true; }
	veTransformFeedback* getTransformFeedback() { return _transformFeedback.get(); }

	void needLink();

	static void restoreGLState();

private:

	void applyProgram(const veRenderCommand &command);
	void applyUniforms(const veRenderCommand &command);
	void applyLightsUniforms(const veRenderCommand &command);
	void applyLightUniforms(unsigned int idx, veLight *light, const veRenderCommand &command);
	void applyLightTextures(unsigned int beginTexUnit, const veRenderCommand &command);
	void locateLightUnifroms(const veRenderCommand &command);

private:

	struct LightUniformLocations
	{
		GLint lightSamples;
		GLint dirLightVisible;
		GLint pointLightVisible;
		GLint spotLightVisible;
		GLint dirLightShadowMap;
		GLint pointLightShadowMap;
		GLint spotLightShadowMap;
		std::vector<GLint> dirlightParams;
		std::vector<GLint> pointlightParams;
		std::vector<GLint> spotlightParams;
	};

	bool _depthTest;
	bool _depthWirte;
	bool _cullFace;
	GLenum _cullFaceMode;
	veBlendFunc _blendFunc;
	unsigned int _mask;
	//GLenum _polygonMode;
	GLuint _program;
	bool    _needLinkProgram;
	std::map<veShader::Type, VE_Ptr<veShader> >           _shaders;
	std::vector< VE_Ptr<veTexture> >                      _textures;
	std::vector< VE_Ptr<veUniform> >                      _uniforms;
	LightUniformLocations                                 _lightUniformLocations;
	VE_Ptr<veTransformFeedback>                           _transformFeedback;
};

class VE_EXPORT veTechnique
{
public:
	veTechnique();
	~veTechnique();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void addPass(vePass *pass);
	const vePass* getPass(size_t idx) const;
	vePass* getPass(size_t idx);
	vePass* removePass(size_t idx);
	size_t getPassNum() const { return _passes.size(); }

private:

	std::vector< VE_Ptr<vePass> > _passes;
};

class VE_EXPORT veMaterial
{
public:

	static const std::string SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY;
	static const std::string SYSTEM_MATERIAL_DIRECTIONAL_SHADOW_MAP_FOR_ENTITY;
	static const std::string SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ANIM_ENTITY;
	static const std::string SYSTEM_MATERIAL_OMNIDIRECTIONAL_SHADOW_MAP_FOR_ENTITY;
	static const std::string SYSTEM_MATERIAL_TEXTURES;

	veMaterial();
	~veMaterial();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	veTechnique* activeTechnique() { return _activeTechnique; }
	void activateTechnique(veTechnique *tech) { _activeTechnique = tech; };
	void addTechnique(veTechnique *tech);
	const veTechnique* getTechnique(size_t idx) const;
	veTechnique* getTechnique(size_t idx);
	const veTechnique* getTechnique(const std::string &name) const;
	veTechnique* getTechnique(const std::string &name);
	veTechnique* removeTechnique(size_t idx);
	size_t getTechniqueNum() const { return _techniques.size(); }

private:

	std::vector< VE_Ptr<veTechnique> > _techniques;
	veTechnique                       *_activeTechnique;
};

class VE_EXPORT veMaterialArray
{
	friend class veMaterialManager;
public:

	~veMaterialArray();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void addMaterial(veMaterial *material);
	const veMaterial* getMaterial(unsigned int idx) const;
	veMaterial* getMaterial(unsigned int idx);
	const veMaterial* getMaterial(const std::string &name) const;
	veMaterial* getMaterial(const std::string &name);
	size_t getMaterialNum() const { return _materials.size(); }

private:

	veMaterialArray();

private:

	std::vector< VE_Ptr<veMaterial> > _materials;
};

#endif