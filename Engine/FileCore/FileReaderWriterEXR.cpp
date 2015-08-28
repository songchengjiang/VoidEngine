#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Image.h"
#include "openexr/include/ImfRgbaFile.h"
#include "openexr/include/ImfRgba.h"
#include "openexr/include/ImfArray.h"
#include "openexr/include/ImathBox.h"

#ifdef _DEBUG
#pragma comment(lib, "openexr/lib/Half_d.lib")
#pragma comment(lib, "openexr/lib/Iex-2_2_d.lib")
#pragma comment(lib, "openexr/lib/IlmImf-2_2_d.lib")
#pragma comment(lib, "openexr/lib/IlmThread-2_2_d.lib")
#else
#pragma comment(lib, "openexr/lib/Half.lib")
#pragma comment(lib, "openexr/lib/Iex-2_2.lib")
#pragma comment(lib, "openexr/lib/IlmImf-2_2.lib")
#pragma comment(lib, "openexr/lib/IlmThread-2_2.lib")
#endif
class veFileReaderWriterEXR : public veFileReaderWriter
{
public:
	veFileReaderWriterEXR()
		: _image(nullptr)
	{};
	virtual ~veFileReaderWriterEXR(){};

	virtual void* readFile(const std::string &filePath){
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		Imf::Array2D<Imf::Rgba> pixels;
		Imf::RgbaInputFile file(fullPath.c_str());
		if (file.isComplete()){
			Imath::Box2i dw = file.dataWindow();
			int width = dw.max.x - dw.min.x + 1;
			int height = dw.max.y - dw.min.y + 1;
			pixels.resizeErase(height, width);
			file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
			file.readPixels(dw.min.y, dw.max.y);
			readImage(pixels);
		}
		if (!_image) VE_PRINT(std::string("veFileReaderWriterEXR: read ") + filePath + std::string(" failed!"));
		return _image;
	}

	virtual bool writeFile(void *data, const std::string &filePath){
		return true;
	}

private:

	void readImage(Imf::Array2D<Imf::Rgba> &pixels){
		GLint internalFormat = GL_RGBA32F;
		GLenum pixelFormat = GL_RGBA;
		GLenum dataType = GL_FLOAT;
		float *buffer = new float[pixels.width() * pixels.height() * 4];
		unsigned int index = 0;
		for (int h = pixels.height() - 1; 0 <= h; --h){
			for (int w = 0; w < pixels.width(); ++w){
				buffer[index++] = pixels[h][w].r;
				buffer[index++] = pixels[h][w].g;
				buffer[index++] = pixels[h][w].b;
				buffer[index++] = pixels[h][w].a;
			}
		}
		_image = new veImage;
		_image->set(pixels.width(), pixels.height(), 1, internalFormat, pixelFormat, dataType, (unsigned char *)buffer);
	}

private:

	veImage *_image;
	std::string _name;
};

VE_READERWRITER_REG("exr", veFileReaderWriterEXR);
