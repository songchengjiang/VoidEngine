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

class vePass;
class VE_EXPORT veUniform
{
public:

	enum Type
	{
		INT,
		BOOL,
		REAL,
		VEC2,
		VEC3,
		VEC4,
		MAT3,
		MAT4,
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

	bool getValue(int &val);
	bool getValue(bool &val);
	bool getValue(veReal &val);
	bool getValue(std::string &val);
	bool getValue(veVec2 &val);
	bool getValue(veVec3 &val);
	bool getValue(veVec4 &val);
	bool getValue(veMat3 &val);
	bool getValue(veMat4 &val);

private:
	
	std::string _name;
	std::string _autoBindingValue;
	Type        _type;
	veRealArray _values;
	GLint       _location;
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

	void apply(const veRenderCommand &command);

	void setType(Type type) { _type = type; }
	Type getType() const { return _type; }

	void setSource(const std::string &filePath);
	void setSource(const char *str);

	void addUniform(veUniform *uniform);
	veUniform* getUniform(unsigned int idx);
	veUniform* removeUniform(unsigned int idx);
	unsigned int getUniformNum() const { return _uniforms.size(); }

private:

	GLuint compile();

private:

	Type _type;
	GLuint _shader;
	std::string _source;
	bool _isCompiled;
	std::vector< VE_Ptr<veUniform> > _uniforms;
};

#endif