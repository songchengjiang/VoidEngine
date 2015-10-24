#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Image.h"
#include "libpng/include/png.h"

#pragma comment(lib, "libpng/lib/libpng.lib")
class veFileReaderWriterPNG : public veFileReaderWriter
{
public:
	veFileReaderWriterPNG()
		: _image(nullptr)
	{};
	virtual ~veFileReaderWriterPNG(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		FILE *fp = fopen(fullPath.c_str(), "rb");
		if (fp){
			_name = name;
			_fileName = filePath;
			unsigned char header[8];
			fread(header, 1, 8, fp);
			if (png_sig_cmp(header, 0, 8) == 0){
				png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
				if (png_ptr){
					png_infop info_ptr = png_create_info_struct(png_ptr);
					if (info_ptr){
						if (!setjmp(png_jmpbuf(png_ptr))){
							png_init_io(png_ptr, fp);
							readImage(png_ptr, info_ptr);
						}
						png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
					}
				}
			}
			fclose(fp);
		}
		if (!_image) veLog(std::string("veFileReaderWriterPNG: read ") + filePath + std::string(" failed!"));
		return _image;
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
			_image = new veImage;
			_image->setName(_name);
			_image->setFileName(_fileName);
			_image->set(width, height, 1, internalFormat, pixelFormat, dataType, buffer);
			delete[] buffer;
		}
	}

private:

	veImage *_image;
	std::string _name;
	std::string _fileName;
};

VE_READERWRITER_REG("png", veFileReaderWriterPNG);
