#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Texture.h"
#include "KernelCore/SceneManager.h"

class veFileReaderWriterDDS : public veFileReaderWriter
{
public:
	veFileReaderWriterDDS()
		: _texture(nullptr)
	{};
	virtual ~veFileReaderWriterDDS(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		auto fileData = veFile::instance()->readFileToBuffer(filePath);
		_sceneManager = sm;
		if (fileData){
			_name = name;
		}
		if (!_texture) veLog("veFileReaderWriterDDS: read %s failed!\n", filePath.c_str());
		return _texture;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readImage(){
	}

private:

	veTexture *_texture;
	std::string _name;
	veSceneManager *_sceneManager;
};

VE_READERWRITER_REG("dds", veFileReaderWriterDDS);
