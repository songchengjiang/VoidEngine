#ifndef _VE_MATERIAL_
#define _VE_MATERIAL_
#include "Prerequisites.h"
#include "Shader.h"
#include "Texture.h"
#include "VE_Ptr.h"
#include "RenderCommand.h"
#include <unordered_map>

struct veRenderCommand;
class veRenderer;
class veFrameBufferObject;

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

	vePass();
	~vePass();

	USE_VE_PTR;

	void visit(const veRenderCommand &command);
	void apply(const veRenderCommand &command);

	const bool& depthTest() const { return _depthTest; };
	bool& depthTest() { return _depthTest; }

	const bool& depthWrite() const { return _depthWirte; }
	bool& depthWrite(){ return _depthWirte; }

	const bool& cullFace() const{ return _cullFace; }
	bool& cullFace(){ return _cullFace; }

	const veBlendFunc& blendFunc() const { return _blendFunc; }
	veBlendFunc& blendFunc() { return _blendFunc; }

	const unsigned int& drawMask() const { return _mask; }
	unsigned int& drawMask() { return _mask; }

	const GLenum& polygonMode() const { return _polygonMode; }
	GLenum& polygonMode() { return _polygonMode; }

	void setShader(veShader *shader);
	veShader* getShader(veShader::Type type);

	void addTexture(veTexture *texture);
	void setTexture(unsigned int idx, veTexture *texture);
	veTexture* getTexture(unsigned int idx);
	veTexture* removeTexture(unsigned int idx);
	unsigned int getTextureNum() const { return _textures.size(); }

private:

	void applyProgram();

private:

	bool _depthTest;
	bool _depthWirte;
	bool _cullFace;
	veBlendFunc _blendFunc;
	unsigned int _mask;
	GLenum _polygonMode;
	GLuint _program;
	std::unordered_map<veShader::Type, VE_Ptr<veShader> > _shaders;
	std::vector< VE_Ptr<veTexture> >                      _textures;
};

class VE_EXPORT veTechnique
{
public:
	veTechnique();
	~veTechnique();

	USE_VE_PTR;
	USE_NAME_PROPERTY;

	void addPass(vePass *pass);
	const vePass* getPass(unsigned int idx) const;
	vePass* getPass(unsigned int idx);
	vePass* removePass(unsigned int idx);
	unsigned int getPassNum() const { return _passes.size(); }

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
	const veTechnique* getTechnique(unsigned int idx) const;
	veTechnique* getTechnique(unsigned int idx);
	const veTechnique* getTechnique(const std::string &name) const;
	veTechnique* getTechnique(const std::string &name);
	veTechnique* removeTechnique(unsigned int idx);
	unsigned int getTechniqueNum() const { return _techniques.size(); }

private:

	std::vector< VE_Ptr<veTechnique> > _techniques;
	veTechnique                       *_activeTechnique;
};

class VE_EXPORT veMaterialArray
{
public:

	veMaterialArray();
	~veMaterialArray();

	USE_VE_PTR;

	void addMaterial(veMaterial *material);
	const veMaterial* getMaterial(unsigned int idx) const;
	veMaterial* getMaterial(unsigned int idx);
	const veMaterial* getMaterial(const std::string &name) const;
	veMaterial* getMaterial(const std::string &name);
	unsigned int getMaterialNum() const { return _materials.size(); }

private:

	std::vector< VE_Ptr<veMaterial> > _materials;
};

#endif