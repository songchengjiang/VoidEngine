#include "Shader.h"
#include "Material.h"
#include "Constants.h"
#include "FileCore/File.h"
#include "Mesh.h"
#include "MeshNode.h"
#include "Camera.h"
#include "EntityRenderer.h"
#include "SceneManager.h"

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
	if (_location < 0 && _maxReLocation == 255)
		return;
	if (_location < 0) {
		_location = glGetUniformLocation(command.pass->_program, _name.c_str());
		_location == _preLocation ? ++_maxReLocation : _maxReLocation = 0;
	}
	if (_location < 0) return;
	switch (_type)
	{
	case Type::INT:
	case Type::BOOL:
		glUniform1i(_location, _values[0]);
		break;

	case Type::REAL:
	case Type::REAL_ARRAY:
		glUniform1fv(_location, GLsizei(_values.size()), _values.buffer());
		break;

	case Type::VEC2:
	case Type::VEC2_ARRAY:
		glUniform2fv(_location, GLsizei(_values.size() / 2), _values.buffer());
		break;

	case Type::VEC3:
	case Type::VEC3_ARRAY:
		glUniform3fv(_location, GLsizei(_values.size() / 3), _values.buffer());
		break;

	case Type::VEC4:
	case Type::VEC4_ARRAY:
		glUniform4fv(_location, GLsizei(_values.size() / 4), _values.buffer());
		break;

	case Type::MAT3:
	case Type::MAT3_ARRAY:
		glUniformMatrix3fv(_location, GLsizei(_values.size() / 9), GL_FALSE, _values.buffer());
		break;

	case Type::MAT4:
	case Type::MAT4_ARRAY:
		glUniformMatrix4fv(_location, GLsizei(_values.size() / 16), GL_FALSE, _values.buffer());
		break;

	case Type::AUTO:
	{
		if (!_autoBindingValue.empty()) {

			if (_autoBindingValue == SCREEN_WIDTH) {
				glUniform1f(_location, command.camera->getViewport().width - command.camera->getViewport().x);
			}
			else if (_autoBindingValue == SCREEN_HEIGHT) {
				glUniform1f(_location, command.camera->getViewport().height - command.camera->getViewport().y);
			}
			else if (_autoBindingValue == SIM_TIME) {
				glUniform1f(_location, command.sceneManager->getSimulationTime());
			}
			else if (_autoBindingValue == SIM_SIN_TIME) {
				glUniform1f(_location, veMath::veSin(command.sceneManager->getSimulationTime()));
			}
			else {
				const veMat4 &worldMat = command.worldMatrix->value();
				const veMat4 &viewMat = command.camera->viewMatrix();
				const veMat4 &invViewMat = command.camera->getNodeToWorldMatrix();
				veMat4 modelViewMat = viewMat * worldMat;
				if (_autoBindingValue == MVP_MATRIX) {
					veMat4 mvp = command.camera->projectionMatrix() * modelViewMat;
					float m[16];
					m[0] = mvp[0][0]; m[4] = mvp[0][1]; m[8] = mvp[0][2]; m[12] = mvp[0][3];
					m[1] = mvp[1][0]; m[5] = mvp[1][1]; m[9] = mvp[1][2]; m[13] = mvp[1][3];
					m[2] = mvp[2][0]; m[6] = mvp[2][1]; m[10] = mvp[2][2]; m[14] = mvp[2][3];
					m[3] = mvp[3][0]; m[7] = mvp[3][1]; m[11] = mvp[3][2]; m[15] = mvp[3][3];
					glUniformMatrix4fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == MV_MATRIX) {
					float m[16];
					m[0] = modelViewMat[0][0]; m[4] = modelViewMat[0][1]; m[8] = modelViewMat[0][2]; m[12] = modelViewMat[0][3];
					m[1] = modelViewMat[1][0]; m[5] = modelViewMat[1][1]; m[9] = modelViewMat[1][2]; m[13] = modelViewMat[1][3];
					m[2] = modelViewMat[2][0]; m[6] = modelViewMat[2][1]; m[10] = modelViewMat[2][2]; m[14] = modelViewMat[2][3];
					m[3] = modelViewMat[3][0]; m[7] = modelViewMat[3][1]; m[11] = modelViewMat[3][2]; m[15] = modelViewMat[3][3];
					glUniformMatrix4fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == P_MATRIX) {
					const veMat4 &p = command.camera->projectionMatrix();
					float m[16];
					m[0] = p[0][0]; m[4] = p[0][1]; m[8] = p[0][2]; m[12] = p[0][3];
					m[1] = p[1][0]; m[5] = p[1][1]; m[9] = p[1][2]; m[13] = p[1][3];
					m[2] = p[2][0]; m[6] = p[2][1]; m[10] = p[2][2]; m[14] = p[2][3];
					m[3] = p[3][0]; m[7] = p[3][1]; m[11] = p[3][2]; m[15] = p[3][3];
					glUniformMatrix4fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == INV_P_MATRIX) {
					veMat4 invP = command.camera->projectionMatrix();
					invP.inverse();
					float m[16];
					m[0] = invP[0][0]; m[4] = invP[0][1]; m[8] = invP[0][2]; m[12] = invP[0][3];
					m[1] = invP[1][0]; m[5] = invP[1][1]; m[9] = invP[1][2]; m[13] = invP[1][3];
					m[2] = invP[2][0]; m[6] = invP[2][1]; m[10] = invP[2][2]; m[14] = invP[2][3];
					m[3] = invP[3][0]; m[7] = invP[3][1]; m[11] = invP[3][2]; m[15] = invP[3][3];
					glUniformMatrix4fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == NORMAL_MATRIX) {
					veMat3 normMat(modelViewMat[0][0], modelViewMat[0][1], modelViewMat[0][2]
						, modelViewMat[1][0], modelViewMat[1][1], modelViewMat[1][2]
						, modelViewMat[2][0], modelViewMat[2][1], modelViewMat[2][2]);
					normMat.inverse();
					normMat.transpose();
					float m[9];
					m[0] = normMat[0][0]; m[3] = normMat[0][1]; m[6] = normMat[0][2];
					m[1] = normMat[1][0]; m[4] = normMat[1][1]; m[7] = normMat[1][2];
					m[2] = normMat[2][0]; m[5] = normMat[2][1]; m[8] = normMat[2][2];
					glUniformMatrix3fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == M_MATRIX) {
					float m[16];
					m[0] = worldMat[0][0]; m[4] = worldMat[0][1]; m[8] = worldMat[0][2]; m[12] = worldMat[0][3];
					m[1] = worldMat[1][0]; m[5] = worldMat[1][1]; m[9] = worldMat[1][2]; m[13] = worldMat[1][3];
					m[2] = worldMat[2][0]; m[6] = worldMat[2][1]; m[10] = worldMat[2][2]; m[14] = worldMat[2][3];
					m[3] = worldMat[3][0]; m[7] = worldMat[3][1]; m[11] = worldMat[3][2]; m[15] = worldMat[3][3];
					glUniformMatrix4fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == V_MATRIX) {
					float m[16];
					m[0] = viewMat[0][0]; m[4] = viewMat[0][1]; m[8] = viewMat[0][2]; m[12] = viewMat[0][3];
					m[1] = viewMat[1][0]; m[5] = viewMat[1][1]; m[9] = viewMat[1][2]; m[13] = viewMat[1][3];
					m[2] = viewMat[2][0]; m[6] = viewMat[2][1]; m[10] = viewMat[2][2]; m[14] = viewMat[2][3];
					m[3] = viewMat[3][0]; m[7] = viewMat[3][1]; m[11] = viewMat[3][2]; m[15] = viewMat[3][3];
					glUniformMatrix4fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == INV_V_MATRIX) {
					float m[16];
					m[0] = invViewMat[0][0]; m[4] = invViewMat[0][1]; m[8] = invViewMat[0][2]; m[12] = invViewMat[0][3];
					m[1] = invViewMat[1][0]; m[5] = invViewMat[1][1]; m[9] = invViewMat[1][2]; m[13] = invViewMat[1][3];
					m[2] = invViewMat[2][0]; m[6] = invViewMat[2][1]; m[10] = invViewMat[2][2]; m[14] = invViewMat[2][3];
					m[3] = invViewMat[3][0]; m[7] = invViewMat[3][1]; m[11] = invViewMat[3][2]; m[15] = invViewMat[3][3];
					glUniformMatrix4fv(_location, 1, GL_FALSE, m);
				}
				else if (_autoBindingValue == BONE_MATRIXES) {
					static float boneMates[60 * 16];
					veNode *node = static_cast<veNode *>(command.user1);
					veMesh *mesh = static_cast<veMesh *>(command.user2);
					veMat4 worldToMesh = worldMat;
					worldToMesh.inverse();
					for (unsigned int i = 0; i < mesh->getBoneNum(); ++i) {
						unsigned int idx = 16 * i;
						const auto &bone = mesh->getBone(i);
						veMat4 boneMat = worldToMesh * node->getNodeToWorldMatrix() * bone->getBoneNode()->toMeshNodeRootMatrix() * bone->getOffsetMat();
						boneMates[idx + 0] = boneMat[0][0]; boneMates[idx + 4] = boneMat[0][1]; boneMates[idx + 8] = boneMat[0][2]; boneMates[idx + 12] = boneMat[0][3];
						boneMates[idx + 1] = boneMat[1][0]; boneMates[idx + 5] = boneMat[1][1]; boneMates[idx + 9] = boneMat[1][2]; boneMates[idx + 13] = boneMat[1][3];
						boneMates[idx + 2] = boneMat[2][0]; boneMates[idx + 6] = boneMat[2][1]; boneMates[idx + 10] = boneMat[2][2]; boneMates[idx + 14] = boneMat[2][3];
						boneMates[idx + 3] = boneMat[3][0]; boneMates[idx + 7] = boneMat[3][1]; boneMates[idx + 11] = boneMat[3][2]; boneMates[idx + 15] = boneMat[3][3];
					}
					glUniformMatrix4fv(_location, 60, GL_FALSE, boneMates);
				}
			}
			//uniform->setValue(values);
		}
	}
		break;

	default:
		break;
	}
}

