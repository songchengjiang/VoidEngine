#include "Shader.h"
#include "Material.h"

veUniform::veUniform(const std::string &name)
	: USE_VE_PTR_INIT
	, _name(name)
{
}

veUniform::veUniform(const std::string &name, int val)
	: USE_VE_PTR_INIT
	, _name(name)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, veReal val)
	: USE_VE_PTR_INIT
	, _name(name)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veVec2& val)
	: USE_VE_PTR_INIT
	, _name(name)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veVec3& val)
	: USE_VE_PTR_INIT
	, _name(name)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veVec4& val)
	: USE_VE_PTR_INIT
	, _name(name)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veMat3& val)
	: USE_VE_PTR_INIT
	, _name(name)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veMat4& val)
	: USE_VE_PTR_INIT
	, _name(name)
{
	setValue(val);
}

veUniform::~veUniform()
{

}

void veUniform::setValue(int val)
{
	_type = INT;
	_values.resize(1);
	_values[0] = val;
}

void veUniform::setValue(bool val)
{
	_type = BOOL;
	_values.resize(1);
	_values[0] = val;
}

void veUniform::setValue(veReal val)
{
	_type = REAL;
	_values.resize(1);
	_values[0] = val;
}

void veUniform::setValue(const veVec2& val)
{
	_type = VEC2;
	_values.resize(2);
	_values[0] = val.x();
	_values[1] = val.y();
}

void veUniform::setValue(const veVec3& val)
{
	_type = VEC3;
	_values.resize(3);
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
}

void veUniform::setValue(const veVec4& val)
{
	_type = VEC4;
	_values.resize(4);
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
	_values[3] = val.w();
}

void veUniform::setValue(const veMat3& val)
{
	_type = MAT3;
	_values.resize(9);
	_values[0] = val[0][0]; _values[3] = val[0][1]; _values[6] = val[0][2];
	_values[1] = val[1][0]; _values[4] = val[1][1]; _values[7] = val[1][2];
	_values[2] = val[2][0]; _values[5] = val[2][1]; _values[8] = val[2][2];
}

void veUniform::setValue(const veMat4& val)
{
	_type = MAT4;
	_values.resize(16);
	_values[0] = val[0][0]; _values[4] = val[0][1]; _values[8]  = val[0][2]; _values[12] = val[0][3];
	_values[1] = val[1][0]; _values[5] = val[1][1]; _values[9]  = val[1][2]; _values[13] = val[1][3];
	_values[2] = val[2][0]; _values[6] = val[2][1]; _values[10] = val[2][2]; _values[14] = val[2][3];
	_values[3] = val[3][0]; _values[7] = val[3][1]; _values[11] = val[3][2]; _values[15] = val[3][3];
}

bool veUniform::getValue(int &val)
{
	if (_type != INT) return false;
	val = _values[0];
	return true;
}

bool veUniform::getValue(bool &val)
{
	if (_type != BOOL) return false;
	val = _values[0] == 0? false : true; 
	return true;
}

bool veUniform::getValue(veReal &val)
{
	if (_type != REAL) return false;
	val = _values[0];
	return true;
}

bool veUniform::getValue(veVec2 &val)
{
	if (_type != VEC2) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	return true;
}

bool veUniform::getValue(veVec3 &val)
{
	if (_type != VEC3) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	val.z() = _values[2];
	return true;
}

bool veUniform::getValue(veVec4 &val)
{
	if (_type != VEC4) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	val.z() = _values[2];
	val.w() = _values[3];
	return true;
}

bool veUniform::getValue(veMat3 &val)
{
	if (_type != MAT3) return false;
	val[0][0] = _values[0]; val[0][1] = _values[3]; val[0][2] = _values[6];
	val[1][0] = _values[1]; val[1][1] = _values[4]; val[1][2] = _values[7];
	val[2][0] = _values[2]; val[2][1] = _values[5]; val[2][2] = _values[8];
	return true;
}

bool veUniform::getValue(veMat4 &val)
{
	if (_type != MAT4) return false;
	val[0][0] = _values[0]; val[0][1] = _values[4]; val[0][2] = _values[8];  val[0][3] = _values[12];
	val[1][0] = _values[1]; val[1][1] = _values[5]; val[1][2] = _values[9];  val[1][3] = _values[13];
	val[2][0] = _values[2]; val[2][1] = _values[6]; val[2][2] = _values[10]; val[2][3] = _values[14];
	val[3][0] = _values[3]; val[3][1] = _values[7]; val[3][2] = _values[11]; val[3][3] = _values[15];
	return true;
}

veShader::veShader(Type type, const std::string &filePath)
	: USE_VE_PTR_INIT
	, _type(type)
{

}

veShader::veShader(Type type, const char *str)
	: USE_VE_PTR_INIT
	, _type(type)
{

}

veShader::veShader()
	: USE_VE_PTR_INIT
{

}

veShader::~veShader()
{

}

void veShader::apply(vePass * pass)
{
}

void veShader::setSource(const std::string &filePath)
{

}

void veShader::setSource(const char *str)
{

}

void veShader::addUniform(veUniform *uniform)
{
	_uniforms.push_back(uniform);
}

veUniform* veShader::getUniform(unsigned int idx)
{
	veAssert(idx < _uniforms.size());
	return _uniforms[idx].get();
}

veUniform* veShader::removeUniform(unsigned int idx)
{
	veAssert(idx < _uniforms.size());
	veUniform *uniform = _uniforms[idx].get();
	_uniforms.erase(_uniforms.begin() + idx);
	return uniform;
}
