#include "Shader.h"
#include "Material.h"
#include "Constants.h"
#include "FileCore/File.h"
#include "Visualiser.h"

veUniform::veUniform(const std::string &name)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
}

veUniform::veUniform(const std::string &name, int val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, veReal val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const std::string &val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veVec2& val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veVec3& val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veVec4& val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veMat3& val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veMat4& val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::veUniform(const std::string &name, const veRealArray &val)
	: USE_VE_PTR_INIT
	, _name(name)
	, _location(-1)
	, _preLocation(-1)
	, _maxReLocation(0)
{
	setValue(val);
}

veUniform::~veUniform()
{

}

void veUniform::apply(const veRenderCommand &command)
{
	command.renderer->uniformUpdate(this, command);
	if (_location < 0) {
		_location = glGetUniformLocation(command.pass->_program, _name.c_str());
		_location == _preLocation ? ++_maxReLocation : _maxReLocation = 0;
	}
	if (_location < 0) return;
	switch (_type)
	{
	case INT:
	case BOOL:
		glUniform1i(_location, _values[0]);
		break;

	case REAL:
	case REAL_ARRAY:
		glUniform1fv(_location, _values.size(), _values.buffer());
		break;

	case VEC2:
	case VEC2_ARRAY:
		glUniform2fv(_location, _values.size() / 2, _values.buffer());
		break;

	case VEC3:
	case VEC3_ARRAY:
		glUniform3fv(_location, _values.size() / 3, _values.buffer());
		break;

	case VEC4:
	case VEC4_ARRAY:
		glUniform4fv(_location, _values.size() / 4, _values.buffer());
		break;

	case MAT3:
	case MAT3_ARRAY:
		glUniformMatrix3fv(_location, _values.size() / 9, GL_FALSE, _values.buffer());
		break;

	case MAT4:
	case MAT4_ARRAY:
		glUniformMatrix4fv(_location, _values.size() / 16, GL_FALSE, _values.buffer());
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
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(bool val)
{
	_type = BOOL;
	_values.resize(1);
	_values[0] = val;
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(veReal val)
{
	_type = REAL;
	_values.resize(1);
	_values[0] = val;
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const std::string &val)
{
	_type = AUTO;
	_autoBindingValue = val;
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec2& val)
{
	_type = VEC2;
	_values.resize(2);
	_values[0] = val.x();
	_values[1] = val.y();
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec3& val)
{
	_type = VEC3;
	_values.resize(3);
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
	_preLocation = _location;
	if (_maxReLocation < 255)
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
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veMat3& val)
{
	_type = MAT3;
	_values.resize(9);
	_values[0] = val[0][0]; _values[3] = val[0][1]; _values[6] = val[0][2];
	_values[1] = val[1][0]; _values[4] = val[1][1]; _values[7] = val[1][2];
	_values[2] = val[2][0]; _values[5] = val[2][1]; _values[8] = val[2][2];
	_preLocation = _location;
	if (_maxReLocation < 255)
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
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veRealArray &val)
{
	_type = REAL_ARRAY;
	_values = val;
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec2 *val, unsigned int n)
{
	_type = VEC2_ARRAY;
	_values.resize(2 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[2 * i]     = val[i].x();
		_values[2 * i + 1] = val[i].y();
	}
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec3 *val, unsigned int n)
{
	_type = VEC3_ARRAY;
	_values.resize(3 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[3 * i]     = val[i].x();
		_values[3 * i + 1] = val[i].y();
		_values[3 * i + 2] = val[i].z();
	}
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec4 *val, unsigned int n)
{
	_type = VEC4_ARRAY;
	_values.resize(4 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[4 * i] = val[i].x();
		_values[4 * i + 1] = val[i].y();
		_values[4 * i + 2] = val[i].z();
		_values[4 * i + 3] = val[i].w();
	}
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veMat3 *val, unsigned int n)
{
	_type = MAT3_ARRAY;
	_values.resize(9 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[9 * i + 0] = val[i][0][0]; _values[9 * i + 3] = val[i][0][1]; _values[9 * i + 6] = val[i][0][2];
		_values[9 * i + 1] = val[i][1][0]; _values[9 * i + 4] = val[i][1][1]; _values[9 * i + 7] = val[i][1][2];
		_values[9 * i + 2] = val[i][2][0]; _values[9 * i + 5] = val[i][2][1]; _values[9 * i + 8] = val[i][2][2];
	}
	_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veMat4 *val, unsigned int n)
{
	_type = MAT4_ARRAY;
	_values.resize(16 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[16 * i + 0] = val[i][0][0]; _values[16 * i + 4] = val[i][0][1]; _values[16 * i + 8]  = val[i][0][2]; _values[16 * i + 12] = val[i][0][3];
		_values[16 * i + 1] = val[i][1][0]; _values[16 * i + 5] = val[i][1][1]; _values[16 * i + 9]  = val[i][1][2]; _values[16 * i + 13] = val[i][1][3];
		_values[16 * i + 2] = val[i][2][0]; _values[16 * i + 6] = val[i][2][1]; _values[16 * i + 10] = val[i][2][2]; _values[16 * i + 14] = val[i][2][3];
		_values[16 * i + 3] = val[i][3][0]; _values[16 * i + 7] = val[i][3][1]; _values[16 * i + 11] = val[i][3][2]; _values[16 * i + 15] = val[i][3][3];
	}
	_preLocation = _location;
	if (_maxReLocation < 255)
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
	val = _autoBindingValue;
	return true;
}

bool veUniform::getValue(veRealArray &val)
{
	if (_type != REAL_ARRAY) return false;
	val = _values;
	return true;
}

void veUniform::setName(const std::string &name)
{
	_name = name;
	_location = -1;
	_preLocation = -1;
	_maxReLocation = 0;
}

veShader::veShader(Type type, const std::string &filePath)
	: USE_VE_PTR_INIT
	, _type(type)
	, _shader(0)
	, _isCompiled(false)
{
	_source = veFile::instance()->readFileToBuffer(filePath);
}

veShader::veShader(Type type, const char *str)
	: USE_VE_PTR_INIT
	, _type(type)
	, _shader(0)
	, _source(str)
	, _isCompiled(false)
{

}

veShader::veShader()
	: USE_VE_PTR_INIT
	, _shader(0)
	, _isCompiled(false)
{

}

veShader::~veShader()
{

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

GLuint veShader::compile(const std::string &definations)
{
	if (!_shader)
		_shader = glCreateShader(_type);

	std::string source = definations + _source;
	char *buffer = new char[source.size() + 1];
	strcpy(buffer, source.c_str());
	glShaderSource(_shader, 1, &buffer, nullptr);

	GLint state = GL_FALSE;
	glCompileShader(_shader);
	glGetShaderiv(_shader, GL_COMPILE_STATUS, &state);
	if (!state) {
		GLint maxLen;
		glGetShaderiv(_shader, GL_INFO_LOG_LENGTH, &maxLen);
		if (maxLen > 0) {
			GLchar *errors = new GLchar[maxLen];
			glGetShaderInfoLog(_shader, maxLen, &maxLen, errors);
			if (strcmp(errors, "") != 0) {
				VE_PRINT(errors);
			}
			delete[] errors;
		}
		glDeleteShader(_shader);
		_shader = 0;
	}
	delete[] buffer;
	return _shader;
}

veShaderManager::~veShaderManager()
{

}

veShaderManager* veShaderManager::instance()
{
	static veShaderManager manager;
	return &manager;
}

veShader* veShaderManager::getOrCreateShader(const std::string &name)
{
	auto iter = _shaders.find(name);
	if (iter != _shaders.end()) return iter->second.get();
	auto shader = new veShader();
	_shaders[name] = shader;
	return shader;
}

veShaderManager::veShaderManager()
{

}
