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
			_texture = nullptr;
			_name = name;
			auto tex = gli::load_ktx(fileData->buffer, fileData->size);
			if (!tex.empty()) {
				readImage(tex);
			}
		}
		if (!_texture) veLog("veFileReaderWriterKTX: read %s failed!\n", filePath.c_str());
		return _texture;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readImage(const gli::texture &tex){
		gli::gl GL;
		gli::gl::format const Format = GL.translate(tex.format());
		GLint internalFormat = Format.Internal;
		GLenum pixelFormat = Format.External;
		GLenum dataType = Format.Type;
		veTexture::MipmapLevels mipmapLevels;
		if (!veTexture::isCompressedTex(internalFormat) && !veTexture::isSupportFormat(pixelFormat)) {
			veLog("veFileReaderWriterKTX: Not support format!");
			return;
		}
		if (tex.target() == gli::TARGET_2D) {
			_texture = _sceneManager->createTexture(_name, veTexture::TEXTURE_2D);
		}
		else if (tex.target() == gli::TARGET_3D){
			_texture = _sceneManager->createTexture(_name, veTexture::TEXTURE_3D);
		}

		for (std::size_t layer = 0; layer < tex.layers(); ++layer) {
			for (std::size_t face = 0; face < tex.faces(); ++face) {
				for (std::size_t level = 0; level < tex.levels(); ++level) {
					glm::tvec3<GLsizei> dims(tex.dimensions(level));
					size_t mipmapSize = tex.size(level);
					veTexture::MipmapLevel mipmap;
					mipmap.width  = dims.x;
					mipmap.height = dims.y;
					mipmap.depth  = dims.z;
					mipmap.data = new unsigned char[mipmapSize];
					mipmap.dataSize = mipmapSize;
					memcpy(mipmap.data, tex.data(layer, face, level), mipmapSize);
					mipmapLevels.push_back(mipmap);
				}
			}
		}

		if (_texture) {
			_texture->storage(mipmapLevels, internalFormat, pixelFormat, dataType);
		}

		if (!mipmapLevels.empty()) {
			for (auto &level : mipmapLevels) {
				VE_SAFE_DELETE_ARRAY(level.data);
			}
		}
	}

private:

	veTexture *_texture;
	std::string _name;
	veSceneManager *_sceneManager;
};

VE_READERWRITER_REG("ktx", veFileReaderWriterKTX);