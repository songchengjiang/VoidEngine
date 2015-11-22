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
		gli::gl::format Format = GL.translate(tex.format());
		convertFormat(Format);
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
			_texture->setSwizzleMode(veTexture::SwizzleMode(Format.Swizzle[0]), veTexture::SwizzleMode(Format.Swizzle[1])
				, veTexture::SwizzleMode(Format.Swizzle[2]), veTexture::SwizzleMode(Format.Swizzle[3]));
			_texture->storage(mipmapLevels, internalFormat, pixelFormat, dataType);
		}

		if (!mipmapLevels.empty()) {
			for (auto &level : mipmapLevels) {
				VE_SAFE_DELETE_ARRAY(level.data);
			}
		}
	}

	void convertFormat(gli::gl::format &ft) {
		bool needSwizzled = false;
		switch (ft.External)
		{

		case gli::gl::EXTERNAL_RGB:
		case gli::gl::EXTERNAL_BGR: {
			ft.External = gli::gl::EXTERNAL_RGB;
			needSwizzled = true;
		}
			break;

		case gli::gl::EXTERNAL_BGRA: {
			ft.External = gli::gl::EXTERNAL_RGBA;
			needSwizzled = true;
		}
			break;

		case gli::gl::EXTERNAL_BGR_INTEGER: {
			ft.External = gli::gl::EXTERNAL_RGB_INTEGER;
			needSwizzled = true;
		}
			break;

		case gli::gl::EXTERNAL_BGRA_INTEGER: {
			ft.External = gli::gl::EXTERNAL_RGBA_INTEGER;
			needSwizzled = true;
		}
			break;

		default:
			break;
		}

		if (needSwizzled) {
			ft.Swizzle[0] = gli::gl::SWIZZLE_BLUE;
			ft.Swizzle[2] = gli::gl::SWIZZLE_RED;
		}
	}

private:

	veTexture *_texture;
	std::string _name;
	veSceneManager *_sceneManager;
};

VE_READERWRITER_REG("ktx", veFileReaderWriterKTX);