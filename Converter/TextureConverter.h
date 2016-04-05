#ifndef _TEXTURE_CONVERTER_
#define _TEXTURE_CONVERTER_
#include "Converter.h"
#include <gli/gli.hpp>

class TextureConverter : public Converter
{
public:
	TextureConverter();
	~TextureConverter();

	virtual void convert(const std::string &filePath, const std::string &outFile) override;

private:


};

#endif