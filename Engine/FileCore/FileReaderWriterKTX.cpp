#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Texture.h"
#include "KernelCore/SceneManager.h"
#include <gli.hpp>

class veFileReaderWriterKTX : public veFileReaderWriter
{
public:
	veFileReaderWriterKTX()
		: _texture(nullptr)
	{};
	virtual ~veFileReaderWriterKTX(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		auto fileData = veFile::instance()->readFileToBuffer(filePath);
		_sceneManager = sm;
		if (fileData){
			_name = name;
			auto tex = gli::load_ktx(fileData->buffer, fileData->size);
			readImage(tex);
		}
		if (!_texture) veLog("veFileReaderWriterKTX: read %s failed!\n", filePath.c_str());
		return _texture;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readImage(const gli::texture &tex){

	}

private:

	veTexture *_texture;
	std::string _name;
	veSceneManager *_sceneManager;
};

VE_READERWRITER_REG("ktx", veFileReaderWriterKTX);