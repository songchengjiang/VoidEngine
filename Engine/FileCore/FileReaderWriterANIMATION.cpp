#include "FileReaderWriter.h"
#include <rapidjson/include/document.h>
#include "Constants.h"
#include "KernelCore/Animation.h"
#include "KernelCore/SceneManager.h"
#include <unordered_map>

using namespace rapidjson;
class veFileReaderWriterANIMATION : public veFileReaderWriter
{
public:
	veFileReaderWriterANIMATION()
		: _doucument(nullptr)
	{};
	~veFileReaderWriterANIMATION(){
		for (auto & iter : _doucumentMap) {
			VE_SAFE_DELETE(iter.second);
		}
	};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		auto doc = _doucumentMap.find(fullPath);
		if (doc != _doucumentMap.end())
			_doucument = doc->second;
		else {
			_doucument = new Document;
			std::string buffer = veFile::readFileToBuffer(fullPath);
			_doucument->Parse(buffer.c_str());
			if (_doucument->HasParseError()) return  nullptr;
			_doucumentMap[fullPath] = _doucument;
		}
		_sceneManager = sm;
		_name = name;
		readAnimations();
		return _animationContainer;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readAnimations() {
		_animationContainer = _sceneManager->createAnimationContainer(_name);
		if ((*_doucument).HasMember(ANIMATIONS_KEY.c_str())) {
			const Value &anims = (*_doucument)[ANIMATIONS_KEY.c_str()];
			for (unsigned int i = 0; i < anims.Size(); ++i) {
				readAnimation(anims[i]);
			}
		}
	}

	void readAnimation(const Value &animVal) {
		veAnimation *animation = nullptr;
		if (animVal.HasMember(NAME_KEY.c_str())) {
			animation = _sceneManager->createAnimation(animVal[NAME_KEY.c_str()].GetString());
		}	
		if (!animation) return;

		if (animVal.HasMember(FRAMES_KEY.c_str())) {
			animation->setDuration(animVal[FRAMES_KEY.c_str()].GetDouble());
		}

		if (animVal.HasMember(FRAMERATE_KEY.c_str())) {
			animation->setFrameRate(animVal[FRAMERATE_KEY.c_str()].GetDouble());
		}

		if (animVal.HasMember(NODES_KEY.c_str())) {
			const Value &nodes = animVal[NODES_KEY.c_str()];
			for (unsigned int i = 0; i < nodes.Size(); ++i) {
				readAnimNode(nodes[i], animation);
			}
		}

		_animationContainer->addAnimationChannel(animation);
	}

	void readAnimNode(const Value &nodeVal, veAnimation *animation) {
		veAnimKeyValues *valuse = new veAnimKeyValues;
		valuse->setName(nodeVal[NAME_KEY.c_str()].GetString());

		if (nodeVal.HasMember(POSITIONS_KEY.c_str())) {
			const Value &timeAndpositions = nodeVal[POSITIONS_KEY.c_str()];
			for (unsigned int i = 0; i < timeAndpositions.Size();) {
				valuse->setPosition(timeAndpositions[i].GetDouble(), veVec3(timeAndpositions[i + 1].GetDouble(), timeAndpositions[i + 2].GetDouble(), timeAndpositions[i + 3].GetDouble()));
				i += 4;
			}
		}

		if (nodeVal.HasMember(ROTATIONS_KEY.c_str())) {
			const Value &timeAndrotations = nodeVal[ROTATIONS_KEY.c_str()];
			for (unsigned int i = 0; i < timeAndrotations.Size();) {
				valuse->setRotation(timeAndrotations[i].GetDouble(), veQuat(timeAndrotations[i + 1].GetDouble(), timeAndrotations[i + 2].GetDouble(), timeAndrotations[i + 3].GetDouble(), timeAndrotations[i + 4].GetDouble()));
				i += 5;
			}
		}

		if (nodeVal.HasMember(SCALES_KEY.c_str())) {
			const Value &timeAndscales = nodeVal[SCALES_KEY.c_str()];
			for (unsigned int i = 0; i < timeAndscales.Size();) {
				valuse->setScale(timeAndscales[i].GetDouble(), veVec3(timeAndscales[i + 1].GetDouble(), timeAndscales[i + 2].GetDouble(), timeAndscales[i + 3].GetDouble()));
				i += 4;
			}
		}

		animation->addAnimKeyValues(valuse);
	}

private:

	Document *_doucument;
	veAnimationContainer *_animationContainer;
	veSceneManager *_sceneManager;
	std::string _name;
	std::map<std::string, Document *> _doucumentMap;
};

VE_READERWRITER_REG("veanim", veFileReaderWriterANIMATION);