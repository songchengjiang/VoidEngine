#ifndef _VE_MATERIAL_
#define _VE_MATERIAL_
#include "Prerequisites.h"
#include "Shader.h"
#include "Texture.h"
#include "VE_Ptr.h"
#include <unordered_map>

class VE_EXPORT vePass
{
public:

	vePass();
	~vePass();

	USE_VE_PTR;

	void apply();

	const bool& depthTest() const { return _depthTest; };
	bool& depthTest() { return _depthTest; }

	const bool& depthWrite() const { return _depthWirte; }
	bool& depthWrite(){ return _depthWirte; }

	const bool& cullFace() const{ return _cullFace; }
	bool& cullFace(){ return _cullFace; }

	const veMat4*& M() { return _M; }
	const veMat4*& V() { return _V; }
	const veMat4*& P() { return _P; }

	void setShader(veShader *shader);
	veShader* getShader(veShader::Type type);

	void addTexture(veTexture *texture);
	void setTexture(unsigned int idx, veTexture *texture);
	veTexture* getTexture(unsigned int idx);
	veTexture* removeTexture(unsigned int idx);
	unsigned int getTextureNum() const { return _textures.size(); }

private:

	bool _depthTest;
	bool _depthWirte;
	bool _cullFace;
	const veMat4 *_M;
	const veMat4 *_V;
	const veMat4 *_P;
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

	void addTechnique(veTechnique *tech);
	const veTechnique* getTechnique(unsigned int idx) const;
	veTechnique* getTechnique(unsigned int idx);
	const veTechnique* getTechnique(const std::string &name) const;
	veTechnique* getTechnique(const std::string &name);
	veTechnique* removeTechnique(unsigned int idx);
	unsigned int getTechniqueNum() const { return _techniques.size(); }

private:

	std::vector< VE_Ptr<veTechnique> > _techniques;
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
