#ifndef _VE_MATERIAL_
#define _VE_MATERIAL_
#include "Prerequisites.h"
#include "Shader.h"
#include "Texture.h"
#include "VE_Ptr.h"
#include "RenderCommand.h"
#include "RenderState.h"
#include "TransformFeedback.h"
#include "GLDataBuffer.h"
#include <unordered_map>

struct veRenderCommand;
class veRenderer;
class veFrameBufferObject;
class veLight;

class VE_EXPORT vePass
{
	friend class veUniform;
public:
	static vePass* CURRENT_PASS;
	typedef std::function<void()> ApplyFunctionCallback;
    
    enum RenderPass{
        DEFERRED_PASS,
        FORWARD_PASS,
    };

	enum TextureType
	{
		AMBIENT_TEXTURE,
		DIFFUSE_TEXTURE,
		SPECULAR_TEXTURE,
		EMISSIVE_TEXTURE,
		NORMAL_TEXTURE,
		HEIGHT_TEXTURE,
		SHININESS_TEXTURE,
		OPACITYT_TEXTURE,
		DISPLACEMENT_TEXTURE,
		LIGHTMAP_TEXTURE,
        ROUGHNESS_TEXTURE,
		REFLECTION_TEXTURE,
        
        //USED FOR FBO
        COLOR_BUFFER_TEXTURE,
        DEPTH_STENCIL_BUFFER_TEXTURE,
	};
	vePass();
	~vePass();

	USE_VE_PTR;

	void visit(const veRenderCommand &command);
	bool apply(const veRenderCommand &command);

    void setRenderPass(RenderPass rp) { _renderPass = rp; }
    RenderPass getRenderPass() const { return _renderPass; }
    
	const bool& depthTest() const { return _depthTest; };
	bool& depthTest() { return _depthTest; }

	const bool& depthWrite() const { return _depthWirte; }
	bool& depthWrite() { return _depthWirte; }

	const bool& stencilTest() const { return _stencilTest; };
	bool& stencilTest() { return _stencilTest; }

	const bool& cullFace() const { return _cullFace; }
	bool& cullFace() { return _cullFace; }

	const GLenum& cullFaceMode() const { return _cullFaceMode; }
	GLenum& cullFaceMode() { return _cullFaceMode; }

	const veBlendFunc& blendFunc() const { return _blendFunc; }
	veBlendFunc& blendFunc() { return _blendFunc; }

	const GLenum& blendEquation() const { return _blendEquation; }
	GLenum& blendEquation() { return _blendEquation; }

	const veStencilFunc& stencilFunc() const { return _stencilFunc; }
	veStencilFunc& stencilFunc() { return _stencilFunc; }

	const veStencilOp& stencilOp() const { return _stencilOp; }
	veStencilOp& stencilOp() { return _stencilOp; }

	const unsigned int& drawMask() const { return _mask; }
	unsigned int& drawMask() { return _mask; }

	const bool& castShadow() const { return _castShadow; };
	bool& castShadow() { return _castShadow; }

	void setShader(veShader *shader);
	veShader* getShader(veShader::Type type);

	//void addTexture(veTexture *texture);
	//void setTexture(size_t idx, veTexture *texture);
	void setTexture(TextureType type, veTexture *texture);
	//veTexture* getTexture(size_t idx);
	veTexture* getTexture(TextureType type);
	const veTexture* getTexture(TextureType type) const;
	const veTexture* getTexture(size_t idx) const;
	//veTexture* removeTexture(size_t idx);
	size_t getTextureNum() const { return _textures.size(); }

	void addUniform(veUniform *uniform);
	veUniform* getUniform(const std::string &name) const;
    veUniform* getUniform(size_t idx) const;
	veUniform* removeUniform(const std::string &name);
    veUniform* removeUniform(size_t idx);
	size_t getUniformNum() const { return _uniforms.size(); }

	void setTransformFeedback(veTransformFeedback *transFeedback) { _transformFeedback = transFeedback; _needLinkProgram = true; }
	veTransformFeedback* getTransformFeedback() { return _transformFeedback.get(); }

	void setApplyFunctionCallback(const ApplyFunctionCallback &callback) { _callback = callback; }

	void reloadProgram();

private:

	void applyProgram(const veRenderCommand &command);
	void applyUniforms(const veRenderCommand &command);
	int findTextureID(TextureType type) const;

private:

    RenderPass _renderPass;
	bool _depthTest;
	bool _depthWirte;
	bool _stencilTest;
	bool _cullFace;
	bool _castShadow;
	GLenum _cullFaceMode;
	veBlendFunc _blendFunc;
	GLenum _blendEquation;
	veStencilFunc _stencilFunc;
	veStencilOp   _stencilOp;
	unsigned int _mask;
	//GLenum _polygonMode;
    VE_Ptr<veGLDataBuffer> _programBuffer;
	bool    _needLinkProgram;
	std::map<veShader::Type, VE_Ptr<veShader> >           _shaders;
	std::vector< std::pair<TextureType, VE_Ptr<veTexture> > >            _textures;
	std::vector< VE_Ptr<veUniform> >            _uniforms;
	VE_Ptr<veTransformFeedback>                           _transformFeedback;
	ApplyFunctionCallback                                 _callback;
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
	const veMaterial* getMaterial(size_t idx) const;
	veMaterial* getMaterial(size_t idx);
	const veMaterial* getMaterial(const std::string &name) const;
	veMaterial* getMaterial(const std::string &name);
	size_t getMaterialNum() const { return _materials.size(); }

private:

	veMaterialArray();

private:

	std::vector< VE_Ptr<veMaterial> > _materials;
};

#endif