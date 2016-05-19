#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Texture.h"
#include "KernelCore/SceneManager.h"
#include "libpng/include/png.h"

#if (VE_PLATFORM == VE_PLATFORM_WIN32)
#pragma comment(lib, "libpng/lib/libpng.lib")
#endif
class veFileReaderWriterPNG : public veFileReaderWriter
{
public:
	veFileReaderWriterPNG()
		: _texture(nullptr)
	{};
	virtual ~veFileReaderWriterPNG(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		auto fileData = veFile::instance()->readFileToBuffer(filePath);
        if (!fileData)
            return nullptr;
		_sceneManager = sm;
		if (fileData){
			_name = name;
			if (png_sig_cmp((unsigned char *)fileData->buffer, 0, 8) == 0){
				png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if (png_ptr){
					png_infop info_ptr = png_create_info_struct(png_ptr);
					if (info_ptr){
						if (!setjmp(png_jmpbuf(png_ptr))){
							png_set_read_fn(png_ptr, fileData, readDataFromMemory);
							readImage(png_ptr, info_ptr);
						}
						png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
					}
				}
			}
		}
		if (!_texture) veLog("veFileReaderWriterPNG: read %s failed!\n", filePath.c_str());
		return _texture;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readImage(png_structp png_ptr, png_infop info_ptr){
		GLint internalFormat = GL_RGB8;
		GLenum pixelFormat = GL_RGB;
		GLenum dataType = GL_UNSIGNED_BYTE;
		png_set_sig_bytes(png_ptr, 8);
		_currentByteIndex = 8;
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);
		png_byte color_type = png_get_color_type(png_ptr, info_ptr);
		int width = png_get_image_width(png_ptr, info_ptr);
		int height = png_get_image_height(png_ptr, info_ptr);
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
		unsigned char *buffer = nullptr;
		unsigned int index = 0;
		if (color_type == PNG_COLOR_TYPE_RGB_ALPHA){
			buffer = new unsigned char[width * height * 4];
			for (int y = height - 1; 0 <= y; --y){
				for (int x = 0; x < width * 4;){
					buffer[index++] = row_pointers[y][x++];//r
					buffer[index++] = row_pointers[y][x++];//g
					buffer[index++] = row_pointers[y][x++];//b
					buffer[index++] = row_pointers[y][x++];//a
				}
			}
			internalFormat = GL_RGBA8;
			pixelFormat = GL_RGBA;
		}
		else if (color_type == PNG_COLOR_TYPE_RGB){
			buffer = new unsigned char[width * height * 3];
			for (int y = height - 1; 0 <= y; --y){
				for (int x = 0; x < width * 3;){
					buffer[index++] = row_pointers[y][x++];//r
					buffer[index++] = row_pointers[y][x++];//g
					buffer[index++] = row_pointers[y][x++];//b
				}
			}
		}
		else if (color_type == PNG_COLOR_TYPE_GRAY){
			buffer = new unsigned char[width * height];
			for (int y = height - 1; 0 <= y; --y){
				for (int x = 0; x < width;){
					buffer[index++] = row_pointers[y][x++];//gray
				}
			}
			internalFormat = GL_R8;
			pixelFormat = GL_RED;
		}
		else if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA){
			buffer = new unsigned char[width * height * 2];
			for (int y = height - 1; 0 <= y; --y){
				for (int x = 0; x < width * 2;){
					buffer[index++] = row_pointers[y][x++];//gray
					buffer[index++] = row_pointers[y][x++];//alpha
				}
			}
			internalFormat = GL_RG8;
			pixelFormat = GL_RG;
		}

		if (buffer){
			_texture = _sceneManager->createTexture(_name);
			_texture->storage(width, height, 1, internalFormat, pixelFormat, dataType, buffer, (unsigned int)log2(width) + 1);
			delete[] buffer;
		}
	}

	static void readDataFromMemory(png_structp png_ptr, png_bytep outBytes,
		png_size_t byteCountToRead) {
		png_voidp io_ptr = png_get_io_ptr(png_ptr);
		veFileData *fileData = static_cast<veFileData *>(io_ptr);
		memcpy(outBytes, &fileData->buffer[_currentByteIndex], byteCountToRead);
		_currentByteIndex += byteCountToRead;
	}
private:

	veTexture *_texture;
	std::string _name;
	veSceneManager *_sceneManager;
	static png_size_t  _currentByteIndex;
};

png_size_t veFileReaderWriterPNG::_currentByteIndex = 0;