void veUniform::setValue(int val)
{
	_type = Type::INT;
	_values.resize(1);
	_values[0] = val;
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(bool val)
{
	_type = Type::BOOL;
	_values.resize(1);
	_values[0] = val;
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(veReal val)
{
	_type = Type::REAL;
	_values.resize(1);
	_values[0] = val;
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const std::string &val)
{
	_type = Type::AUTO;
	_autoBindingValue = val;
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec2& val)
{
	_type = Type::VEC2;
	_values.resize(2);
	_values[0] = val.x();
	_values[1] = val.y();
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec3& val)
{
	_type = Type::VEC3;
	_values.resize(3);
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec4& val)
{
	_type = Type::VEC4;
	_values.resize(4);
	_values[0] = val.x();
	_values[1] = val.y();
	_values[2] = val.z();
	_values[3] = val.w();
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veMat3& val)
{
	_type = Type::MAT3;
	_values.resize(9);
	_values[0] = val[0][0]; _values[3] = val[0][1]; _values[6] = val[0][2];
	_values[1] = val[1][0]; _values[4] = val[1][1]; _values[7] = val[1][2];
	_values[2] = val[2][0]; _values[5] = val[2][1]; _values[8] = val[2][2];
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veMat4& val)
{
	_type = Type::MAT4;
	_values.resize(16);
	_values[0] = val[0][0]; _values[4] = val[0][1]; _values[8]  = val[0][2]; _values[12] = val[0][3];
	_values[1] = val[1][0]; _values[5] = val[1][1]; _values[9]  = val[1][2]; _values[13] = val[1][3];
	_values[2] = val[2][0]; _values[6] = val[2][1]; _values[10] = val[2][2]; _values[14] = val[2][3];
	_values[3] = val[3][0]; _values[7] = val[3][1]; _values[11] = val[3][2]; _values[15] = val[3][3];
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veRealArray &val)
{
	_type = Type::REAL_ARRAY;
	_values = val;
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec2 *val, unsigned int n)
{
	_type = Type::VEC2_ARRAY;
	_values.resize(2 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[2 * i]     = val[i].x();
		_values[2 * i + 1] = val[i].y();
	}
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec3 *val, unsigned int n)
{
	_type = Type::VEC3_ARRAY;
	_values.resize(3 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[3 * i]     = val[i].x();
		_values[3 * i + 1] = val[i].y();
		_values[3 * i + 2] = val[i].z();
	}
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veVec4 *val, unsigned int n)
{
	_type = Type::VEC4_ARRAY;
	_values.resize(4 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[4 * i] = val[i].x();
		_values[4 * i + 1] = val[i].y();
		_values[4 * i + 2] = val[i].z();
		_values[4 * i + 3] = val[i].w();
	}
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veMat3 *val, unsigned int n)
{
	_type = Type::MAT3_ARRAY;
	_values.resize(9 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[9 * i + 0] = val[i][0][0]; _values[9 * i + 3] = val[i][0][1]; _values[9 * i + 6] = val[i][0][2];
		_values[9 * i + 1] = val[i][1][0]; _values[9 * i + 4] = val[i][1][1]; _values[9 * i + 7] = val[i][1][2];
		_values[9 * i + 2] = val[i][2][0]; _values[9 * i + 5] = val[i][2][1]; _values[9 * i + 8] = val[i][2][2];
	}
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

void veUniform::setValue(const veMat4 *val, unsigned int n)
{
	_type = Type::MAT4_ARRAY;
	_values.resize(16 * n);
	for (unsigned int i = 0; i < n; ++i) {
		_values[16 * i + 0] = val[i][0][0]; _values[16 * i + 4] = val[i][0][1]; _values[16 * i + 8]  = val[i][0][2]; _values[16 * i + 12] = val[i][0][3];
		_values[16 * i + 1] = val[i][1][0]; _values[16 * i + 5] = val[i][1][1]; _values[16 * i + 9]  = val[i][1][2]; _values[16 * i + 13] = val[i][1][3];
		_values[16 * i + 2] = val[i][2][0]; _values[16 * i + 6] = val[i][2][1]; _values[16 * i + 10] = val[i][2][2]; _values[16 * i + 14] = val[i][2][3];
		_values[16 * i + 3] = val[i][3][0]; _values[16 * i + 7] = val[i][3][1]; _values[16 * i + 11] = val[i][3][2]; _values[16 * i + 15] = val[i][3][3];
	}
	if (0 <= _location)
		_preLocation = _location;
	if (_maxReLocation < 255)
		_location = -1;
}

bool veUniform::getValue(int &val) const
{
	if (_type != Type::INT) return false;
	val = _values[0];
	return true;
}

bool veUniform::getValue(bool &val) const
{
	if (_type != Type::BOOL) return false;
	val = _values[0] == 0? false : true; 
	return true;
}

bool veUniform::getValue(veReal &val) const
{
	if (_type != Type::REAL) return false;
	val = _values[0];
	return true;
}

bool veUniform::getValue(veVec2 &val) const
{
	if (_type != Type::VEC2) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	return true;
}

bool veUniform::getValue(veVec3 &val) const
{
	if (_type != Type::VEC3) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	val.z() = _values[2];
	return true;
}

bool veUniform::getValue(veVec4 &val) const
{
	if (_type != Type::VEC4) return false;
	val.x() = _values[0];
	val.y() = _values[1];
	val.z() = _values[2];
	val.w() = _values[3];
	return true;
}

bool veUniform::getValue(veMat3 &val) const
{
	if (_type != Type::MAT3) return false;
	val[0][0] = _values[0]; val[0][1] = _values[3]; val[0][2] = _values[6];
	val[1][0] = _values[1]; val[1][1] = _values[4]; val[1][2] = _values[7];
	val[2][0] = _values[2]; val[2][1] = _values[5]; val[2][2] = _values[8];
	return true;
}

bool veUniform::getValue(veMat4 &val) const
{
	if (_type != Type::MAT4) return false;
	val[0][0] = _values[0]; val[0][1] = _values[4]; val[0][2] = _values[8];  val[0][3] = _values[12];
	val[1][0] = _values[1]; val[1][1] = _values[5]; val[1][2] = _values[9];  val[1][3] = _values[13];
	val[2][0] = _values[2]; val[2][1] = _values[6]; val[2][2] = _values[10]; val[2][3] = _values[14];
	val[3][0] = _values[3]; val[3][1] = _values[7]; val[3][2] = _values[11]; val[3][3] = _values[15];
	return true;
}

bool veUniform::getValue(std::string &val) const
{
	val = _autoBindingValue;
	return true;
}

bool veUniform::getValue(veRealArray &val) const
{
	if (_type != Type::REAL_ARRAY) return false;
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
	setSource(filePath);
}

veShader::veShader(Type type, const char *str)
	: USE_VE_PTR_INIT
	, _type(type)
	, _shader(0)
	, _source(str)
	, _isCompiled(false)
{
	setSource(str);
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
	auto fileData = veFile::instance()->readFileToBuffer(filePath);
	_source.assign(fileData->buffer, fileData->size);
	_isCompiled = false;
}

void veShader::setSource(const char *str)
{
	_source = str;
	_isCompiled = false;
}

void veShader::setShaderHeader(Type type, const std::string &sHeader)
{
	_shaderHeaders[type] = sHeader;
}

GLuint veShader::compile()
{
	if (!_shader)
		_shader = glCreateShader(_type);

	std::string source = _shaderHeaders[_type] + _source;
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
			GLchar *errors = new GLchar[maxLen + 1];
			glGetShaderInfoLog(_shader, maxLen, &maxLen, errors);
			if (strcmp(errors, "") != 0) {
                std::string shaderType = typeToString();
                veLog("%s Shader Errors\n", shaderType.c_str());
				veLog(errors);
				veLog("%s Shader Source\n", shaderType.c_str());
                veLog(source.c_str());
			}
			delete[] errors;
		}
		glDeleteShader(_shader);
		_shader = 0;
	}
	delete[] buffer;
	return _shader;
}

std::string veShader::typeToString()
{
    switch (_type) {
        case VERTEX_SHADER:
            return "VERTEX_SHADER";
            
        case FRAGMENT_SHADER:
            return "FRAGMENT_SHADER";
            
        default:
            return "undefine";
    }
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
