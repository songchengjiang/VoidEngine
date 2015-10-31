#include "FileReaderWriter.h"
#include <unordered_map>
#include "KernelCore/Texture.h"
#include "KernelCore/SceneManager.h"
#include "openexr/include/ImfRgbaFile.h"
#include "openexr/include/ImfRgba.h"
#include "openexr/include/ImfIO.h"
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

class MemStream : public Imf::IStream
{
public:

public:
	MemStream(char *exrbuf, int exrsize)
		: Imf::IStream("")
		, _exrsize(exrsize)
		, _exrpos(0)
		, _exrbuf(exrbuf)
	{}
	virtual ~MemStream(){}

	virtual bool read(char c[/*n*/], int n) {
		if (n + _exrpos < _exrsize)
		{
			memcpy(c, &(_exrbuf[_exrpos]), n);
			_exrpos += n;
			return true;
		}
		else
			return false;
	}
	virtual Imf::Int64 tellg() {
		return _exrpos;
	}
	virtual void seekg(Imf::Int64 pos) {
		_exrpos = pos;
	}

	virtual void clear() {

	}

private:

	Imf::Int64 _exrpos;
	Imf::Int64 _exrsize;
	char *_exrbuf;
};
class veFileReaderWriterEXR : public veFileReaderWriter
{
public:
	veFileReaderWriterEXR()
		: _texture(nullptr)
	{};
	virtual ~veFileReaderWriterEXR(){};

	virtual void* readFile(veSceneManager *sm, const std::string &filePath, const std::string &name, const veFileParam &param) override{
		std::string fullPath = veFile::instance()->getFullFilePath(filePath);
		auto fileData = veFile::instance()->readFileToBuffer(filePath);
		_sceneManager = sm;
		Imf::Array2D<Imf::Rgba> pixels;
		MemStream memStream(fileData->buffer, fileData->size);
		Imf::RgbaInputFile file(memStream);
		if (file.isComplete()){
			_name = name;
			Imath::Box2i dw = file.dataWindow();
			int width = dw.max.x - dw.min.x + 1;
			int height = dw.max.y - dw.min.y + 1;
			pixels.resizeErase(height, width);
			file.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
			file.readPixels(dw.min.y, dw.max.y);
			readImage(pixels);
		}
		if (!_texture) veLog("veFileReaderWriterEXR: read %s failed!\n", filePath.c_str());
		return _texture;
	}

	virtual bool writeFile(veSceneManager *sm, void *data, const std::string &filePath) override{
		return true;
	}

private:

	void readImage(Imf::Array2D<Imf::Rgba> &pixels){
		GLint internalFormat = GL_RGBA32F;
		GLenum pixelFormat = GL_RGBA;
		GLenum dataType = GL_FLOAT;
		float *buffer = new float[pixels.width() * pixels.height() * 4];
		unsigned int index = 0;
		for (int h = int(pixels.height() - 1); 0 <= h; --h){
			for (int w = 0; w < int(pixels.width()); ++w){
				buffer[index++] = pixels[h][w].r;
				buffer[index++] = pixels[h][w].g;
				buffer[index++] = pixels[h][w].b;
				buffer[index++] = pixels[h][w].a;
			}
		}
		_texture = _sceneManager->createTexture(_name);
		_texture->storage(int(pixels.width()), int(pixels.height()), 1, internalFormat, pixelFormat, dataType, (unsigned char *)buffer);
	}

private:

	veTexture *_texture;
	std::string _name;
	veSceneManager *_sceneManager;
};

VE_READERWRITER_REG("exr", veFileReaderWriterEXR);
