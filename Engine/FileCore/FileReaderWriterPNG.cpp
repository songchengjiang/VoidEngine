#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Image.h"
#include "libpng/include/png.h"

class veFileReaderWriterPNG : public veFileReaderWriter
{
public:
	veFileReaderWriterPNG()
		: _image(nullptr)
	{};
	virtual ~veFileReaderWriterPNG(){};

	virtual void* readFile(const std::string &filePath){
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		_name = fullPath.substr(fullPath.find_last_of("/\\") + 1);
		FILE *fp = fopen(fullPath.c_str(), "rb");
		if (fp){
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
		}
		fclose(fp);
		return _image;
	}

	virtual bool writeFile(void *data, const std::string &filePath){
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

		if (buffer){
			_image = new veImage;
			_image->setName(_name);
			_image->set(width, height, 1, internalFormat, pixelFormat, dataType, buffer);
			delete[] buffer;
		}
	}

	static png_uint_32 getRowBytes(png_uint_32 width){
		if ((width * 3) % 4 == 0){
			return width * 3;
		}
		else{
			return ((width * 3) / 4 + 1) * 4;
		}
	}

private:

	veImage *_image;
	std::string _name;
};

VE_READERWRITER_REG("png", veFileReaderWriterPNG);
