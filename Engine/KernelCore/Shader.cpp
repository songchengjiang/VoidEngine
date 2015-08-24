#include "Shader.h"
#include "Material.h"
#include "Constants.h"
#include "FileCore/File.h"

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

veUniform::veUniform(const std::string &name, const std::string &val)
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

void veUniform::apply(vePass *pass)
{
	if (_location < 0) _location = glGetUniformLocation(pass->_program, _name.c_str());

	switch (_type)
	{
	case INT:
	case BOOL:
		glUniform1i(_location, _values[0]);
		break;

	case REAL:
		glUniform1f(_location, _values[0]);
		break;

	case VEC2:
		glUniform2f(_location, _values[0], _values[1]);
		break;

	case VEC3:
		glUniform3f(_location, _values[0], _values[1], _values[2]);
		break;

	case VEC4:
		glUniform4f(_location, _values[0], _values[1], _values[2], _values[3]);
		break;

	case MAT3:
		glUniformMatrix3fv(_location, 1, false, _values.buffer());
		break;

	case MAT4:
		glUniformMatrix4fv(_location, 1, false, _values.buffer());
		break;

	case AUTO:
		{
			veMat4 mv = pass->V() * pass->M();
			if (_autoBindingValue == MVP_MATRIX){
				veMat4 mvp = pass->P() * mv;
				glUniformMatrix4fv(_location, 1, true, mvp[0]);
			}
			else if (_autoBindingValue == MV_MATRIX){
				glUniformMatrix4fv(_location, 1, true, mv[0]);
			}
			else if (_autoBindingValue == P_MATRIX){
				glUniformMatrix4fv(_location, 1, true, pass->P()[0]);
			}
			else if (_autoBindingValue == NORMAL_MATRIX){
				veMat3 normMat(mv[0][0], mv[0][1], mv[0][2]
					, mv[1][0], mv[1][1], mv[1][2]
					, mv[2][0], mv[2][1], mv[2][2]);
				normMat.inverse();
				normMat.transpose();
				glUniformMatrix3fv(_location, 1, true, normMat[0]);
			}
		}
		break;

	default:
		break;
	}
}

void veUniform::setValue(int val)
{
	_type = INT;
	_values.resize(1);
	_values[0] = val;
	_location = -1;
}

void veUniform::setValue(bool val)
{
	_type = BOOL;
	_values.resize(1);
	_values[0] = val;
	_location = -1;
}

void veUniform::setValue(veReal val)
{
	_type = REAL;
	_values.resize(1);
	_values[0] = val;
	_location = -1;
}

void veUniform::setValue(const std::string &val)
{
	_type = AUTO;
	_autoBindingValue = val;
	_location = -1;
}

void veUniform::setValue(const veVec2& val)
{
	_type = VEC2;
	_values.resize(2);
	_values[0] = val.x();
	_values[1] = val.y();
	_location = -1;
}

void veUniform::setValue(const veVec3& val)
{
	_type = VEC3;
	_values.resize(3);
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
	_location = -1;
}

void veUniform::setValue(const veVec4& val)
{
	_type = VEC4;
	_values.resize(4);
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
	_values[3] = val.w();
	_location = -1;
}

void veUniform::setValue(const veMat3& val)
{
	_type = MAT3;
	_values.resize(9);
	_values[0] = val[0][0]; _values[3] = val[0][1]; _values[6] = val[0][2];
	_values[1] = val[1][0]; _values[4] = val[1][1]; _values[7] = val[1][2];
	_values[2] = val[2][0]; _values[5] = val[2][1]; _values[8] = val[2][2];
	_location = -1;
}

void veUniform::setValue(const veMat4& val)
{
	_type = MAT4;
	_values.resize(16);
	_values[0] = val[0][0]; _values[4] = val[0][1]; _values[8]  = val[0][2]; _values[12] = val[0][3];
	_values[1] = val[1][0]; _values[5] = val[1][1]; _values[9]  = val[1][2]; _values[13] = val[1][3];
	_values[2] = val[2][0]; _values[6] = val[2][1]; _values[10] = val[2][2]; _values[14] = val[2][3];
	_values[3] = val[3][0]; _values[7] = val[3][1]; _values[11] = val[3][2]; _values[15] = val[3][3];
	_location = -1;
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

bool veUniform::getValue(std::string &val)
{
	if (_type != AUTO) return false;
	val = _autoBindingValue;
	return true;
}

veShader::veShader(Type type, const std::string &filePath)
	: USE_VE_PTR_INIT
	, _type(type)
	, _shader(0)
{
	_source = veFile::instance()->readFileToBuffer(filePath);
}

veShader::veShader(Type type, const char *str)
	: USE_VE_PTR_INIT
	, _type(type)
	, _shader(0)
	, _source(str)
{

}

veShader::veShader()
	: USE_VE_PTR_INIT
	, _shader(0)
{

}

veShader::~veShader()
{

}

void veShader::apply(vePass * pass)
{
	for (auto &iter : _uniforms){
		iter->apply(pass);
	}
}

void veShader::setSource(const std::string &filePath)
{
	_source = veFile::instance()->readFileToBuffer(filePath);
	_isCompiled = false;
}

void veShader::setSource(const char *str)
{
	_source = str;
	_isCompiled = false;
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

GLuint veShader::compile()
{
	GLint state = GL_FALSE;

	if (!_isCompiled){
		if (!_shader)
			_shader = glCreateShader(_type);

		char *buffer = new char[_source.size() + 1];
		strcpy(buffer, _source.c_str());
		glShaderSource(_shader, 1, &buffer, nullptr);

		glCompileShader(_shader);
		glGetShaderiv(_shader, GL_COMPILE_STATUS, &state);
		if (!state){
			GLint maxLen;
			glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &maxLen);
			if (maxLen > 0){
				GLchar *errors = new GLchar[maxLen];
				glGetShaderInfoLog(_shader, maxLen, &maxLen, errors);
				if (strcmp(errors, "") != 0){
					VE_PRINT(errors);
				}
				delete[] errors;
			}
		}
		delete[] buffer;
		_isCompiled = true;
	}

	return state == GL_TRUE ? _shader : 0;
}
