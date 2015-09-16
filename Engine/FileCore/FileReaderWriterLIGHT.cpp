#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Light.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterLIGHT : public veFileReaderWriter
{
public:
	veFileReaderWriterLIGHT()
		: _light(nullptr)
	{};
	~veFileReaderWriterLIGHT(){};

	virtual void* readFile(const std::string &filePath){
		_fileFolder = filePath.substr(0, filePath.find_last_of("/\\") + 1);
		std::string buffer = veFile::instance()->readFileToBuffer(filePath);
		_doucument.Parse(buffer.c_str());
		readLight();
		return _light;
	}

	virtual bool writeFile(void *data, const std::string &filePath){
		return true;
	}

private:

	void readLight() {
		_light = nullptr;
		if (!readLightDef()) return;
		std::string name = _doucument[NAME_KEY.c_str()].GetString();
		const Value &paramsVals = _doucument[PARAMETERS_KEY.c_str()];
		//if (!paramsVals.Empty()) {
		//	_light = new veLight(_className);
		//	_light->setName(name);
		//	veLightManager::Parameters params;
		//	for (unsigned int i = 0; i < paramsVals.Size(); ++i) {
		//		const Value & pm = paramsVals[i];
		//		const Value & values = pm[VALUES_KEY.c_str()];
		//		auto paramter = _light->getParameter(pm[NAME_KEY.c_str()].GetString());
		//		if (paramter) {
		//			setValues(paramter, values);
		//		}
		//	}
		//}
	}

	bool readLightDef() {
		if (_doucument.HasMember(CLASS_KEY.c_str())) {
			std::string classFile = _doucument[CLASS_KEY.c_str()].GetString();
			std::string buffer = veFile::instance()->readFileToBuffer(_fileFolder + classFile);
			Document doc;
			doc.Parse(buffer.c_str());
			_className = doc[NAME_KEY.c_str()].GetString();
			veLightManager::LightTemplate lightTemplate;
			lightTemplate.limit = doc[LIMIT_KEY.c_str()].GetUint();
			const Value &paramsVals = doc[PARAMETERS_KEY.c_str()];
			if (!paramsVals.Empty()) {
				for (unsigned int i = 0; i < paramsVals.Size(); ++i) {
					const Value & pm = paramsVals[i];
					lightTemplate.parameters.push_back(std::make_pair(pm[NAME_KEY.c_str()].GetString(), getType(pm[TYPE_KEY.c_str()].GetString())));
				}
				veLightManager::instance()->addLightTemplate(_className, lightTemplate);
				return true;
			}
		}
		return false;
	}

	veUniform::Type getType(const char *str) {
		if (strncmp(INT_KEY.c_str(), str, INT_KEY.size()) == 0) {
			return veUniform::INT;
		}
		else if (strncmp(BOOL_KEY.c_str(), str, BOOL_KEY.size()) == 0) {
			return veUniform::BOOL;
		}
		else if (strncmp(FLOAT_KEY.c_str(), str, FLOAT_KEY.size()) == 0) {
			return veUniform::REAL;
		}
		else if (strncmp(VEC2_KEY.c_str(), str, VEC2_KEY.size()) == 0) {
			return veUniform::VEC2;
		}
		else if (strncmp(VEC3_KEY.c_str(), str, VEC3_KEY.size()) == 0) {
			return veUniform::VEC3;
		}
		else if (strncmp(VEC4_KEY.c_str(), str, VEC4_KEY.size()) == 0) {
			return veUniform::VEC4;
		}
		else if (strncmp(MAT3_KEY.c_str(), str, MAT3_KEY.size()) == 0) {
			return veUniform::MAT3;
		}
		else if (strncmp(MAT4_KEY.c_str(), str, MAT4_KEY.size()) == 0) {
			return veUniform::MAT4;
		}

		return veUniform::REAL;
	}

	void setValues(veUniform *paramter, const Value & values) {
		if (values.IsInt()) {
			paramter->setValue(values.GetInt());
		}
		else if (values.IsDouble()) {
			paramter->setValue((veReal)values.GetDouble());
		}
		else if (values.Size() == 2) {
			veVec2 vec2(values[0].GetDouble(), values[1].GetDouble());
			paramter->setValue(vec2);
		}
		else if (values.Size() == 3) {
			veVec3 vec3(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble());
			paramter->setValue(vec3);
		}
		else if (values.Size() == 4) {
			veVec4 vec4(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble(), values[3].GetDouble());
			paramter->setValue(vec4);
		}
		else if (values.Size() == 9) {
			veMat3 mat(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble()
				, values[3].GetDouble(), values[4].GetDouble(), values[5].GetDouble()
				, values[6].GetDouble(), values[7].GetDouble(), values[8].GetDouble());
			paramter->setValue(mat);
		}
		else if (values.Size() == 16) {
			veMat4 mat(values[0].GetDouble(), values[1].GetDouble(), values[2].GetDouble(), values[3].GetDouble()
				, values[4].GetDouble(), values[5].GetDouble(), values[6].GetDouble(), values[7].GetDouble()
				, values[8].GetDouble(), values[9].GetDouble(), values[10].GetDouble(), values[11].GetDouble()
				, values[12].GetDouble(), values[13].GetDouble(), values[14].GetDouble(), values[15].GetDouble());
			paramter->setValue(mat);
		}
	}

private:

	Document _doucument;
	std::string _fileFolder;
	std::string _className;
	veLight *_light;
};

VE_READERWRITER_REG("velight", veFileReaderWriterLIGHT);