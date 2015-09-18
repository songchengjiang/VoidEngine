#ifndef _VE_SHADER_
#define _VE_SHADER_
#include "Prerequisites.h"
#include "VE_Ptr.h"
#include "BaseCore/Vector2.h"
#include "BaseCore/Vector3.h"
#include "BaseCore/Vector4.h"
#include "BaseCore/Matrix3.h"
#include "BaseCore/Matrix4.h"
#include "BaseCore/Array.h"
#include "RenderCommand.h"
#include <map>

class vePass;
class VE_EXPORT veUniform
{
public:

	enum class Type
	{
		INT,
		BOOL,
		REAL,
		VEC2,
		VEC3,
		VEC4,
		MAT3,
		MAT4,
		REAL_ARRAY,
		VEC2_ARRAY,
		VEC3_ARRAY,
		VEC4_ARRAY,
		MAT3_ARRAY,
		MAT4_ARRAY,
		AUTO,
	};

	veUniform(const std::string &name);
	veUniform(const std::string &name, int val);
	veUniform(const std::string &name, bool val);
	veUniform(const std::string &name, veReal val);
	veUniform(const std::string &name, const std::string &val);
	veUniform(const std::string &name, const veVec2& val);
	veUniform(const std::string &name, const veVec3& val);
	veUniform(const std::string &name, const veVec4& val);
	veUniform(const std::string &name, const veMat3& val);
	veUniform(const std::string &name, const veMat4& val);
	veUniform(const std::string &name, const veRealArray &val);
	veUniform(const std::string &name, const veVec2 *val, unsigned int n);
	veUniform(const std::string &name, const veVec3 *val, unsigned int n);
	veUniform(const std::string &name, const veVec4 *val, unsigned int n);
	veUniform(const std::string &name, const veMat3 *val, unsigned int n);
	veUniform(const std::string &name, const veMat4 *val, unsigned int n);

	~veUniform();

	USE_VE_PTR;

	void apply(const veRenderCommand &command);

	void setValue(int val);
	void setValue(bool val);
	void setValue(veReal val);
	void setValue(const std::string &val);
	void setValue(const veVec2& val);
	void setValue(const veVec3& val);
	void setValue(const veVec4& val);
	void setValue(const veMat3& val);
	void setValue(const veMat4& val);
	void setValue(const veRealArray &val);
	void setValue(const veVec2 *val, unsigned int n);
	void setValue(const veVec3 *val, unsigned int n);
	void setValue(const veVec4 *val, unsigned int n);
	void setValue(const veMat3 *val, unsigned int n);
	void setValue(const veMat4 *val, unsigned int n);

	bool getValue(int &val) const;
	bool getValue(bool &val) const;
	bool getValue(veReal &val) const;
	bool getValue(std::string &val) const;
	bool getValue(veVec2 &val) const;
	bool getValue(veVec3 &val) const;
	bool getValue(veVec4 &val) const;
	bool getValue(veMat3 &val) const;
	bool getValue(veMat4 &val) const;
	bool getValue(veRealArray &val) const;

	void setName(const std::string &name);
	const std::string& getName() const { return _name; }
	Type getType() const { return _type; }

private:
	
	std::string _name;
	std::string _autoBindingValue;
	Type        _type;
	veRealArray _values;
	GLint       _location;
	GLint       _preLocation;
	unsigned char _maxReLocation;
};

class VE_EXPORT veShader
{
	friend class vePass;
public:

	enum Type
	{
		VERTEX_SHADER   = GL_VERTEX_SHADER,
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
	};

	veShader();
	veShader(Type type, const std::string &filePath);
	veShader(Type type, const char *str);
	~veShader();

	USE_VE_PTR;

	void setType(Type type) { _type = type; }
	Type getType() const { return _type; }

	void setSource(const std::string &filePath);
	void setSource(const char *str);

	void setShaderHeader(Type type, const std::string &sHeader);

private:

	GLuint compile();

private:

	Type _type;
	GLuint _shader;
	std::string _source;
	std::unordered_map<Type, std::string> _shaderHeaders;
	bool _isCompiled;
};

class VE_EXPORT veShaderManager
{
public:
	~veShaderManager();

	static veShaderManager* instance();
	veShader* getOrCreateShader(const std::string &name);

private:
	veShaderManager();

private:

	std::map<std::string, VE_Ptr<veShader> > _shaders;
};

#endif