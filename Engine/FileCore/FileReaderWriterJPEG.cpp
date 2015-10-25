#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Texture.h"
#include "KernelCore/SceneManager.h"
extern "C" {
#if defined(_MSC_VER)
#include "libjpeg/include/win/jpeglib.h"
#elif defined(__APPLE_CC__)
#include "libjpeg/include/mac/jpeglib.h"
#endif
}
#if defined(_MSC_VER)
#pragma comment(lib, "libjpeg/lib/libjpeg.lib")
#endif
class veFileReaderWriterJPEG : public veFileReaderWriter
{
public:
	veFileReaderWriterJPEG()
		: _texture(nullptr)
	{};
	virtual ~veFileReaderWriterJPEG(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		_sceneManager = sm;
		FILE *fp = fopen(fullPath.c_str(), "rb");
		if (fp){
			_name = name;
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);
			jpeg_stdio_src(&cinfo, fp);
			jpeg_read_header(&cinfo, true);
			readImage(cinfo);
			jpeg_destroy_decompress(&cinfo);
			fclose(fp);
		}
		if (!_texture) veLog(std::string("veFileReaderWriterJPEG: read ") + filePath + std::string(" failed!"));
		return _texture;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readImage(jpeg_decompress_struct &cinfo){
		int width = cinfo.image_width;
		int height = cinfo.image_height;
		int components = cinfo.num_components;
		if (components < 1) return;
		GLint internalFormat = GL_RGB8;
		GLenum pixelFormat = GL_RGB;
		GLenum dataType = GL_UNSIGNED_BYTE;
		if (!getFormat(cinfo, internalFormat, pixelFormat)) return;
		unsigned char *buffer = new unsigned char[width * height * components];
		jpeg_start_decompress(&cinfo);
		JSAMPROW row_pointer[1];
		while (cinfo.output_scanline < cinfo.output_height)
		{
			row_pointer[0] = &buffer[(cinfo.output_height - cinfo.output_scanline - 1) *width *cinfo.num_components];
			jpeg_read_scanlines(&cinfo, row_pointer, 1);
		}
		jpeg_finish_decompress(&cinfo);
		_texture = _sceneManager->createTexture(_name);
		_texture->storage(width, height, 1, internalFormat, pixelFormat, dataType, buffer);
		delete[] buffer;
	}

	bool getFormat(jpeg_decompress_struct &cinfo, GLint &internalFormat, GLenum &pixelFormat){
		if (cinfo.num_components == 1){
			internalFormat = GL_R8;
			pixelFormat = GL_RED;
			return true;
		}
		else if (cinfo.num_components == 3){
			internalFormat = GL_RGB8;
			pixelFormat = GL_RGB;
			return true;
		}
		return false;
	}

private:

	veTexture *_texture;
	std::string _name;
	veSceneManager *_sceneManager;
};

VE_READERWRITER_REG("jpg", veFileReaderWriterJPEG);